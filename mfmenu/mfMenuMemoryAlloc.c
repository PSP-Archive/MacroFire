/* mfMenuMemoryAlloc.c */

#include "mfmenu_types.h"

#ifdef DEBUG_ENABLED
extern int __mfmenu__g_memalloc_count;
#endif

void *mfMenuMemoryAlloc( size_t size )
{
#ifdef DEBUG_ENABLED
	void *ptr =dmemAlloc( __mfmenu__gParams->MemoryUid, size );
	if( ptr ){
		__mfmenu__g_memalloc_count++;
		dbgprintf( "Memory allocated from menu memory: %p. It is now %d", ptr, __mfmenu__g_memalloc_count );
	}
	return ptr;
#else
	return dmemAlloc( __mfmenu__gParams->MemoryUid, size );
#endif
}
