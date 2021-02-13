/* heapAlloc.c */

#include "heap_types.h"

void *heapAlloc( HeapUID uid, SceSize size )
{
	struct heap_allocated_memblock_header *cur  = (struct heap_allocated_memblock_header *)uid;
	struct heap_allocated_memblock_header *prev = NULL;
	struct heap_allocated_memblock_header *heap = NULL;
	SceSize alloc_size;
	
	/* 確保するサイズが0の場合は、1が指定されたものと見なす */
	if( ! size ) size = 1;
	
	alloc_size = size + sizeof( struct heap_allocated_memblock_header ) + HEAP_ALIGN_SIZE;
	
	/* 先頭要素だけはヒープ全体の情報として特別扱い */
	if( ! cur->next ){
		if( cur->size < alloc_size ) return NULL;
		cur->next = (struct heap_allocated_memblock_header *)MEMORY_ALIGN_ADDR( HEAP_ALIGN_SIZE, ( (uintptr_t)cur + sizeof( struct heap_allocated_memblock_header ) ) );
		cur->next->size = alloc_size;
		cur->next->next = NULL;
		return (void *)( (uintptr_t)cur->next + sizeof( struct heap_allocated_memblock_header ) );
	} else{
		/* メモリが一つでも割り当てられていれば、rootと最初の要素の間に空き容量がないか検査 */
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
	
	/* 十分な空き容量を探して取得 */
	for( ; cur; prev = cur, cur = cur->next ){
		/*
			直前のメモリブロックの終端(prev + prev->size)と現在のメモリブロック(cur)の差から、
			未使用メモリサイズを算出し、必要な容量が空いていれば使用、そうでなければ次へ。
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
		この時点で割り当てられていない場合、prevがもっとも後ろのメモリブロックを指しているので、
		ヒープメモリの終端とprevの終端の差から末尾の空きメモリをチェック。
	*/
	
	if( ! heap &&  ( ( (uintptr_t)uid + ((struct heap_allocated_memblock_header *)uid)->size ) - ( (uintptr_t)prev + prev->size ) >= alloc_size ) ){
		heap = (struct heap_allocated_memblock_header *)MEMORY_ALIGN_ADDR( HEAP_ALIGN_SIZE, ( (uintptr_t)prev + prev->size ) );
		heap->size = alloc_size;
		heap->next = prev->next;
		prev->next = heap;
	}
	
	return heap ? (void *)( (uintptr_t)heap + sizeof( struct heap_allocated_memblock_header ) ) : NULL;
}
