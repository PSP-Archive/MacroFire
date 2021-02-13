/* fiomgrTell.c */

#include "fiomgr_types.h"

SceOff fiomgrTell( FiomgrHandle fh )
{
	struct fiomgr_params *params = (struct fiomgr_params *)fh;
	SceOff pos;
	
	if( params->flags & FH_O_LARGEFILE ){
		pos = sceIoLseek( params->fd, 0, FH_SEEK_CUR );
	} else{
		pos = sceIoLseek32( params->fd, 0, FH_SEEK_CUR );
	}
	
	if( params->cache.length ){
		switch( params->cache.lastState ){
			case FIOMGR_CACHE_LAST_READ:
				pos -= params->cache.length - params->cache.position;
				break;
			case FIOMGR_CACHE_LAST_WRITE:
				pos += params->cache.length;
				break;
		}
	}
	
	return pos;
}
