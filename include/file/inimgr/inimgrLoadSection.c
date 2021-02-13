/* inimgrLoadSection.c */

#include "inimgr_types.h"

int inimgrLoadSection( InimgrUID uid, const char *name )
{
	struct inimgr_context  ctx = {
		(struct inimgr_params *)uid,
		__inimgr_find_section( (struct inimgr_params *)uid, name ),
		INIMGR_OP_LOAD
	};
	char *key, *value;
	
	if( ! ctx.section ){
		return CG_ERROR_INI_SECTION_NOT_FOUND;
	} else if( ! ctx.section->callback ){
		ctx.section->callback = __inimgr_find_callback( ctx.params, ctx.section->name );
	}
	
	if( ctx.section->callback ){
		*(ctx.params->buffer) = '\0';
		return ( ctx.section->callback->func )( (InimgrContext)&ctx, ctx.section->callback->arg );
	} else{
		int ret;
		
		fiomgrSeek( ctx.params->ini, ctx.section->offsetStart, FH_SEEK_SET );
		
		while( inimgrReadEntry( (InimgrContext)&ctx, &key, &value ) ){
			if( ( ret = __inimgr_set_value( ctx.params, ctx.section, key, value ) ) != CG_ERROR_OK ) return ret;
		}
	}
	
	return CG_ERROR_OK;
}
