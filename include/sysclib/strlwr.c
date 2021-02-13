/* strlwr.c */

#include <ctype.h>

char *strlwr( char *s )
{
	while( *s != '\0' ){
		*s = tolower( *s );
		s++;
	}
	return s;
}
