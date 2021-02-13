/* mfMenuGetCurrentAnalogStickY.c */

#include "mfmenu_types.h"

unsigned char mfMenuGetCurrentAnalogStickY( void )
{
	return __mfmenu__gParams->Ctrl.Pad->Ly;
}
