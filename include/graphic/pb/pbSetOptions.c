/* pbSetOptions.c */

#include "pb_types.h"

void pbSetOptions( unsigned int opt )
{
	__pb_internal_params.options = opt;
}
