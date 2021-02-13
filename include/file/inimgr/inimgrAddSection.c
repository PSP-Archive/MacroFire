/* inimgrAddSection.c */

#include "inimgr_types.h"

int inimgrAddSection( InimgrUID uid, const char *name )
{
	struct inimgr_params  *params  = (struct inimgr_params *)uid;
	struct inimgr_section *section = __inimgr_find_section( params, name );
	
	if( ! section ){
		section = __inimgr_create_section( params, name );
		if( ! section ) return CG_ERROR_NOT_ENOUGH_MEMORY;
		
		for( section->prev = params->last; section->prev->next; section->prev = section->prev->next );
		section->prev->next = section;
	}
	return CG_ERROR_OK;
}
