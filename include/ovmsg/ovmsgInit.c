/* ovmsgInit.c */

#include "ovmsg_types.h"

OvmsgUID ovmsgInit( void )
{
	int ret;
	struct ovmsg_params *params = memoryExalloc( "OvmsgParams", MEMORY_USER, 0, sizeof( struct ovmsg_params ) + OVMSG_MESSAGE_MAX_LENGTH, PSP_SMEM_Low, NULL );
	if( ! params ) return CG_ERROR_NOT_ENOUGH_MEMORY;
	
	params->selfThreadId = 0;
	
	params->ctrlEvId = sceKernelCreateEventFlag( "OvmsgCtrl", 0, 0, NULL );
	if( params->ctrlEvId < 0 ){
		ret = params->ctrlEvId;
		memoryFree( params );
		return ret;
	}
	params->workEvId = sceKernelCreateEventFlag( "OvmsgWork", 0, 0, NULL );
	if( params->workEvId < 0 ){
		ret = params->workEvId;
		sceKernelDeleteEventFlag( params->ctrlEvId );
		memoryFree( params );
		return ret;
	}
	
	params->x = 10;
	params->y = 250;
	params->fgcolor = 0xffeeeeee;
	params->bgcolor = 0x88000000;
	params->message = (char *)( (uintptr_t)params + sizeof( struct ovmsg_params ) );
	
	params->displayStart = 0;
	params->displaySec   = 0;
	
	return (OvmsgUID)params;
}
