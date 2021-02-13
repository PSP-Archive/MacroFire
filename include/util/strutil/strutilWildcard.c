/* strutilWildcard.c */

#include "../strutil.h"

static int strutil_wildcard_test( const char *__restrict__  *__restrict__ wc, const char *__restrict__  *__restrict__ test );

int strutilWildcard( const char *__restrict__ wc, const char *__restrict__ name )
{
	if( ! wc || ! name ) return 0;
	
	while( *wc != '\0' ){
		if( *wc == '*' ){
			wc++;
			if( ! strutil_wildcard_test( &wc, &name ) ){
				if( *name == '\0' ){
					return 0;
				} else{
					wc--;
					name++;
				}
			}
		} else{
			if( ! strutil_wildcard_test( &wc, &name ) ) return 0;
		}
	}
	return 1;
}

static int strutil_wildcard_test( const char *__restrict__  *__restrict__ wc, const char *__restrict__  *__restrict__ test )
{
	const char *reset_wc = *wc;
	
	while( **wc != '*' && **wc != '\0' ){
		if( ( **wc == '?' && **test != '\0' ) || ( **wc == **test ) ){
			(*wc)++;
			(*test)++;
		} else{
			*wc = reset_wc;
			return 0;
		}
	}
	return 1;
}
