/* heapAlloc.c */

#include "heap_types.h"

void *heapAlloc( HeapUID uid, SceSize size )
{
	return sceKernelAllocHeapMemory( (SceUID)uid, size );
}
