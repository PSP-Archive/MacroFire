/* pbGetCurrentDrawBufferAddr.c */

#include "pb_types.h"

void *pbGetCurrentDisplayBufferAddr( void )
{
	return __pb_internal_params.display->addr;
}
