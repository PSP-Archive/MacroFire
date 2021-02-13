/* pbIsEnabled.c */

#include "pb_types.h"

bool pbIsEnabled( unsigned int opt )
{
	return __pb_internal_params.options & opt ? true : false;
}
