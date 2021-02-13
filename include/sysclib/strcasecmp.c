/* strcasecmp.c */

#include <stdio.h>
#include <string.h>

int strcasecmp( const char *s1, const char *s2 )
{
	register char ss1, ss2;
	
	while( ( ss1 = toupper( *s1 ) ) == ( ss2 = toupper( *s2 ) ) ){
		if( *s1 == '\0' ){
			return 0;
		} else{
			s1++;
			s2++;
		}
	}
	
	return ss1 - ss2;
}
