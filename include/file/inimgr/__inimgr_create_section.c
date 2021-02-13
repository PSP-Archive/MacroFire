/* __inimgr_create_section.c */

#include "inimgr_types.h"

struct inimgr_section *__inimgr_create_section( struct inimgr_params *params, const char *name )
{
	struct inimgr_section *section = dmemAlloc( params->dmem, sizeof( struct inimgr_section ) + strlen( name ) + 1 );
	if( ! section ) return NULL;
	
	section->name = (char *)( (uintptr_t)section + sizeof( struct inimgr_section ) );
	strcpy( section->name, name );
	section->offsetStart = 0;
	section->offsetEnd   = 0;
	section->entry       = NULL;
	section->callback    = NULL;
	section->next        = NULL;
	section->prev        = NULL;
	
	return section;
}
