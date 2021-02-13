/* strutilRTrim */

#include "../strutil.h"

int strutilRTrim( char *__restrict__ str, const char *__restrict__ search )
{
	char *str_fin = strutilCounterReversePbrk( str, search );
	int  count = 0;
	
	if( str_fin ){
		str_fin++;
		count = strchr( str_fin, '\0' ) - str_fin;
		*str_fin = '\0';
	}
	
	return count;
}