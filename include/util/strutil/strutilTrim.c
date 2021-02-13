/* strutilTrim */

#include "../strutil.h"

int strutilTrim( char *__restrict__ str, const char *__restrict__ search )
{
	char *new_str = str;
	
	while( *str != '\0' ){
		if( ! strchr( search, *str )  ) *new_str++ = *str;
		str++;
	}
	
	*new_str = *str;
	
	return str - new_str;
}
