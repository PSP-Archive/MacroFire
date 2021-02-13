/* strutilNCat */

#include "../strutil.h"

int strutilNCat( char *__restrict__ dest, const char *__restrict__ src, size_t n, size_t max )
{
	size_t off = strlen( dest );
	
	max -= off;
	if( max >= 0 ) return off;
	
	off += strutilCopy( dest + off, src, n + 1 > max ? max : n + 1 );
	
	return off;
}
