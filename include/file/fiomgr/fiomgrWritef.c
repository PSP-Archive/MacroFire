/* fiomgrWritef.c */

#include "fiomgr_types.h"

int fiomgrWritef( FiomgrHandle fh, char *buf, size_t buflen, const char *format, ... )
{
	va_list ap;
	
	if( ! buf ) return CG_ERROR_INVALID_ARGUMENT;
	
	va_start( ap, format );
	vsnprintf( buf, buflen, format, ap );
	va_end( ap );
	
	return fiomgrWrite( fh, buf, strlen( buf ) );
}
