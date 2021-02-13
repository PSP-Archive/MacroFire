/* inimgrGetString.c */

#include "inimgr_types.h"

int inimgrGetString( InimgrUID uid, const char *name, const char *key, char *res, size_t reslen )
{
	int ret;
	char *value;
	
	if( ( ret = __inimgr_get_value( (struct inimgr_params *)uid, name, key, &value ) ) == CG_ERROR_OK ){
		ret = strutilCopy( res, value, reslen ) - 1;
	}
	
	return ret;
}
