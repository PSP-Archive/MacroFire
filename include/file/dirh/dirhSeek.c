/* dirhSeek.c */

#include "dirh_types.h"

void dirhSeek( DirhUID uid, DirhWhence whence, int offset )
{
	struct dirh_params *params = (struct dirh_params *)uid;
	
	switch( whence ){
		case DIRH_SEEK_SET: params->data.entry.pos = 0; break;
		case DIRH_SEEK_END: params->data.entry.pos = params->data.entry.count; break;
		case DIRH_SEEK_CUR: break;
	}
	
	params->data.entry.pos += offset;
	
	if( params->data.entry.pos < 0 ){
		params->data.entry.pos = 0;
	} else if( params->data.entry.pos > params->data.entry.count ){
		params->data.entry.pos = params->data.entry.count;
	}
}
