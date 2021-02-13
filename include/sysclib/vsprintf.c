/* vsprintf.c */

#include "xprintf_types.h"

int vsprintf( char *buf, const char *fmt, va_list ap )
{
	struct xprintf_ctx ctx = { buf, NULL, 0 };
	
	prnt( __xprintf_cb_sprnt, (void *)&ctx, fmt, ap );
	ctx.buf[ctx.cpylen] = '\0';
	
	return ctx.cpylen;
}
