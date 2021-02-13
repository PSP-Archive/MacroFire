/* inimgrSave.c */

#include "inimgr_types.h"

int inimgrSave( InimgrUID uid, const char *path, const char *sig, InimgrVersion version )
{
	struct inimgr_entry   *entry;
	struct inimgr_context ctx = {
		(struct inimgr_params *)uid,
		NULL,
		INIMGR_OP_SAVE,
	};
	int ret = CG_ERROR_OK;
	
	//if( CG_IS_VALID_UID( ctx.params->ini ) ){
	//	ctx.params->ini = fiomgrOpen( path, FH_O_RDWR | FH_O_CREAT | FH_O_ALLOC_HIGH, 0777 );
	//	if( ! CG_IS_VALID_UID( ctx.params->ini ) ) return ctx.params->ini;
	//} else{
	//	fiomgrSeek( ctx.params->ini, 0, FH_SEEK_SET );
	//}
	
	/*
		�t�@�C�����쐬�A�폜���邽�߂ɕK����x�J�������B
		ftruncate()���ق����c�c�B
	*/
	if( CG_IS_VALID_UID( ctx.params->ini ) ) fiomgrClose( ctx.params->ini );
	ctx.params->ini = fiomgrOpen( path, FH_O_RDWR | FH_O_CREAT | FH_O_TRUNC | FH_O_ALLOC_HIGH, 0777 );
	if( ! CG_IS_VALID_UID( ctx.params->ini ) ) return ctx.params->ini;
	
	if( sig && *sig != '\0' ){
		fiomgrWrite( ctx.params->ini, (void *)sig, strlen( sig ) );
		if( version ){
			char ver_text[13];
			unsigned char len;
			len = snprintf( ver_text, sizeof( ver_text ), " %d.%d.%d", (unsigned char)( version >> 16 ), (unsigned char)( version >> 8 ), (unsigned char)version );
			fiomgrWriteln( ctx.params->ini, ver_text, len );
		}
	}
	
	for( ctx.section = ctx.params->section; ctx.section; ctx.section = ctx.section->next ){
		if( ! ctx.section->callback ) ctx.section->callback = __inimgr_find_callback( ctx.params, ctx.section->name );
		
		/* �Z�N�V�����̕�����INIMGR_DEFAULT_SECTION���w���Ă��Ȃ���΃Z�N�V���������L�^ */
		if( (void *)ctx.section->name != (void *)INIMGR_DEFAULT_SECTION ){
			if( fiomgrTell( ctx.params->ini ) != 0 ) fiomgrWriteln( ctx.params->ini, "", 0 );
			fiomgrWritefln( ctx.params->ini, ctx.params->buffer, sizeof( ctx.params->buffer ), "[%s]", ctx.section->name );
		}
		
		if( ctx.section->callback ){
			ret = ( ctx.section->callback->func )( (InimgrContext)&ctx, ctx.section->callback->arg );
			if( ret != CG_ERROR_OK ) break;
		} else{
			for( entry = ctx.section->entry; entry; entry = entry->next ){
				inimgrWriteEntry( (InimgrContext)&ctx, entry->key, entry->value );
			}
		}
		
	}
	
	// fiomgrTruncate( ctx.params->ini, fiomgrTell( ctx.params->ini ) );
	
	return ret;
}
