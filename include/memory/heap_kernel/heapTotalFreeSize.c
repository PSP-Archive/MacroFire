/* heapTotalFreeSize.c */

#include "heap_types.h"

SceSize heapTotalFreeSize( HeapUID uid )
{
	return sceKernelHeapTotalFreeSize( uid );
}
