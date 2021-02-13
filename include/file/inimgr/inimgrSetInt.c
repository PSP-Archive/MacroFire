/* inimgrSetInt.c */

#include "inimgr_types.h"

int inimgrSetInt( InimgrUID uid, const char *name, const char *key, int32_t integer )
{
	char value[11];
	
	/* 32�r�b�g�̐��l��10�����z���Ȃ��̂Ńo�b�t�@���̓`�F�b�N���Ȃ� */
	sprintf( value, "%d", integer );
	
	return __inimgr_set_value( (struct inimgr_params *)uid, __inimgr_find_section( (struct inimgr_params *)uid, name ), key, value );
}
