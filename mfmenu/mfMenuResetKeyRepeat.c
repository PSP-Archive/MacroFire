/* mfMenuResetKeyRepeat.c */

#include "mfmenu_types.h"

void mfMenuResetKeyRepeat( void )
{
	padctrlResetRepeat( __mfmenu__gParams->Ctrl.Uid );
}
