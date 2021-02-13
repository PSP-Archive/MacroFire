/* pbFillRect.c */

#include "pb_types.h"

void pbFillRect( int sx, int sy, int ex, int ey, uint32_t color )
{
	PbColor      pc;
	uintptr_t    start_addr, draw_addr;
	unsigned int offset;
	int          w, h, x;
	
	if( color == PB_TRANSPARENT || __pb_internal_params.options & PB_NO_DRAW ) return;
	
	pbColorParse8888( &pc, color );
	
	if( sx > ex ) PB_SWAP( &sx, &ex );
	if( sy > ey ) PB_SWAP( &sy, &ey );
	
	w = ex - sx;
	h = ey - sy;
	
	start_addr = (uintptr_t)__pb_buf_addr( __pb_internal_params.draw, sx, sy );
	for( ; h; h--, start_addr += __pb_internal_params.draw->lineSize ){
		offset = 0;
		for( x = 0; x < w; x++, offset += __pb_internal_params.draw->pixelSize ){
			draw_addr = start_addr + offset;
			PB_PUT_PIXEL( draw_addr, &pc );
		}
	}
}
