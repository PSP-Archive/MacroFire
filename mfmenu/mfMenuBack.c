/* mfMenuBack.c */

#include "mfmenu_types.h"

void mfMenuBack( void )
{
	__mfmenu__gParams->Message.Main |= MF_MENU_IM_BACK;
}
