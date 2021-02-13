/* mfRapidfireBind.c */

#define MFRAPIDFIRE_INIT
#include "mfrapidfire_types.h"
#undef MFRAPIDFIRE_INIT

MfRapidfireUID mfRapidfireBind( MfRapidfireParams *params )
{
	unsigned int i;
	
	if( ! params ) return 0;
	
	params->lastTick = 0;
	
	for( i = 0; i < MF_RAPIDFIRE_NUMBER_OF_AVAIL_BUTTONS; i++ ){
		params->pref[i].mode         = MF_RAPIDFIRE_MODE_NORMAL;
		params->pref[i].pressDelay   = 0;
		params->pref[i].releaseDelay = 0;
		params->pref[i].nextDelay    = 0;
		params->pref[i].bitFlags     = 0;
	}
	
	params->pref[0].button  = PSP_CTRL_SELECT;
	params->pref[1].button  = PSP_CTRL_START;
	params->pref[2].button  = PSP_CTRL_UP;
	params->pref[3].button  = PSP_CTRL_RIGHT;
	params->pref[4].button  = PSP_CTRL_DOWN;
	params->pref[5].button  = PSP_CTRL_LEFT;
	params->pref[6].button  = PSP_CTRL_LTRIGGER;
	params->pref[7].button  = PSP_CTRL_RTRIGGER;
	params->pref[8].button  = PSP_CTRL_TRIANGLE;
	params->pref[9].button  = PSP_CTRL_CIRCLE;
	params->pref[10].button = PSP_CTRL_CROSS;
	params->pref[11].button = PSP_CTRL_SQUARE;
	
	return (MfRapidfireUID)params;
}
