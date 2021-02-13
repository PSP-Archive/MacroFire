/* pbInit.c */

#include "pb_types.h"

struct pb_params __pb_internal_params;

void pbInit( void )
{
	__pb_internal_params.frame0.format    = 0;
	__pb_internal_params.frame0.addr      = NULL;
	__pb_internal_params.frame0.width     = 0;
	__pb_internal_params.frame0.pixelSize = 0;
	__pb_internal_params.frame0.lineSize  = 0;
	__pb_internal_params.frame0.colorConv = NULL;
	
	__pb_internal_params.frame1.format    = 0;
	__pb_internal_params.frame1.addr      = NULL;
	__pb_internal_params.frame1.width     = 0;
	__pb_internal_params.frame1.pixelSize = 0;
	__pb_internal_params.frame1.lineSize  = 0;
	__pb_internal_params.frame1.colorConv = NULL;
	
	__pb_internal_params.display        = &(__pb_internal_params.frame0);
	__pb_internal_params.draw           = &(__pb_internal_params.frame1);
	__pb_internal_params.options        = 0;
}

void *__pb_buf_addr( struct pb_frame_buffer *fb, int x, int y )
{
	return (void *)( (uintptr_t)fb->addr + ( x + y * fb->width ) * fb->pixelSize );
}
