/* fiomgrClose.c */

#include "fiomgr_types.h"

int fiomgrClose( FiomgrHandle fh )
{
	struct fiomgr_params *params = (struct fiomgr_params *)fh;
	int ret = __fiomgr_close( params );
	
	if( ! ( ret < 0 ) ) memoryFree( params );
	
	return ret;
}

int __fiomgr_close( struct fiomgr_params *params )
{
	if( ( params->cache.lastState != FIOMGR_CACHE_LAST_READ ) && params->cache.length ){
		__fiomgr_cache_flush( params );
	}
	
	return sceIoClose( params->fd );
}