/* pbSetDrawBuffer.c */

#include "pb_types.h"

void pbSetDrawBuffer( int format, void *fbp, int width )
{
	__pb_internal_params.draw->format = format;
	__pb_internal_params.draw->addr   = fbp;
	__pb_internal_params.draw->width  = width;
}
