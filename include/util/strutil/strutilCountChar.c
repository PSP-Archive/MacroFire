/* strutilCountChar */

#include "../strutil.h"

unsigned int strutilCountChar( const char *str, char c )
{
	unsigned int cnt = 0;
	const char *save = str;
	
	while( ( save = strchr( save, c ) ) ) cnt++;
	
	return cnt;
}
