/* pbPoint.c */

#include "pb_types.h"

void pbPoint( int x, int y, unsigned int color )
{
	PbColor pc;
	uintptr_t draw_addr;
	
	if( color == PB_TRANSPARENT || __pb_internal_params.options & PB_NO_DRAW ) return;
	
	pbColorParse8888( &pc, color );
	
	draw_addr = (uintptr_t)__pb_buf_addr( __pb_internal_params.draw, x, y );
	PB_PUT_PIXEL( draw_addr, &pc );
}
