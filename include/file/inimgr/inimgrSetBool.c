/* inimgrSetBool.c */

#include "inimgr_types.h"

int inimgrSetBool( InimgrUID uid, const char *name, const char *key, bool bl )
{
	char value[4];
	
	if( bl ){
		strcpy( value, "On" );
	} else{
		strcpy( value, "Off" );
	}
	
	return __inimgr_set_value( (struct inimgr_params *)uid, __inimgr_find_section( (struct inimgr_params *)uid, name ), key, value );
}
