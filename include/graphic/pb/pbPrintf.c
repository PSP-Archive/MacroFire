/* pbPrintf.c */

#include "pb_types.h"

int pbPrintf( int x, int y, unsigned int fg, unsigned int bg, const char *format, ... )
{
	char str[255];
	va_list ap;
	
	va_start( ap, format );
	vsnprintf( str, sizeof( str ), format, ap );
	va_end( ap );
	
	return pbPrint( x, y, fg, bg, str );
}
