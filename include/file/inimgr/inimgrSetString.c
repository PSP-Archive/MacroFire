/* inimgrSetString.c */

#include "inimgr_types.h"

int inimgrSetString( InimgrUID uid, const char *name, const char *key, const char *str )
{
	return __inimgr_set_value( (struct inimgr_params *)uid, __inimgr_find_section( (struct inimgr_params *)uid, name ), key, str );
}
