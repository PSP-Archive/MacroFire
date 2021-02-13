/* dirhSort.c */

#include "dirh_types.h"

static int dirh_default_sort( const void *a, const void *b );

void dirhSort( DirhUID uid, int ( *compare )( const void*, const void* ) )
{
	struct dirh_params *params = (struct dirh_params *)uid;
	
	qsort( params->data.entry.list, params->data.entry.count, sizeof( DirhFileInfo ), compare ? compare : dirh_default_sort );
}

static int dirh_default_sort( const void *a, const void *b )
{
	DirhFileInfo *file_a = (DirhFileInfo *)a;
	DirhFileInfo *file_b = (DirhFileInfo *)b;
	
	if( file_a->type == file_b->type ){
		return strcasecmp( file_a->name, file_b->name );
	} else if( file_a->type == DIRH_DIR ){
		return -1;
	} else{
		return 1;
	}
}
