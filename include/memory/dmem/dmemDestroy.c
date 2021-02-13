/* dmemDestroy.c */

#include "dmem_types.h"

void dmemDestroy( DmemUID uid )
{
	struct dmem_root *root = (struct dmem_root *)uid;
	
	if( root->heapList ){
		struct dmem_heap *prev = root->heapList;
		struct dmem_heap *cur  = prev->next;
		
		while( cur ){
			heapDestroy( prev->uid );
			prev = cur;
			cur  = cur->next;
		}
		heapDestroy( prev->uid );
	}
	
	memoryFree( root );
}
