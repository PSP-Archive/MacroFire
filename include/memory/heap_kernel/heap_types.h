/* heap_types.h                   */
/* 内部で使用。外から参照しない。 */

#include "../heap_kernel.h"
#include <string.h>

#define __HEAP_MGMTAREA_SIZE 32 // ヒープ作成時に必ず32バイト消費される。多分管理用領域。

