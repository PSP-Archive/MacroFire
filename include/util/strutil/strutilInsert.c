/* strutilInsert.c */

#include "../strutil.h"

int strutilInsert( char *__restrict__ dest, const char *__restrict__ src, size_t max )
{
	return strutilNInsert( dest, src, strlen( src ), max );
}
