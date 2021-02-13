/* mfMenuGetCurrentAnalogStickX.c */

#include "mfmenu_types.h"

unsigned char mfMenuGetCurrentAnalogStickX( void )
{
	return __mfmenu__gParams->Ctrl.Pad->Lx;
}
