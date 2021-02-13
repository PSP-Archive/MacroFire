/* padutilGetAnalogStickDirection.c */

#include "padutil_types.h"

unsigned int padutilGetAnalogStickDirection( PadutilCoord x, PadutilCoord y, PadutilCoord deadzone )
{
	unsigned int direction = 0;
	unsigned char nx, ny;
	nx = abs( x - PADUTIL_CENTER_X );
	ny = abs( y - PADUTIL_CENTER_Y );
	
	if( nx == 0 && ny == 0 ) return 0;
	
	if( ny > (int)(sin( PADUTIL_DEGREE_TO_RADIAN( PADUTIL_INVALID_RIGHT_TRIANGLE_DEGREE ) ) * nx) ){
		if( y > PADUTIL_CENTER_Y ){
			direction |= PADUTIL_CTRL_ANALOG_DOWN;
		} else{
			direction |= PADUTIL_CTRL_ANALOG_UP;
		}
	}
	
	if( nx > (int)(sin( PADUTIL_DEGREE_TO_RADIAN( PADUTIL_INVALID_RIGHT_TRIANGLE_DEGREE ) ) * ny) ){
		if( x > PADUTIL_CENTER_X ){
			direction |= PADUTIL_CTRL_ANALOG_RIGHT;
		} else{
			direction |= PADUTIL_CTRL_ANALOG_LEFT;
		}
	}
	
	return direction;
}
