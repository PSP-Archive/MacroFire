/* pbColorParse5551.c */

#include "pb_types.h"

void pbColorParse5551( PbColor *cd, unsigned int color )
{
	cd->red   = ( color & 0x1F ) << 3;
	cd->green = ( ( color >> 5 ) & 0x1F ) << 3;
	cd->blue  = ( ( color >> 10 ) & 0x1F ) << 3;
	cd->alpha = ( ( color >> 15 ) & 0x01 ) ? 0xFF : 0;
}