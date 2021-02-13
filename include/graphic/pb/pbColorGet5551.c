/* pbColorGet5551.c */

#include "pb_types.h"

unsigned int pbColorGet5551( PbColor *cd )
{
	return ( ( cd->red & 0xF8 ) >> 3 | ( cd->green & 0xF8 ) << 2 | ( cd->blue & 0xF8 ) << 7 | ( cd->alpha > 0x7F ? 1 : 0 ) << 15 );
}