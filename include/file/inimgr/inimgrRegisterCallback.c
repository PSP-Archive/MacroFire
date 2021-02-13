/* inimgrRegisterCallback.c */

#include "inimgr_types.h"

int inimgrRegisterCallback( InimgrUID uid, const char *wildcard, InimgrCallback cb, void *arg )
{
	struct inimgr_params   *params = (struct inimgr_params *)uid;
	struct inimgr_callback *new_entry;
	
	if( ! wildcard ) wildcard = INIMGR_DEFAULT_SECTION;
	if( wildcard[0] == '\0' || ! cb ) return CG_ERROR_INVALID_ARGUMENT;
	
	new_entry = dmemAlloc( params->dmem, sizeof( struct inimgr_callback ) + strlen( wildcard ) + 1 );
	if( ! new_entry ) return CG_ERROR_NOT_ENOUGH_MEMORY;
	
	new_entry->wildcard = (char *)( (uintptr_t)new_entry + sizeof( struct inimgr_callback ) );
	strcpy( new_entry->wildcard, wildcard );
	new_entry->func = cb;
	new_entry->arg  = arg;
	new_entry->next = NULL;
	
	if( ! params->callback ){
		params->callback = new_entry;
	} else{
		struct inimgr_callback *entry;
		for( entry = params->callback; entry->next; entry = entry->next );
		entry->next = new_entry;
	}
	
	return CG_ERROR_OK;
}
