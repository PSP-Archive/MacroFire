/* mfMenuGetCurrentButtons.c */

#include "mfmenu_types.h"

unsigned int mfMenuGetCurrentButtons( void )
{
	return __mfmenu__gParams->Ctrl.Pad->Buttons;
}
