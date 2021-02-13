/* heapExCreate.c */

#include "heap_types.h"

HeapUID heapExCreate( const char *name, MemoryPartition partition, SceSize size, int type )
{
	struct heap_allocated_memblock_header *heap_root = (struct heap_allocated_memblock_header *)memoryExalloc( name, partition, 0, size + sizeof( struct heap_allocated_memblock_header ), type == HEAP_HIGH ? PSP_SMEM_High : PSP_SMEM_Low , NULL );
	if( heap_root ){
		heap_root->next = NULL;
		heap_root->size = memoryUsableSize( (void *)heap_root ) - sizeof( struct heap_allocated_memblock_header );
	}
	
	return (HeapUID)heap_root;
}

void heapDestroy( HeapUID uid )
{
	memoryFree( (void *)uid );
}
