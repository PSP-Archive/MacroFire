/* strutilCat */

#include "../strutil.h"

int strutilCat( char *__restrict__ dest, const char *__restrict__ src, size_t max )
{
	size_t off = strlen( dest );
	
	max -= off;
	if( max <= 0 ) return off;
	
	off += strutilCopy( dest + off, src, max );
	
	return off;
}
