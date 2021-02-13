/* memoryExalloc.c */

#include "memory_types.h"

void *memoryExalloc( const char *name, MemoryPartition partition, unsigned int align, SceSize size, int type, void *addr )
{
	struct memory_header header;
	void *memblock;
	SceSize real_size;
	
	/* 0バイト確保しようとした場合は、最小値で確保 */
	if( ! size ) size = 1;
	
	real_size = MEMORY_REAL_SIZE( size ); /* MEMORY_PAGE_SIZE区切りの実際に確保された値に補正 */
	
	header.blockId = sceKernelAllocPartitionMemory( partition, name, type, sizeof( struct memory_header ) + real_size + align, addr );
	if( header.blockId < 0 ) return NULL;
	
	memblock = (void *)( (uintptr_t)(sceKernelGetBlockHeadAddr( header.blockId )) + sizeof( struct memory_header ) );
	if( align ){
		if( ! MEMORY_POWER_OF_TWO( align ) ){
			sceKernelFreePartitionMemory( header.blockId );
			return NULL;
		}
		memblock = (void *)MEMORY_ALIGN_ADDR( align, memblock );
	}
	
	header.size      = real_size;
	header.part_type = MEMORY_SET_PARTITION_BLOCKTYPE( partition, type );
	header.align     = align;
	
	memcpy( (void *)( (uintptr_t)memblock - sizeof( struct memory_header ) ), (void *)&header, sizeof( struct memory_header ) );
	
	return memblock;
}

int memoryFree( void *memblock )
{
	struct memory_header *header;

	if( ! memblock ) return 0;
	header = (struct memory_header *)( (uintptr_t)memblock - sizeof( struct memory_header ) );

	return sceKernelFreePartitionMemory( header->blockId );
}
