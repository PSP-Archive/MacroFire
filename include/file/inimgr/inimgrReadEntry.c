/* inimgrReadEntry.c */

#include "inimgr_types.h"

bool inimgrReadEntry( InimgrContext context, char **key, char **value )
{
	struct inimgr_context *ctx = (struct inimgr_context *)context;
	char *entry, *delim;
	
	if( ctx->mode != INIMGR_OP_LOAD ) return false;
	
	do {
		if( fiomgrTell( ctx->params->ini ) >= ctx->section->offsetEnd ) return false;
		entry = __inimgr_read_line( ctx->params->ini, ctx->params->buffer, sizeof( ctx->params->buffer ) );
		if( ! entry ) return false;
		
		/*
			行内に最初に出現するセミコロンをコメントと見なして終端文字に置き換える
			キーや値にセミコロンが入っていても、それ以降をコメントにされてしまう!
		*/
		delim = strchr( entry, ';' );
		if( delim ) *delim = '\0';
	/* '=' がなかったら次へ */
	} while( *entry == '\0' || ! ( delim = strchr( entry, '=' ) ));
	
	/* この時点ではdelimには'='へのポインタがセットされているので\0をセットして分割 */
	*delim = '\0';
	
	*key   = entry;
	*value = delim + 1;
	
	delim = strutilCounterReversePbrk( *key, "\x20\t" );
	if( delim ) *(delim + 1) = '\0';
	
	delim = strutilCounterPbrk( *value, "\x20\t" );
	if( delim ) *value = delim;
	
	if( **value != '\0' ) strutilRTrim( *value, "\x20\t" );
	
	return true;
}
