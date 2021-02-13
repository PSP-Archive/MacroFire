/* strutilToUpperFirst */

#include "../strutil.h"

char *strutilToUpperFirst( char *str )
{
	str[0] = toupper( str[0] );
	return str;
}
