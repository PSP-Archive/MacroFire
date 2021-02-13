/* pbColorGet8888.c */

#include "pb_types.h"

unsigned int pbColorGet8888( PbColor *cd )
{
	return ( cd->red | cd->green << 8 | cd->blue << 16 | cd->alpha << 24 );
}