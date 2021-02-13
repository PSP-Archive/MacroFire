/* heapCreate.c */

#include "heap_types.h"

HeapUID heapCreate( SceSize size )
{
	return heapExCreate( "KernelHeap", MEMORY_USER, size + __HEAP_MGMTAREA_SIZE, HEAP_LOW );
}
