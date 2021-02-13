/* fiomgrRead.c */

#include "fiomgr_types.h"

int fiomgrRead( FiomgrHandle fh, void *data, size_t length )
{
	struct fiomgr_params *params = (struct fiomgr_params *)fh;
	
	if( ! length ) return 0;
	
	if( params->cache.lastState != FIOMGR_CACHE_LAST_READ ){
		if( params->cache.length ){
			__fiomgr_cache_flush( params );
			__fiomgr_cache_clear( params );
		}
		params->cache.lastState = FIOMGR_CACHE_LAST_READ;
	}
	
	if( length > params->cache.length - params->cache.position ){
		int number_of_bytes_read = params->cache.length - params->cache.position;
		int number_of_bytes_rest = length - number_of_bytes_read;
		
		if( number_of_bytes_read ) memcpy( data, params->cache.memory + params->cache.position, number_of_bytes_read );
		__fiomgr_cache_clear( params );
		
		if( params->eof ){
			return number_of_bytes_read;
		} else if( number_of_bytes_rest > FIOMGR_CACHE_SIZE ){
			/* 読み込み量がキャッシュサイズを超える場合は、キャッシュせずに直接読み出す */
			int non_cached_number_of_bytes_read = sceIoRead( params->fd, data + number_of_bytes_read, number_of_bytes_rest );
			if( non_cached_number_of_bytes_read < number_of_bytes_rest ) params->eof = true;
			return number_of_bytes_read + non_cached_number_of_bytes_read;
		}
		
		__fiomgr_cache_load( params );
		
		params->cache.position = length - number_of_bytes_read;
		
		if( params->cache.length < number_of_bytes_rest ){
			memcpy( data + number_of_bytes_read, params->cache.memory, params->cache.length );
			params->cache.position = params->cache.length;
		} else{
			memcpy( data + number_of_bytes_read, params->cache.memory, number_of_bytes_rest );
			params->cache.position = number_of_bytes_rest;
		}
		return number_of_bytes_read + params->cache.position;
	} else{
		memcpy( data, params->cache.memory + params->cache.position, length );
		params->cache.position += length;
		return length;
	}
}
