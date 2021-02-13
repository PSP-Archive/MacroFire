/* pbLineRect.c */

#include "pb_types.h"

void pbLineRect( int sx, int sy, int ex, int ey, uint32_t color )
{
	if( color == PB_TRANSPARENT || __pb_internal_params.options & PB_NO_DRAW ) return;
	
	pbLine( sx, sy, ex, sy, color );
	pbLine( ex, sy, ex, ey, color );
	pbLine( ex, ey, sx, ey, color );
	pbLine( sx, ey, sx, sy, color );
}
