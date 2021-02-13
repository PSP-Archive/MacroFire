/* strupr.c */

#include <ctype.h>

char *strupr( char *s )
{
	while( *s != '\0' ){
		*s = toupper( *s );
		s++;
	}
	return s;
}
