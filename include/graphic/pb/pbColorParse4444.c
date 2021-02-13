/* pbColorParse4444.c */

#include "pb_types.h"

void pbColorParse4444( PbColor *cd, unsigned int color )
{
	cd->red   = ( color & 0xF ) << 4;
	cd->green = ( ( color >> 4 ) & 0xF ) << 4;
	cd->blue  = ( ( color >> 8 ) & 0xF ) << 4;
	cd->alpha = ( ( color >> 12 ) & 0xF ) << 4;
}
