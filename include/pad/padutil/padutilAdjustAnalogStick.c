/* padutilAdjustAnalogStick.c */

#include "padutil_types.h"

void padutilAdjustAnalogStick( const PadutilAnalogStick *analogstick, SceCtrlData *pad )
{
	if( PADUTIL_IN_DEADZONE( abs( pad->Lx - analogstick->originX ), abs( pad->Ly - analogstick->originY ), analogstick->deadzone ) ){
		pad->Lx = PADUTIL_CENTER_X;
		pad->Ly = PADUTIL_CENTER_Y;
	} else if( analogstick->sensitivity != 100 ){
		int x = analogstick->originX + (int)( ( pad->Lx - analogstick->originX ) * analogstick->sensitivity ) / 100;
		int y = analogstick->originY + (int)( ( pad->Ly - analogstick->originY ) * analogstick->sensitivity ) / 100;
		
		if( x > PADUTIL_MAX_COORD ){
			x = PADUTIL_MAX_COORD;
		} else if( x < 0 ){
			x = 0;
		}
		
		if( y > PADUTIL_MAX_COORD ){
			y = PADUTIL_MAX_COORD;
		} else if( y < 0 ){
			y = 0;
		}
		
		pad->Lx = x;
		pad->Ly = y;
	}
	
	if( 
		( analogstick->originX < PADUTIL_CENTER_X && pad->Lx > analogstick->originX && pad->Lx < PADUTIL_CENTER_X ) ||
		( analogstick->originX > PADUTIL_CENTER_X && pad->Lx < analogstick->originX && pad->Lx > PADUTIL_CENTER_X )
	){
		pad->Lx = PADUTIL_CENTER_X;
	}
	
	if(
		( analogstick->originY < PADUTIL_CENTER_Y && pad->Ly > analogstick->originY && pad->Ly < PADUTIL_CENTER_Y ) ||
		( analogstick->originY > PADUTIL_CENTER_Y && pad->Ly < analogstick->originY && pad->Ly > PADUTIL_CENTER_Y )
	){
		pad->Ly = PADUTIL_CENTER_Y;
	}
}
