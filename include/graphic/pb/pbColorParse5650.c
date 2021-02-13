/* pbColorParse5650.c */

#include "pb_types.h"

void pbColorParse5650( PbColor *cd, unsigned int color )
{
	cd->red   = ( color & 0x1F ) << 3;
	cd->green = ( ( color >> 5 ) & 0x3F ) << 2;
	cd->blue  = ( ( color >> 11 ) & 0x1F ) << 3;
	cd->alpha = 0xFF;
}
