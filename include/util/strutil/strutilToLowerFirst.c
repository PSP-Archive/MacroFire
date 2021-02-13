/* strutilToLowerFirst */

#include "../strutil.h"

char *strutilToLowerFirst( char *str )
{
	str[0] = tolower( str[0] );
	return str;
}
