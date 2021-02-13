/* dirh_types.h                   */
/* 内部で使用。外から参照しない。 */

#include "../dirh.h"

#include <stdlib.h>
#include <string.h>
#include "memory/dmem.h"
#include "util/makepath.h"
#include "cgerrno.h"

enum dirh_event {
	DIRH_EVENT_OK    = 0x00000001,
	DIRH_EVENT_EXIT  = 0x00000002
};

struct dirh_cwd {
	char  *path;
	size_t max;
};

struct dirh_entry {
	DirhFileInfo *list;
	unsigned int count;
	int          pos;
};

struct dirh_data {
	struct dirh_cwd   cwd;
	struct dirh_entry entry;
};

struct dirh_thread_dopen_params {
	SceUID eventId;
	struct dirh_data *data;
	int status;
};

struct dirh_params {
	SceUID semaId;
	struct dirh_data data;
	unsigned int options;
};

void __dirh_clear_entries( struct dirh_entry *entry );
