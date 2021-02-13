/* pbColorGet5650.c */

#include "pb_types.h"

unsigned int pbColorGet5650( PbColor *cd )
{
	return ( ( cd->red & 0xF8 ) >> 3 | ( cd->green & 0xFC ) << 3 | ( cd->blue & 0xF8 ) << 8 );
}