/* padutilGetButtonCodeByNames.c */

#include "padutil_types.h"

PadutilButtons padutilGetButtonCodeByNames( PadutilButtonName *availbtn, char *names, const char *delim, unsigned int opt )
{
	int i, nameslen = 0, delimlen = 0;
	PadutilButtons buttons = 0;
	char *btn_name, *next = NULL;
	
	void *comparer;
	char delimtoken[16];
	
	if( ! names || names[0] == '\0' || ! availbtn ) return 0;
	if( delim ) delimlen = strlen( delim );
	
	nameslen = strlen( names );
	
	strutilCopy( delimtoken, delim, sizeof( delimtoken ) );
	
	if( opt & PADUTIL_OPT_IGNORE_SP ){
		strutilTrim( names,      "\x20\t" );
		strutilTrim( delimtoken, "\x20\t" );
	}
	if( opt & PADUTIL_OPT_CASE_SENS ){
		comparer = strcmp;
	} else{
		comparer = strcasecmp;
	}
	
	btn_name = names;
	
	do{
		if( delimlen ){
			next = strstr( btn_name, delimtoken );
			if( next ){
				*next = '\0';
				next += delimlen;
			}
		}
		
		for( i = 0; availbtn[i].button || availbtn[i].name; i++ ){
			if( availbtn[i].name && ( ((int (*)(const char*, const char*))comparer)( btn_name, availbtn[i].name ) == 0 ) ){
				buttons |= availbtn[i].button;
				break;
			}
		}
		btn_name = next;
	} while( btn_name );
	
	return buttons;
}
