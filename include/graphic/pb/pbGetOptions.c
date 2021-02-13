/* pbGetOptions.c */

#include "pb_types.h"

unsigned int pbGetOptions( void )
{
	return __pb_internal_params.options;
}
