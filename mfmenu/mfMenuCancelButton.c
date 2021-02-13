/* mfMenuCancelButton.c */

#include "mfmenu_types.h"

unsigned int mfMenuCancelButton( void )
{
	return __mfmenu__gParams->Ctrl.Cancel.Button;
}
