/* strutilNInsert.c */

#include "../strutil.h"

int strutilNInsert( char *__restrict__ dest, const char *__restrict__ src, size_t n, size_t max )
{
	size_t movelen = strlen( dest ) + 1;
	
	if( movelen + n >= max ) movelen -= movelen + n - max;
	
	memmove( dest, dest + n, movelen );
	strcpy( dest, src );
	
	return movelen + n;
}
