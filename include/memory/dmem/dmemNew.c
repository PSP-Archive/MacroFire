/* dmemNew.c */

#include "dmem_types.h"

DmemUID dmemNew( size_t minblock, MemoryPartition partid, int type )
{
	struct dmem_root *root;
	
	root = memoryExalloc( "DmemParams", partid, 0, sizeof( struct dmem_root ), type == DMEM_HIGH ? PSP_SMEM_High : PSP_SMEM_Low, NULL );
	if( ! root ) return 0;
	
	root->minHeapSize = minblock > 0 ? minblock : DMEM_DEFAULT_MINBLOCK;
	root->memPart     = partid;
	root->allocType   = type;
	root->heapList    = NULL;
	root->lastUse     = NULL;
	
	return (DmemUID)root;
}
