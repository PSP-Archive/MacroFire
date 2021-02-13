/* pbEnable.c */

#include "pb_types.h"

void pbEnable( unsigned int opt )
{
	__pb_internal_params.options |= opt;
}
