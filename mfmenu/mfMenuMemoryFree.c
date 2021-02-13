/* mfMenuMemoryFree.c */

#include "mfmenu_types.h"

#ifdef DEBUG_ENABLED
extern int __mfmenu__g_memalloc_count;
#endif

int mfMenuMemoryFree( void *addr )
{
#ifdef DEBUG_ENABLED
	int ret = dmemFree( __mfmenu__gParams->MemoryUid, addr );
	if( ret == CG_OK ){
		__mfmenu__g_memalloc_count--;
		dbgprintf( "Free memory from menu memory: %p. It is now %d", addr, __mfmenu__g_memalloc_count );
	}
	return ret;
#else
	return dmemFree( __mfmenu__gParams->MemoryUid, addr );
#endif
}
