/* dirhNew.c */

#include "dirh_types.h"

DirhUID dirhNew( size_t pathmax, unsigned int options )
{
	struct dirh_params *params = (struct dirh_params *)memoryExalloc( "DirhParams", MEMORY_USER, 0, sizeof( struct dirh_params ) + pathmax, options & DIRH_O_ALLOC_HIGH ? PSP_SMEM_High : PSP_SMEM_Low, NULL );
	if( ! params ) return 0;
	
	/* ‰Šú‰» */
	params->semaId = sceKernelCreateSema( "DirhSema", 0, 1, 1, 0 );
	if( CG_IS_ERROR( params->semaId ) ){
		DirhUID ret = params->semaId;
		memoryFree( params );
		return ret;
	}
	params->data.cwd.path    = (char *)( (uintptr_t)params + sizeof( struct dirh_params ) );
	params->data.cwd.max     = pathmax;
	params->data.entry.list  = NULL;
	params->data.entry.count = 0;
	params->data.entry.pos   = 0;
	params->options          = options;
	
	params->data. cwd.path[0] = '\0';
	
	return (DirhUID)params;
}
