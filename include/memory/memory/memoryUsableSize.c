/* memoryUsableSize.c */

#include "memory_types.h"

SceSize memoryUsableSize( const void *memblock )
{
	struct memory_header *header;
	
	if( ! memblock ) return 0;
	header = (struct memory_header *)( (uintptr_t)memblock - sizeof( struct memory_header ) );
	
	return header->size;
}
