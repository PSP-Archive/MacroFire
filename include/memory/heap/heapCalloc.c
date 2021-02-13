/* heapCalloc.c */

#include "heap_types.h"

void *heapCalloc( HeapUID uid, SceSize size )
{
	void *heap = heapAlloc( uid, size );
	
	if( heap ) memset( heap, 0, size );
	
	return heap;
}
