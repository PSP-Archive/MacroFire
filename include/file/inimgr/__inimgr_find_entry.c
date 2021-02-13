/* __inimgr_find_entry.c */

#include "inimgr_types.h"

struct inimgr_entry *__inimgr_find_entry( struct inimgr_section *section, const char *key )
{
	struct inimgr_entry *entry;
	for( entry = section->entry; entry; entry = entry->next ){
		if( strcasecmp( entry->key, key ) == 0 ) return entry;
	}
	return NULL;
}
