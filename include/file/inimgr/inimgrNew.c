/* inimgrNew.c */

#include "inimgr_types.h"

InimgrUID inimgrNew( void )
{
	DmemUID dmem = dmemNew( 512, MEMORY_USER, DMEM_HIGH );
	if( ! dmem ) return CG_ERROR_NOT_ENOUGH_MEMORY;
	
	struct inimgr_params *params = dmemAlloc( dmem, sizeof( struct inimgr_params ) );
	if( ! params ){
		dmemDestroy( dmem );
		return CG_ERROR_NOT_ENOUGH_MEMORY;
	}
	
	params->ini           = 0;
	params->dmem          = dmem;
	params->callback      = NULL;
	params->section       = __inimgr_create_section( params, "" );
	if( ! params->section ){
		dmemDestroy( dmem );
		return CG_ERROR_NOT_ENOUGH_MEMORY;
	}
	params->section->name = INIMGR_DEFAULT_SECTION;
	params->last          = params->section;
	
	return (InimgrUID)params;
}
