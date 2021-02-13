/* hook_types.h                   */
/* 内部で使用。外から参照しない。 */

#include "../hook.h"

#include <pspkernel.h>
#include <psputilsforkernel.h>
#include <stdbool.h>
#include <string.h>

/* PSPLinkから拝借 */
struct hook_syscall_table_header
{
	void *next; /* 次のテーブル(って何?)へのポインタ */
	unsigned int offset;                    /* offset to substract from syscall code (substractって何?) */
	unsigned int num;                       /* テーブルの全エントリ数 */
	unsigned int size;                      /* テーブル全体のサイズ(バイト単位) */
};
