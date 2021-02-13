/* snprintf.c */

#include "xprintf_types.h"

int snprintf( char *buf, size_t n, const char *fmt, ... )
{
	va_list ap;
	int ret;
	
	va_start( ap, fmt );
	ret = vsnprintf( buf, n, fmt, ap );
	va_end( ap );
	
	return ret;
}
