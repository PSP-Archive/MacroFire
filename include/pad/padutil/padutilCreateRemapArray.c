/* padutilCreateRemapArray.c */

#include "padutil_types.h"

PadutilRemap *padutilCreateRemapArray( size_t count )
{
	int i;
	PadutilRemap *remap = (PadutilRemap *)memoryExalloc( "PadutilRemapArray", MEMORY_USER, 0, sizeof( PadutilRemap ) * count, PSP_SMEM_High, NULL );
	if( ! remap ) return NULL;
	
	remap[0].realButtons  = 0;
	remap[0].remapButtons = 0;
	remap[0].prev         = NULL;
	remap[0].next         = NULL;
	
	for( i = 1; i < count; i++ ){
		remap[i].realButtons  = 0;
		remap[i].remapButtons = 0;
		remap[i].prev         = &(remap[i - 1]);
		remap[i].next         = NULL;
		remap[i - 1].next     = &(remap[i]);
	}
	
	return remap;
}
