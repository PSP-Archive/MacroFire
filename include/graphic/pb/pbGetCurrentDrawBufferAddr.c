/* pbGetCurrentDrawBufferAddr.c */

#include "pb_types.h"

void *pbGetCurrentDrawBufferAddr( void )
{
	return __pb_internal_params.draw->addr;
}
