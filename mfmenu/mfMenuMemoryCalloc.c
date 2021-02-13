/* mfMenuMemoryCalloc.c */

#include "mfmenu_types.h"

void *mfMenuMemoryCalloc( size_t size )
{
	void *ptr = mfMenuMemoryAlloc( size );
	memset( ptr, 0, size );
	return ptr;
}
