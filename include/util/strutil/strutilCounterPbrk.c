/* strutilCounterPbrk */

#include "../strutil.h"

char *strutilCounterPbrk( const char *__restrict__ src, const char *__restrict__ search )
{
	while( *src != '\0' ){
		if( ! strchr( search, *src ) ){
			return (char *)src;
		}
		src++;
	}
	return strchr( search, *src );
}
