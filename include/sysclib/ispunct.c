/* ispunct.c */

#include <ctype.h>

int ispunct( int c )
{
	if( iscntrl( c ) || isalnum( c ) || isspace( c ) ) return 0;
	return 1;
}
