/* dmem_types.h                   */
/* �����Ŏg�p�B�O����Q�Ƃ��Ȃ��B */

#include "../dmem.h"
#include <string.h>

#define DMEM_DEFAULT_MINBLOCK 1024
#define DMEM_HEADER_SIZE ( HEAP_HEADER_SIZE + sizeof( struct dmem_heap ) )

struct dmem_heap {
	HeapUID uid;
	size_t count;
	struct dmem_heap *next;
};

struct dmem_root {
	size_t           minHeapSize;
	MemoryPartition  memPart;
	int              allocType;
	struct dmem_heap *heapList;
	struct dmem_heap *lastUse;
};
