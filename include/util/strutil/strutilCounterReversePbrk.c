/* strutilCounterReversePbrk */

#include "../strutil.h"

char *strutilCounterReversePbrk( const char *__restrict__ src, const char *__restrict__ search )
{
	const char *new_str = src + strlen( src );
	
	while( new_str != src ){
		if( ! strchr( search, *new_str ) ) return (char *)new_str;
		new_str--;
	}
	return strchr( search, *new_str );
}
