/* mfRapidfireReadEntry.c */

#include "mfrapidfire_types.h"

bool mfRapidfireReadEntry( MfRapidfireUID uid, MfRapidfireData *data, unsigned int *save )
{
	if( ! save || *save >= MF_RAPIDFIRE_NUMBER_OF_AVAIL_BUTTONS ) return false;
	
	*data = ((MfRapidfireParams *)uid)->pref[(*save)++];
	
	return true;
}
