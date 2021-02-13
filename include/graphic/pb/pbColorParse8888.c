/* pbColorParse8888.c */

#include "pb_types.h"

void pbColorParse8888( PbColor *cd, unsigned int color )
{
	cd->red   = color & 0xFF;
	cd->green = ( color >> 8 ) & 0xFF;
	cd->blue  = ( color >> 16 ) & 0xFF;
	cd->alpha = ( color >> 24 ) & 0xFF;
}
