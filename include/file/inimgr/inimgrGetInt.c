/* inimgrGetInt.c */

#include "inimgr_types.h"

int inimgrGetInt( InimgrUID uid, const char *name, const char *key, int *res )
{
	int ret;
	char *value;
	
	if( ( ret = __inimgr_get_value( (struct inimgr_params *)uid, name, key, &value ) ) == CG_ERROR_OK ){
		*res = strtol( value, NULL, 10 );
	}
	
	return ret;
}
