/* strutilCopy.c */

#include "../strutil.h"

int strutilCopy( char *__restrict__ dest, const char *__restrict__ src, size_t max )
{
	int workmax;
	
	if( max <= 0 ) return 0;
	
	max--;
	workmax = max;
	while( workmax-- && *src != '\0' ){
		*dest++ = *src++;
	}
	
	*dest = '\0';
	
	return max - workmax;
}
