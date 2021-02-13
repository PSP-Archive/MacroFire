/* dmemCalloc.c */

#include "dmem_types.h"

void *dmemCalloc( DmemUID uid, size_t nelem, size_t size )
{
	void *ptr;
	
	/* オーバーフローチェック */
	if( ( nelem * size ) / size != nelem ) return NULL;
	
	ptr = dmemAlloc( uid, nelem * size );
	if( ptr ) memset( ptr, 0, nelem * size );
	return ptr;
}
