/* heapExCreate.c */

#include "heap_types.h"

HeapUID heapExCreate( const char *name, MemoryPartition partition, SceSize size, int type )
{
	return (HeapUID)sceKernelCreateHeap( partition, size + __HEAP_MGMTAREA_SIZE + 1024, type, name );
}

void heapDestroy( HeapUID uid )
{
	sceKernelDeleteHeap( (SceUID)uid );
}
