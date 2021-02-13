/* mfRapidfireNew.c */

#include "mfrapidfire_types.h"

MfRapidfireUID mfRapidfireNew( void )
{
	MfRapidfireParams *params = (MfRapidfireParams *)mfMemoryAllocLo( sizeof( MfRapidfireParams ) );
	return mfRapidfireBind( params );
}
