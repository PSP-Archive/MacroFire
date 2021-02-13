/* padutilGetButtonNamesByCode.c */

#include "padutil_types.h"

char *padutilGetButtonNamesByCode( PadutilButtonName *availbtn, PadutilButtons buttons, const char *delim, unsigned int opt, char *buf, size_t len )
{
	int i;
	char delimtoken[16];
	
	if( ! buf || ! len || ! availbtn ) return NULL;
	if( ! buttons  ){
		*buf = '\0';
		return buf;
	}
	
	if( delim ){
		strutilCopy( delimtoken, delim, sizeof( delimtoken ) );
		
		if( opt & PADUTIL_OPT_IGNORE_SP ){
			strutilTrim( delimtoken, "\x20\t" );
		}
		if( opt & PADUTIL_OPT_TOKEN_SP ){
			int len = strlen( delimtoken );
			memmove( delimtoken + 1, delimtoken, len + 1 );
			delimtoken[0]       = '\x20';
			delimtoken[len + 1] = '\x20';
			delimtoken[len + 2] = '\0';
		}
	}
	
	*buf = '\0';
	
	for( i = 0; availbtn[i].button || availbtn[i].name; i++ ){
		if( buttons & availbtn[i].button ){
			if( *buf != '\0' && *delimtoken ){
				strutilCat( buf, delimtoken, len );
			}
			if( availbtn[i].name ) strutilCat( buf, availbtn[i].name, len );
		}
	}
	
	return buf;
}
