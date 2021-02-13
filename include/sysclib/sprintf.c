/* sprintf.c */

#include "xprintf_types.h"

int sprintf( char *buf, const char *fmt, ... )
{
	va_list ap;
	int ret;
	
	va_start( ap, fmt );
	ret = vsprintf( buf, fmt, ap );
	va_end( ap );
	
	return ret;
}
