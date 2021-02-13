/* pbSetDisplayBuffer.c */

#include "pb_types.h"

void pbSetDisplayBuffer( int format, void *fbp, int width )
{
	__pb_internal_params.display->format = format;
	__pb_internal_params.display->addr   = fbp;
	__pb_internal_params.display->width  = width;
}

