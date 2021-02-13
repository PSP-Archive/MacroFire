/* fiomgrReadln.c */

#include "fiomgr_types.h"

int fiomgrReadln( FiomgrHandle fh, char *str, size_t maxlength )
{
	struct fiomgr_params *params = (struct fiomgr_params *)fh;
	char *start, *lnbrk, lncode, nextcode;
	size_t read_bytes, str_length, free_length, copy_length;
	
	if( params->cache.lastState != FIOMGR_CACHE_LAST_READ ){
		if( params->cache.length ){
			__fiomgr_cache_flush( params );
			__fiomgr_cache_clear( params );
		}
		params->cache.lastState = FIOMGR_CACHE_LAST_READ;
	}
	
	/* I’[•¶Žš•ªŠm•Û */
	maxlength--;
	
	*str = '\0';
	str_length = 0;
	read_bytes = 0;
	do{
		start = params->cache.memory + params->cache.position;
		lnbrk = strpbrk( start, "\x0D\x0A" );
		
		free_length = maxlength - str_length;
		copy_length = lnbrk ? ( lnbrk - start ) : params->cache.length - params->cache.position;
		
		if( ! lnbrk && ! copy_length ){
			__fiomgr_cache_clear( params );
			if( params->eof ) break;
			__fiomgr_cache_load( params );
		} else if( copy_length > free_length ){
			memcpy( str + str_length, params->cache.memory + params->cache.position, free_length );
			str[maxlength] = '\0';
			params->cache.position += free_length;
			read_bytes += free_length;
			break;
		} else{
			memcpy( str + str_length, params->cache.memory + params->cache.position, copy_length );
			str_length             += copy_length;
			params->cache.position += copy_length;
			read_bytes             += copy_length;
			
			str[str_length] = '\0';
			
			if( lnbrk ){
				lncode   = *lnbrk;
				nextcode = 0;
				if( params->cache.length == params->cache.position ){
					__fiomgr_cache_clear( params );
					if( ! params->eof ){
						__fiomgr_cache_load( params );
						nextcode = *(params->cache.memory);
					}
				} else{
					nextcode = *(lnbrk + 1);
				}
				
				params->cache.position++;
				read_bytes++;
				if( lncode == '\x0D' ){
					if( nextcode == '\x0A' ){
						params->cache.position++;
						read_bytes++;
					}
				} else if( nextcode == '\x0D' ){
					params->cache.position++;
					read_bytes++;
				}
			} else{
				__fiomgr_cache_clear( params );
				__fiomgr_cache_load( params );
			}
		}
	} while( ! lnbrk );
	
	return read_bytes;
}
