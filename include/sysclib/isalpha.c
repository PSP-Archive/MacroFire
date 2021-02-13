/* isalpha.c */

#include <ctype.h>

int isalpha( int c )
{
	if( islower( c ) || isupper( c ) ) return 1;
	return 0;
}
