/* fiomgrWrite.c */

#include "fiomgr_types.h"

int fiomgrWrite( FiomgrHandle fh, void *data, size_t length )
{
	struct fiomgr_params *params = (struct fiomgr_params *)fh;
	int number_of_bytes_write = 0;
	
	if( ! length ) return 0;
	
	if( params->cache.lastState != FIOMGR_CACHE_LAST_WRITE ){
		__fiomgr_cache_clear( params );
		params->cache.lastState = FIOMGR_CACHE_LAST_WRITE;
	}
	
	if( ( FIOMGR_CACHE_SIZE - 1 ) - params->cache.length < length ){
		if( params->cache.length ){
			sceIoWrite( params->fd, params->cache.memory, params->cache.length );
			__fiomgr_cache_clear( params );
		}
	}
	
	if( length > ( FIOMGR_CACHE_SIZE - 1 ) ){
		number_of_bytes_write = sceIoWrite( params->fd, data, length );
	} else{
		memcpy( params->cache.memory + params->cache.length, data, length );
		params->cache.length += length;
	}
	
	return length;
}
