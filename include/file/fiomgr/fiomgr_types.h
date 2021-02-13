/* fiomgr_types.h                 */
/* 内部で使用。外から参照しない。 */

#include "../fiomgr.h"

#include <stdbool.h>
#include "memory/memory.h"
#include "util/strutil.h"
#include "cgerrno.h"

#define FIOMGR_CACHE_SIZE 1024

enum fiomgr_cache_last_state {
	FIOMGR_CACHE_LAST_READ = 0,
	FIOMGR_CACHE_LAST_WRITE
};

struct fiomgr_cache_params {
	char    memory[FIOMGR_CACHE_SIZE];
	size_t length, position;
	enum fiomgr_cache_last_state lastState;
};

struct fiomgr_params {
	char    *path;
	int     flags;
	SceMode mode;
	
	SceUID    fd;
	bool      eof;
	struct fiomgr_cache_params cache;
};

void __fiomgr_cache_flush( struct fiomgr_params *params );
void __fiomgr_cache_clear( struct fiomgr_params *params );
void __fiomgr_cache_load( struct fiomgr_params *params );
int __fiomgr_close( struct fiomgr_params *fh );
