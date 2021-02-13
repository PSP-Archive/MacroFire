/* fiomgrSeek.c */

#include "fiomgr_types.h"

SceOff fiomgrSeek( FiomgrHandle fh, SceOff offset, int whence )
{
	struct fiomgr_params *params = (struct fiomgr_params *)fh;
	SceOff pos;
	
	if( whence == FH_SEEK_CUR && offset == 0 ){
		return fiomgrTell( fh );
	}
	
	params->eof = false;
	
	if( params->cache.lastState != FIOMGR_CACHE_LAST_READ ){
		if( params->cache.length ) __fiomgr_cache_flush( params );
		params->cache.lastState = FIOMGR_CACHE_LAST_READ;
	}
	
	__fiomgr_cache_clear( params );
	
	if( params->flags & FH_O_LARGEFILE ){
		pos = sceIoLseek( params->fd, offset, whence );
	} else{
		pos = sceIoLseek32( params->fd, offset, whence );
	}
	
	return pos;
}
