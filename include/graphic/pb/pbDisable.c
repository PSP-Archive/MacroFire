/* pbDisable.c */

#include "pb_types.h"

void pbDisable( unsigned int opt )
{
	__pb_internal_params.options &= ~opt;
}
