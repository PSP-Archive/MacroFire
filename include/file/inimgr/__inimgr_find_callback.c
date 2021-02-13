/* __inimgr_find_callback.c */

#include "inimgr_types.h"

struct inimgr_callback *__inimgr_find_callback( struct inimgr_params *params, const char *name )
{
	struct inimgr_callback *callback;
	for( callback = params->callback; callback; callback = callback->next ){
		if( strutilWildcard( callback->wildcard, name ) ) return callback;
	}
	return NULL;
}
