/* heapFree.c */

#include "heap_types.h"

int heapFree( HeapUID uid, void *ptr )
{
	return sceKernelFreeHeapMemory( uid, ptr );
}
