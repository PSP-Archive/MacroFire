/* mfRapidfireSetMode.c */

#include "mfrapidfire_types.h"

void mfRapidfireSetMode( MfRapidfireUID uid, unsigned int buttons, MfRapidfireMode mode, unsigned int pdelay, unsigned rdelay )
{
	unsigned int i;
	for( i = 0; i < MF_RAPIDFIRE_NUMBER_OF_AVAIL_BUTTONS; i++ ){
		if( buttons & ((MfRapidfireParams *)uid)->pref[i].button ){
			((MfRapidfireParams *)uid)->pref[i].mode         = mode;
			((MfRapidfireParams *)uid)->pref[i].pressDelay   = pdelay;
			((MfRapidfireParams *)uid)->pref[i].releaseDelay = rdelay;
			((MfRapidfireParams *)uid)->pref[i].bitFlags     = 0;
		}
	}
}
