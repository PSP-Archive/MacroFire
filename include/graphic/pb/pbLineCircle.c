/* pbLineCircle.c */

#include "pb_types.h"

void pbLineCircle( int x, int y, unsigned int radius, uint32_t color )
{
	int cx = 0, cy = radius;
	int d = 3 - 2 * radius;
	
	if( color == PB_TRANSPARENT || __pb_internal_params.options & PB_NO_DRAW || ! radius ) return;
	
	/* 開始点 */
	pbPoint( x, y + radius, color ); /* ( 0, R) */
	pbPoint( x, y - radius, color ); /* ( 0,-R) */
	pbPoint( x + radius, y, color ); /* ( R, 0) */
	pbPoint( x - radius, y, color ); /* (-R, 0) */
	
	for( cx = 0; cx <= cy; cx++ ){
		if( d >= 0 ){
			d += 10 + 4 * cx - 4 * cy--;
		} else{
			d += 6 + 4 * cx;
		}
		
		pbPoint( x + cy, y + cx, color ); /*   0-45  度 */
		pbPoint( x + cx, y + cy, color ); /*  45-90  度 */
		pbPoint( x - cx, y + cy, color ); /*  90-135 度 */
		pbPoint( x - cy, y + cx, color ); /* 135-180 度 */
		pbPoint( x - cy, y - cx, color ); /* 180-225 度 */
		pbPoint( x - cx, y - cy, color ); /* 225-270 度 */
		pbPoint( x + cx, y - cy, color ); /* 270-315 度 */
		pbPoint( x + cy, y - cx, color ); /* 315-360 度 */
	}
}
