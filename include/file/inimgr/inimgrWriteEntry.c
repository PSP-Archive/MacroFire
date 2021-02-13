/* inimgrWriteEntry.c */

#include "inimgr_types.h"

bool inimgrWriteEntry( InimgrContext context, const char *key, const char *value )
{
	struct inimgr_context *ctx = (struct inimgr_context *)context;
	if( ctx->mode != INIMGR_OP_SAVE || ! key || ! value || *key == '\0' ) return false;
	fiomgrWritefln( ctx->params->ini, ctx->params->buffer, sizeof( ctx->params->buffer ), "%s = %s", key, value );
	return true;
}
