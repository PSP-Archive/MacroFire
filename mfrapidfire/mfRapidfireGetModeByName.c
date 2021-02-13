/* mfRapidfireGetModeByName.c */

#include "mfrapidfire_types.h"

MfRapidfireMode mfRapidfireGetModeByName( const char *name )
{
	if( strcasecmp( name, MF_RAPIDFIRE_NAME_RAPID ) == 0 ){
		return MF_RAPIDFIRE_MODE_RAPID;
	} else if( strcasecmp( name, MF_RAPIDFIRE_NAME_AUTORAPID ) == 0 ){
		return MF_RAPIDFIRE_MODE_AUTORAPID;
	} else if( strcasecmp( name, MF_RAPIDFIRE_NAME_HOLD ) == 0 ){
		return MF_RAPIDFIRE_MODE_HOLD;
	} else if( strcasecmp( name, MF_RAPIDFIRE_NAME_AUTOHOLD ) == 0 ){
		return MF_RAPIDFIRE_MODE_AUTOHOLD;
	} else{
		return MF_RAPIDFIRE_MODE_NORMAL;
	}
}
