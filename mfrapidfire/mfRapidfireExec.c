/* mfRapidfireExec.c */

#include "mfrapidfire_types.h"

void mfRapidfireExec( MfRapidfireUID uid, MfHookAction action, SceCtrlData *pad )
{
	unsigned int i;
	unsigned int delay;
	uint64_t cur_tick;
	MfRapidfireParams *params = (MfRapidfireParams *)uid;
	
	sceRtcGetCurrentTick( &cur_tick );
	delay = ( cur_tick - params->lastTick ) / 1000;
	
	if( mfIsRunningApp( MF_APP_WEBBROWSER ) ) return;
	
	for( i = 0; i < MF_RAPIDFIRE_NUMBER_OF_AVAIL_BUTTONS; i++ ){
		if( params->pref[i].mode == MF_RAPIDFIRE_MODE_AUTOHOLD ){
			pad->Buttons ^= params->pref[i].button;
		} else if( params->pref[i].mode == MF_RAPIDFIRE_MODE_HOLD ){
			if( pad->Buttons & params->pref[i].button ){
				if( ! ( params->pref[i].bitFlags & MF_RAPIDFIRE_FLAGS_HOLD_ISREALHOLD ) ){
					params->pref[i].bitFlags ^= MF_RAPIDFIRE_FLAGS_HOLD_ENABLE;
					params->pref[i].bitFlags |= MF_RAPIDFIRE_FLAGS_HOLD_ISREALHOLD;
				}
			} else{
				params->pref[i].bitFlags &= ~MF_RAPIDFIRE_FLAGS_HOLD_ISREALHOLD;
			}
			
			if( params->pref[i].bitFlags & MF_RAPIDFIRE_FLAGS_HOLD_ENABLE ){
				pad->Buttons |= params->pref[i].button;
			} else if( params->pref[i].bitFlags & MF_RAPIDFIRE_FLAGS_HOLD_ISREALHOLD ){
				pad->Buttons ^= params->pref[i].button;
			}
		} else if( params->pref[i].mode == MF_RAPIDFIRE_MODE_RAPID || params->pref[i].mode == MF_RAPIDFIRE_MODE_AUTORAPID ){
			uint64_t cur_tick;
			sceRtcGetCurrentTick( &cur_tick );
			
			if( action == MF_UPDATE && ( delay > params->pref[i].nextDelay ) ){
				unsigned int overtime;
				
				params->lastTick = cur_tick;
				params->pref[i].bitFlags ^= MF_RAPIDFIRE_FLAGS_RAPID_PRESS;
				
				overtime = delay - params->pref[i].nextDelay;
				
				if( params->pref[i].bitFlags & MF_RAPIDFIRE_FLAGS_RAPID_PRESS ){
					params->pref[i].nextDelay = params->pref[i].pressDelay;
				} else{
					params->pref[i].nextDelay = params->pref[i].releaseDelay;
				}
				
				if( params->pref[i].nextDelay > overtime ){
					params->pref[i].nextDelay -= overtime;
				} else{
					params->pref[i].nextDelay = 0;
				}
				
			}
			
			if( params->pref[i].mode == MF_RAPIDFIRE_MODE_AUTORAPID && ( pad->Buttons & params->pref[i].button ) ){
				pad->Buttons ^= params->pref[i].button;
			} else if( params->pref[i].mode == MF_RAPIDFIRE_MODE_AUTORAPID || ( pad->Buttons & params->pref[i].button ) ){
				pad->Buttons |= params->pref[i].button;
				if( ! ( params->pref[i].bitFlags & MF_RAPIDFIRE_FLAGS_RAPID_PRESS ) ){
					pad->Buttons ^= params->pref[i].button;
				}
			}
		}
	}
}
