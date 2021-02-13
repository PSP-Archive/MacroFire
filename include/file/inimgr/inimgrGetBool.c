/* inimgrGetBool.c */

#include "inimgr_types.h"

int inimgrGetBool( InimgrUID uid, const char *name, const char *key, bool *res )
{
	int ret;
	char *value;
	
	if( ( ret = __inimgr_get_value( (struct inimgr_params *)uid, name, key, &value ) ) == CG_ERROR_OK ){
		if( strcasecmp( value, "ON" ) == 0 ){
			*res = true;
		} else if( strcasecmp( value, "OFF" ) == 0 ){
			*res = false;
		} else{
			ret = CG_ERROR_INI_INVALID_VALUE;
		}
	}
	
	return ret;
}
