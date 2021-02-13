/* ovmsgStart.c */

#include "ovmsg_types.h"

static int ovmsg_main( SceSize arglen, void *argp );

int ovmsgStart( OvmsgUID uid, const char *name, int pri )
{
	SceUID thid = sceKernelCreateThread( name, ovmsg_main, pri, 0x2000, 0, 0 );
	if( thid < 0 ) return thid;
	
	((struct ovmsg_params *)uid)->selfThreadId = thid;
	
	sceKernelStartThread( thid, sizeof( OvmsgUID* ), &uid );
	return CG_ERROR_OK;
}

static int ovmsg_main( SceSize arglen, void *argp )
{
	struct ovmsg_params *params = *(struct ovmsg_params **)argp;
	int ret = 0;
	
	for( ;; ){
		ret = sceKernelWaitEventFlag( params->workEvId, OVMSG_EVENT_PRINT | OVMSG_EVENT_SUSPEND | OVMSG_EVENT_SHUTDOWN, PSP_EVENT_WAITOR | PSP_EVENT_WAITCLEAR, &(params->flags), 0 );
		if( ret < 0 || ( params->flags & OVMSG_EVENT_SHUTDOWN ) ){
			break;
		} else if( params->flags & OVMSG_EVENT_PRINT ){
			sceKernelSetEventFlag( params->ctrlEvId, OVMSG_EVENT_OK );
			sceKernelWaitEventFlag( params->workEvId, OVMSG_EVENT_OK, PSP_EVENT_WAITOR | PSP_EVENT_WAITCLEAR, NULL, 0 );
		} else if( params->flags & OVMSG_EVENT_SUSPEND ){
			continue;
		}
		
		sceKernelLibcTime( &(params->displayStart) );
		
		params->w = strlen( params->message ) + 1;
		params->h = 2;
		
		params->displaySec = ( params->w - 1 ) / 7;
		if( params->displaySec < 3 ) params->displaySec = 3;
#ifdef PB_SJIS_SUPPORT
		params->w = pbMeasureString( params->message ) + pbOffsetChar( 1 );
#else
		params->w = pbOffsetChar( params->w );
#endif
		params->h = pbOffsetLine( params->h );
		
		while( ( sceKernelLibcTime( NULL ) - params->displayStart < params->displaySec ) ){
			sceDisplayWaitVblankStart();
			if( sceDisplayGetFrameBuf( &(params->fb), &(params->bufwidth), (int *)&(params->pxformat), PSP_DISPLAY_SETBUF_NEXTFRAME ) == 0 && params->fb && params->bufwidth ){
				pbInit();
				pbEnable( PB_NO_CACHE | PB_BLEND );
				pbSetDisplayBuffer( params->pxformat, params->fb, params->bufwidth );
				pbApply();
				pbFillRectRel( params->x, params->y, params->w, params->h, params->bgcolor );
				pbPrint( params->x + ( pbOffsetChar( 1 ) >> 1 ), params->y + ( pbOffsetLine( 1 ) >> 1 ), params->fgcolor, PB_TRANSPARENT, params->message );
			}
			//sceKernelDelayThread( 1000 );
			if( sceKernelPollEventFlag( params->workEvId, OVMSG_EVENT_PRINT | OVMSG_EVENT_SUSPEND | OVMSG_EVENT_SHUTDOWN, PSP_EVENT_WAITOR, &(params->flags) ) >= 0 ){
				break;
			}
		}
	}
	
	params->selfThreadId = 0;
	
	sceKernelSetEventFlag( params->ctrlEvId, OVMSG_EVENT_OK );
	return sceKernelExitDeleteThread( 0 );
}

