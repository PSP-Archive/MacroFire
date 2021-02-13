/* __inimgr_get_value.c */

#include "inimgr_types.h"

int __inimgr_get_value( struct inimgr_params *params, const char *name, const char *key, char **result )
{
	struct inimgr_section *section;
	struct inimgr_entry   *entry;
	
	if( ! ( section = __inimgr_find_section( params, name ) ) ){
		return CG_ERROR_INI_SECTION_NOT_FOUND;
	} else if( ! ( entry   = __inimgr_find_entry( section, key ) ) ){
		return CG_ERROR_INI_KEY_NOT_FOUND;
	}
	
	*result = entry->value;
	
	return CG_ERROR_OK;
}
