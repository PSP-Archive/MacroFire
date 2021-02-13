/* inimgr_types.h                 */
/* 内部で使用。外から参照しない。 */

#include "../inimgr.h"

#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include "memory/dmem.h"
#include "file/fiomgr.h"
#include "util/strutil.h"
#include "cgtypes.h"
#include "cgerrno.h"

#ifdef PSP_USE_KERNEL_LIBC
#include "sysclib.h"
#endif

#define INIMGR_DEFAULT_SECTION ".DEFAULT"

#ifdef __cplusplus
extern "C" {
#endif

enum inimgr_internal_flags {
	INIMGR_INTFLAG_MERGED = 0x00000001,
	INIMGR_INTFLAG_DELETE = 0x00000002
};
	
struct inimgr_entry {
	unsigned int flags;
	char         *key;
	char         *value;
	struct inimgr_entry *prev, *next;
};

struct inimgr_section {
//	unsigned int flags;
	char   *name;
	SceOff offsetStart, offsetEnd;
	struct inimgr_callback *callback;
	struct inimgr_entry    *entry;
	struct inimgr_section *prev, *next;
};

struct inimgr_callback {
	char *wildcard;
	InimgrCallback func;
	void *arg;
	struct inimgr_callback *next;
};

struct inimgr_params {
	DmemUID dmem;
	FiomgrHandle ini;
	char   buffer[INIMGR_ENTRY_LENGTH];
	struct inimgr_callback *callback;
	struct inimgr_section *last, *section;
};

struct inimgr_context {
	struct inimgr_params  *params;
	struct inimgr_section *section;
	InimgrOpMode mode;
};

char *__inimgr_read_line( FiomgrHandle fh, char *buffer, size_t len );
struct inimgr_section *__inimgr_create_section( struct inimgr_params *params, const char *section );
struct inimgr_callback *__inimgr_find_callback( struct inimgr_params *params, const char *name );
struct inimgr_section *__inimgr_find_section( struct inimgr_params *params, const char *name );
struct inimgr_entry *__inimgr_find_entry( struct inimgr_section *section, const char *key );
int __inimgr_get_value( struct inimgr_params *params, const char *name, const char *key, char **result );
int __inimgr_set_value( struct inimgr_params *params, struct inimgr_section *section, const char *key, const char *value );

#ifdef __cplusplus
}
#endif
