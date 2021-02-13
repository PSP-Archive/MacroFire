/* pbRestoreContext.c */

#include "pb_types.h"

void pbRestoreContext( PbContext context )
{
	__pb_internal_params = *((struct pb_params *)context);
}
