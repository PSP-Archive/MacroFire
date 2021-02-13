/* isgraph.c */

#include <ctype.h>

int isgraph( int c )
{
	if( iscntrl( c ) || isspace( c ) ) return 0;
	return 1;
}
