/* mfRapidfireGetNameByMode.c */

#include "mfrapidfire_types.h"

const char *mfRapidfireGetNameByMode( MfRapidfireMode mode )
{
	switch( mode ){
		case MF_RAPIDFIRE_MODE_RAPID:     return MF_RAPIDFIRE_NAME_RAPID;
		case MF_RAPIDFIRE_MODE_AUTORAPID: return MF_RAPIDFIRE_NAME_AUTORAPID;
		case MF_RAPIDFIRE_MODE_HOLD:      return MF_RAPIDFIRE_NAME_HOLD;
		case MF_RAPIDFIRE_MODE_AUTOHOLD:  return MF_RAPIDFIRE_NAME_AUTOHOLD;
		default:                          return MF_RAPIDFIRE_NAME_NORMAL;
	}
}
