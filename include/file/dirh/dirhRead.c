/* dirhRead.c */

#include "dirh_types.h"

DirhFileInfo *dirhRead( DirhUID uid )
{
	struct dirh_params *params = (struct dirh_params *)uid;
	
	if( params->data.entry.count <= params->data.entry.pos ) return NULL;
	
	return &(params->data.entry.list[params->data.entry.pos++]);
}
