/* isprint.c */

#include <ctype.h>

int isprint( int c )
{
	if( ! iscntrl( c ) ) return 1;
	return 0;
}
