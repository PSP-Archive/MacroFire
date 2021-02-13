/* pbGetCurrentDrawBuffer.c */

#include "pb_types.h"

void pbGetCurrentDrawBuffer( int *__restrict__ format, void **__restrict__ fbp, int *__restrict__ width )
{
	*format = __pb_internal_params.draw->format;
	*fbp    = __pb_internal_params.draw->addr;
	*width  = __pb_internal_params.draw->width;
}
