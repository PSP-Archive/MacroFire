/* strncasecmp.c */

#include <stdio.h>
#include <string.h>

int strncasecmp( const char *s1, const char *s2, size_t n )
{
	register char ss1, ss2;
	
	if( ! n ) return 0;
	
	n--;
	while( ( ss1 = toupper( *s1 ) ) == ( ss2 = toupper( *s2 ) ) ){
		if( *s1 == '\0' || ! n ){
			return 0;
		} else{
			n--;
			s1++;
			s2++;
		}
	}
	
	return ss1 - ss2;
}
