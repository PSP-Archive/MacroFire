/* memoryReallocf.c */

#include "memory_types.h"

void *memoryReallocf( const char *name, void *src_memblock, SceSize new_size )
{
	void *new_memblock = memoryRealloc( name, src_memblock, new_size );
	
	if( src_memblock && new_size && ! new_memblock ) memoryFree( src_memblock );
	return new_memblock;
}
