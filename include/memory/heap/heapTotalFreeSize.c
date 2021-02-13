/* heapTotalFreeSize.c */

#include "heap_types.h"

SceSize heapTotalFreeSize( HeapUID uid )
{
	SceSize totalsize = 0;
	struct heap_allocated_memblock_header *cur = (struct heap_allocated_memblock_header *)uid;
	
	if( ! cur->next ){
		return cur->size;
	} else{
		totalsize = cur->size;
		cur = cur->next;
	}
	
	for( ; cur; cur = cur->next ){
		totalsize -= cur->size;
	}
	
	return totalsize;
}
