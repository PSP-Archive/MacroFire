/* __inimgr_find_section.c */

#include "inimgr_types.h"

struct inimgr_section *__inimgr_find_section( struct inimgr_params *params, const char *name )
{
	struct inimgr_section *section;
	
	if( ! name || *name == '\0' ) name = INIMGR_DEFAULT_SECTION;
	
	if( params->last && ( strcasecmp( params->last->name, name ) == 0 ) ) return params->last;
	
	for( section = params->section; section; section = section->next ){
		if( strcasecmp( section->name, name ) == 0 ){
			params->last = section;
			return section;
		}
	}
	
	return NULL;
}
