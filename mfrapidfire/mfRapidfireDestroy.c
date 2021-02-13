/* mfRapidfireDestroy.c */

#include "mfrapidfire_types.h"

void mfRapidfireDestroy( MfRapidfireUID uid )
{
	memoryFree( (void *)uid );
}
