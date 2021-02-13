/* mfMenuGetCurrentPadData.c */

#include "mfmenu_types.h"

SceCtrlData *mfMenuGetCurrentPadData( void )
{
	return __mfmenu__gParams->Ctrl.Pad;
}
