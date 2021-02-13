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
	
	/* 先頭要素だけはヒープ全体の情報として特別扱い */
	if( ! root->next ){
		/* 先頭の次がない場合は、メモリは何も割り当てられていない */
		return CG_ERROR_INVALID_ARGUMENT;
	} else{
		cur = root->next;
	}
	
	for( ; cur != heap; prev = cur, cur = cur->next ){
		/* curがNULLになるまで回った場合、そのアドレスは使用されていない */
		if( ! cur ) return CG_ERROR_INVALID_ARGUMENT;
	}
	
	/*
		対象メモリブロックの前のメモリブロックが、対象メモリブロックの次を指すことで、
		対象メモリブロックは未使用として扱われる。
	*/
	if( prev ){
		prev->next = cur->next;
	} else{
		/* prevが無い場合は、ヒープの先頭なので、rootのリンク先を変更 */
		root->next = cur->next;
	}
	
	return CG_ERROR_OK;
}
