/* memoryCalloc.c */

#include "memory_types.h"

void *memoryCalloc( SceSize nelem, SceSize size )
{
	void *memblock;
	
	/* �ǂ��炩��0�̏ꍇ�A1�o�C�g�m�� */
	if( nelem == 0 || size == 0 ){
		nelem = 1;
		size  = 1;
	}
	
	/*
		�T�C�Y�`�F�b�N
		
		nelem * size��SceSize�^�̍ő�l�𒴂���(�I�[�o�[�t���[����)�ꍇ�A
		���̎��͋U�ƂȂ�NULL��Ԃ��B
	*/
	if( ( nelem * size ) / size != nelem ) return NULL;
	
	memblock = memoryAlloc( nelem * size );
	if( memblock ) memset( memblock, 0, nelem * size );
	
	return memblock;
}
