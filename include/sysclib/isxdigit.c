/* isxdigit.c */

#include <ctype.h>

int isxdigit( int c )
{
	if ( isdigit( c ) || ( toupper( c ) >= 'A' && toupper( c ) <= 'F' ) ) return 1;
	return 0;
}
