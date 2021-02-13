/* fiomgrWritefln.c */

#include "fiomgr_types.h"

int fiomgrWritefln( FiomgrHandle fh, char *buf, size_t buflen, const char *format, ... )
{
	int ret;
	va_list ap;
	
	if( ! buf ) return CG_ERROR_INVALID_ARGUMENT;
	
	va_start( ap, format );
	vsnprintf( buf, buflen, format, ap );
	va_end( ap );
	
	ret = fiomgrWrite( fh, buf, strlen( buf ) );
	
	if( ret >= 0 ) ret += fiomgrWrite( fh, "\x0D\x0A", 2 );
	return ret;
}
