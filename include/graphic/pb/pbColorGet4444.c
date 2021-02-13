/* pbColorGet4444.c */

#include "pb_types.h"

unsigned int pbColorGet4444( PbColor *cd )
{
	return ( ( cd->red & 0xF0 ) >> 4 | ( cd->green & 0xF0 ) | ( cd->blue & 0xF0 ) <<  4 | ( cd->alpha & 0xF0 ) << 8);
}