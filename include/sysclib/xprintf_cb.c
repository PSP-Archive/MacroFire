/* xprintf_cb.c */

#include "xprintf_types.h"

void __xprintf_cb_sprnt( void *ctx, int ch )
{
	if( ch == XPRINTF_PRNT_SIG_START_OF_STRING || ch == XPRINTF_PRNT_SIG_END_OF_STRING ) return;
	(((struct xprintf_ctx *)ctx)->buf)[((struct xprintf_ctx *)ctx)->cpylen++] = ch;
}

void __xprintf_cb_snprnt( void *ctx, int ch )
{
	if( ch == XPRINTF_PRNT_SIG_START_OF_STRING || ch == XPRINTF_PRNT_SIG_END_OF_STRING ) return;
		
	if( (((struct xprintf_ctx *)ctx)->cpylen) < *(((struct xprintf_ctx *)ctx)->len) )
		(((struct xprintf_ctx *)ctx)->buf)[((struct xprintf_ctx *)ctx)->cpylen] = ch;
	
	(((struct xprintf_ctx *)ctx)->cpylen)++;
}
