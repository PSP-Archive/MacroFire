/* padutilSetAnalogStickDirection.c */

#include "padutil_types.h"

void padutilSetAnalogStickDirection( unsigned int analog_direction, PadutilCoord *x, PadutilCoord *y )
{
	if( analog_direction & PADUTIL_CTRL_ANALOG_UP    ) *y = 0;
	if( analog_direction & PADUTIL_CTRL_ANALOG_RIGHT ) *x = 255;
	if( analog_direction & PADUTIL_CTRL_ANALOG_DOWN  ) *y = 255;
	if( analog_direction & PADUTIL_CTRL_ANALOG_LEFT  ) *x = 0;
}
