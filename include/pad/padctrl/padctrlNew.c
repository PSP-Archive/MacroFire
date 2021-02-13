/* padctrlNew.c */

#include "padctrl_types.h"

PadctrlUID padctrlNew( void )
{
	struct padctrl_params *params = memoryExalloc( "PadctrlParams", MEMORY_USER, 0, sizeof( struct padctrl_params ), PSP_SMEM_High, NULL );
	if( ! params ) return 0;
	
	sceCtrlSetSamplingMode( PSP_CTRL_MODE_ANALOG );
	
	params->buttons  = 0;
	padctrlResetRepeat( (PadctrlUID)params );
	
	return (PadctrlUID)params;
}


void __padctrl_proc( PadctrlUID uid, SceCtrlData *pad, int count )
{
	struct padctrl_params *params = (struct padctrl_params *)uid;
	uint64_t current_tick;
	uint64_t delay_tick;
	
	if( params->prePressed ){
		if( params->prePressed == ( pad->Buttons & PADUTIL_PAD_NORMAL_BUTTONS ) ){
			pad->Buttons &= ~PADUTIL_PAD_NORMAL_BUTTONS;
		} else{
			params->prePressed = 0;
		}
	}
	
	if( ! pad->Buttons ) return;
	
	sceRtcGetCurrentTick( &current_tick );
	delay_tick = params->doRepeat ? PADCTRL_REPEAT_NEXT_DELAY : PADCTRL_REPEAT_START_DELAY;
	
	if( pad->Buttons == params->lastButtons ){
		if( current_tick - params->lastTick >= delay_tick ){
			unsigned int button, buttons = 0;
			
			params->lastTick = current_tick;
			params->doRepeat = true;
			
			for( button = 1; button; button <<= 1 ){
				if( ( pad->Buttons & button ) && ( params->buttons & button ) ) buttons |= button;
			}
			pad->Buttons = buttons;
		} else{
			pad->Buttons &= PADUTIL_PAD_TOGGLE_BUTTONS;
		}
	} else{
		params->lastTick    = current_tick;
		params->lastButtons = pad->Buttons;
		params->doRepeat    = false;
	}
	
	if( count > 1 ){
		int i;
		for( i = 1; i < count; i++ ){
			memcpy( (void *)&pad[i], (void *)&pad[0], sizeof( SceCtrlData ) );
		}
	}
}
