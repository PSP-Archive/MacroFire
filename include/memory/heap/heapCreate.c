/* heapCreate.c */

#include "heap_types.h"

HeapUID heapCreate( SceSize size )
{
	return heapExCreate( "UserHeap", MEMORY_USER, size + sizeof( struct heap_allocated_memblock_header ), HEAP_LOW );
}
