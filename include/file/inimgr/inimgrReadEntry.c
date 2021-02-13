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
			�s���ɍŏ��ɏo������Z�~�R�������R�����g�ƌ��Ȃ��ďI�[�����ɒu��������
			�L�[��l�ɃZ�~�R�����������Ă��Ă��A����ȍ~���R�����g�ɂ���Ă��܂�!
		*/
		delim = strchr( entry, ';' );
		if( delim ) *delim = '\0';
	/* '=' ���Ȃ������玟�� */
	} while( *entry == '\0' || ! ( delim = strchr( entry, '=' ) ));
	
	/* ���̎��_�ł�delim�ɂ�'='�ւ̃|�C���^���Z�b�g����Ă���̂�\0���Z�b�g���ĕ��� */
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
