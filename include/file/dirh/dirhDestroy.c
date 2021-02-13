/* dirhDestroy.c */

#include "dirh_types.h"

void dirhDestroy( DirhUID uid )
{
	if( ((struct dirh_params *)uid)->data.entry.list ) __dirh_clear_entries( &(((struct dirh_params *)uid)->data.entry) );
	
	sceKernelDeleteSema( ((struct dirh_params *)uid)->semaId );
	memoryFree( (void *)uid );
}
