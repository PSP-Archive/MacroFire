/* strutilLTrim */

#include "../strutil.h"

int strutilLTrim( char *__restrict__ str, const char *__restrict__ search )
{
	char *new_str = strutilCounterPbrk( str, search );
	int  count = 0;
	
	if( new_str ){
		count = new_str - str;
		while( *new_str != '\0' ) *str++ = *new_str++;
		*str = '\0';
	}
	
	return count;
}