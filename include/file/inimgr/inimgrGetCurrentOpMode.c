/* inimgrGetCurrentUID.c */

#include "inimgr_types.h"

InimgrOpMode inimgrGetCurrentOpMode( InimgrContext context )
{
	return ((struct inimgr_context *)context)->mode;
}
