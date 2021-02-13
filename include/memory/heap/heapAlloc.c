/* heapAlloc.c */

#include "heap_types.h"

void *heapAlloc( HeapUID uid, SceSize size )
{
	struct heap_allocated_memblock_header *cur  = (struct heap_allocated_memblock_header *)uid;
	struct heap_allocated_memblock_header *prev = NULL;
	struct heap_allocated_memblock_header *heap = NULL;
	SceSize alloc_size;
	
	/* �m�ۂ���T�C�Y��0�̏ꍇ�́A1���w�肳�ꂽ���̂ƌ��Ȃ� */
	if( ! size ) size = 1;
	
	alloc_size = size + sizeof( struct heap_allocated_memblock_header ) + HEAP_ALIGN_SIZE;
	
	/* �擪�v�f�����̓q�[�v�S�̂̏��Ƃ��ē��ʈ��� */
	if( ! cur->next ){
		if( cur->size < alloc_size ) return NULL;
		cur->next = (struct heap_allocated_memblock_header *)MEMORY_ALIGN_ADDR( HEAP_ALIGN_SIZE, ( (uintptr_t)cur + sizeof( struct heap_allocated_memblock_header ) ) );
		cur->next->size = alloc_size;
		cur->next->next = NULL;
		return (void *)( (uintptr_t)cur->next + sizeof( struct heap_allocated_memblock_header ) );
	} else{
		/* ����������ł����蓖�Ă��Ă���΁Aroot�ƍŏ��̗v�f�̊Ԃɋ󂫗e�ʂ��Ȃ������� */
		struct heap_allocated_memblock_header *topheap = (struct heap_allocated_memblock_header *)( (uintptr_t)cur + sizeof( struct heap_allocated_memblock_header ) );
		
		if( (uintptr_t)(cur->next) - (uintptr_t)topheap >= alloc_size ){
			topheap->size = alloc_size;
			topheap->next = cur->next;
			cur->next = (struct heap_allocated_memblock_header *)MEMORY_ALIGN_ADDR( HEAP_ALIGN_SIZE, topheap );
			return (void *)( (uintptr_t)topheap + sizeof( struct heap_allocated_memblock_header ) );
		}
	}
	
	prev = cur->next;
	cur  = prev->next;
	
	/* �\���ȋ󂫗e�ʂ�T���Ď擾 */
	for( ; cur; prev = cur, cur = cur->next ){
		/*
			���O�̃������u���b�N�̏I�[(prev + prev->size)�ƌ��݂̃������u���b�N(cur)�̍�����A
			���g�p�������T�C�Y���Z�o���A�K�v�ȗe�ʂ��󂢂Ă���Ύg�p�A�����łȂ���Ύ��ցB
		*/
		if( (uintptr_t)cur - ( (uintptr_t)prev + prev->size ) >= alloc_size ){
			heap = (struct heap_allocated_memblock_header *)MEMORY_ALIGN_ADDR( HEAP_ALIGN_SIZE, ( (uintptr_t)prev + prev->size ) );
			heap->size = alloc_size;
			heap->next = prev->next;
			prev->next = heap;
			break;
		}
	}
	
	/*
		���̎��_�Ŋ��蓖�Ă��Ă��Ȃ��ꍇ�Aprev�������Ƃ����̃������u���b�N���w���Ă���̂ŁA
		�q�[�v�������̏I�[��prev�̏I�[�̍����疖���̋󂫃��������`�F�b�N�B
	*/
	
	if( ! heap &&  ( ( (uintptr_t)uid + ((struct heap_allocated_memblock_header *)uid)->size ) - ( (uintptr_t)prev + prev->size ) >= alloc_size ) ){
		heap = (struct heap_allocated_memblock_header *)MEMORY_ALIGN_ADDR( HEAP_ALIGN_SIZE, ( (uintptr_t)prev + prev->size ) );
		heap->size = alloc_size;
		heap->next = prev->next;
		prev->next = heap;
	}
	
	return heap ? (void *)( (uintptr_t)heap + sizeof( struct heap_allocated_memblock_header ) ) : NULL;
}
