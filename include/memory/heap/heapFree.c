/* heapFree.c */

#include "heap_types.h"

int heapFree( HeapUID uid, void *ptr )
{
	struct heap_allocated_memblock_header *root;
	struct heap_allocated_memblock_header *cur;
	struct heap_allocated_memblock_header *prev;
	struct heap_allocated_memblock_header *heap;
	
	if( ! ptr ) return CG_ERROR_OK;
	
	root = (struct heap_allocated_memblock_header *)uid;
	cur  = NULL;
	prev = NULL;
	heap = (struct heap_allocated_memblock_header *)( (uintptr_t)ptr - sizeof( struct heap_allocated_memblock_header ) );
	
	/* �擪�v�f�����̓q�[�v�S�̂̏��Ƃ��ē��ʈ��� */
	if( ! root->next ){
		/* �擪�̎����Ȃ��ꍇ�́A�������͉������蓖�Ă��Ă��Ȃ� */
		return CG_ERROR_INVALID_ARGUMENT;
	} else{
		cur = root->next;
	}
	
	for( ; cur != heap; prev = cur, cur = cur->next ){
		/* cur��NULL�ɂȂ�܂ŉ�����ꍇ�A���̃A�h���X�͎g�p����Ă��Ȃ� */
		if( ! cur ) return CG_ERROR_INVALID_ARGUMENT;
	}
	
	/*
		�Ώۃ������u���b�N�̑O�̃������u���b�N���A�Ώۃ������u���b�N�̎����w�����ƂŁA
		�Ώۃ������u���b�N�͖��g�p�Ƃ��Ĉ�����B
	*/
	if( prev ){
		prev->next = cur->next;
	} else{
		/* prev�������ꍇ�́A�q�[�v�̐擪�Ȃ̂ŁAroot�̃����N���ύX */
		root->next = cur->next;
	}
	
	return CG_ERROR_OK;
}
