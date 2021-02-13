/* __inimgr_set_value.c */

#include "inimgr_types.h"

int __inimgr_set_value( struct inimgr_params *params, struct inimgr_section *section, const char *key, const char *value )
{
	struct inimgr_entry *entry;
	unsigned int keylen = strlen( key ) + 1;
	
	if( ! section ){
		return CG_ERROR_INI_SECTION_NOT_FOUND;
	} else if( ! key || ! value ){
		return CG_ERROR_INVALID_ARGUMENT;
	}
	
	if( ! ( entry = __inimgr_find_entry( section, key ) ) ){
		entry = dmemAlloc( params->dmem, sizeof( struct inimgr_entry ) + INIMGR_ENTRY_LENGTH );
		if( ! entry ) return CG_ERROR_NOT_ENOUGH_MEMORY;
		
		entry->key   = (char *)( (uintptr_t)entry + sizeof( struct inimgr_entry ) );
		entry->value = (char *)( (uintptr_t)entry->key + keylen );
		
		entry->flags = 0;
		strcpy( entry->key, key );
		
		if( ! section->entry ){
			entry->prev = NULL;
			entry->next = NULL;
			section->entry = entry;
		} else{
			for( entry->prev = section->entry; entry->prev->next; entry->prev = entry->prev->next );
			entry->prev->next = entry;
			entry->next       = NULL;
		}
	}
	
	strutilCopy( entry->value, value, INIMGR_ENTRY_LENGTH - keylen );
	
	return CG_ERROR_OK;
}
