/* memoryRealloc.c */

#include "memory_types.h"

void *memoryRealloc( const char *name, void *src_memblock, SceSize new_size )
{
	void *new_memblock;
	SceSize src_real_size, new_real_size;
	struct memory_header *header;
	int partition, type;
	
	if( ! src_memblock && ! new_size ){
		return NULL;
	} else if( ! src_memblock ){
		return memoryAlloc( new_size );
	} else if( ! new_size ){
		memoryFree( src_memblock );
		return NULL;
	}
	
	src_real_size = memoryUsableSize( src_memblock );
	new_real_size = MEMORY_REAL_SIZE( new_size );
	if( src_real_size == new_real_size ) return src_memblock;
	
	header    = (struct memory_header *)( (uintptr_t)src_memblock - sizeof( struct memory_header ) );
	partition = MEMORY_GET_PARTITION( header->part_type );
	type      = MEMORY_GET_BLOCKTYPE( header->part_type );
	
	if( type == PSP_SMEM_Addr ) return NULL;
	
	new_memblock = memoryExalloc( name, partition, header->align, new_real_size, type, NULL );
	if( ! new_memblock ) return NULL;
	
	memcpy( new_memblock, src_memblock, ( src_real_size > new_real_size ? new_real_size : src_real_size ) );
	memoryFree( src_memblock );
	
	return new_memblock;
}
