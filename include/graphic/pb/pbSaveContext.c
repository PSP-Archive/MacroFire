/* pbSaveContext.c */

#include "pb_types.h"

void pbSaveContext( PbContext context )
{
	context = (PbContext)(&__pb_internal_params);
}
