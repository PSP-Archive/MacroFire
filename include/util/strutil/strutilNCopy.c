/* strutilNCopy.c */

#include "../strutil.h"

int strutilNCopy( char *__restrict__ dest, const char *__restrict__ src, size_t n, size_t max )
{
	return strutilCopy( dest, src, n + 1 > max ? max : n + 1 );
}
