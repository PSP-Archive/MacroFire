/* vsnprintf.c */

#include "xprintf_types.h"

int vsnprintf( char *buf, size_t n, const char *fmt, va_list ap )
{
	struct xprintf_ctx ctx = { buf, (const size_t *)&n, 0 };
	
	prnt( __xprintf_cb_snprnt, (void *)&ctx, fmt, ap );
	
	if( ctx.cpylen >= *(ctx.len) ){
		ctx.buf[*(ctx.len) - 1] = '\0';
	} else{
		ctx.buf[ctx.cpylen] = '\0';
	}
	
	return ctx.cpylen;
}
