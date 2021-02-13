/* pbGetCurrentDisplayBuffer.c */

#include "pb_types.h"

void pbGetCurrentDisplayBuffer( int *__restrict__ format, void **__restrict__ fbp, int *__restrict__ width )
{
	*format = __pb_internal_params.display->format;
	*fbp    = __pb_internal_params.display->addr;
	*width  = __pb_internal_params.display->width;
}
