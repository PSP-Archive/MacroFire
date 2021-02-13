/* xprintf_types.h                */
/* �����Ŏg�p�B�O����Q�Ƃ��Ȃ��B */

#include <pspkernel.h>
#include <pspsysclib.h>
#include <stdio.h>
#include <stdarg.h>

#define XPRINTF_PRNT_SIG_START_OF_STRING 0x200
#define XPRINTF_PRNT_SIG_END_OF_STRING   0x201

struct xprintf_ctx {
	char   *buf;
	const size_t *len;
	size_t cpylen;
};

void __xprintf_cb_sprnt( void *ctx, int ch );
void __xprintf_cb_snprnt( void *ctx, int ch );
