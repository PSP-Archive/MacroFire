/* mfMenuInitTables.c */

#include "mfmenu_types.h"

void mfMenuInitTables( MfMenuTable menu[], unsigned int menucnt )
{
	__mfmenu__gParams->Menu.CurTable = 0;
	__mfmenu__gParams->Menu.CurRow   = 0;
	__mfmenu__gParams->Menu.CurCol   = 0;
	
	__mfmenu_send_message_for_items( menu, menucnt, MF_CM_INIT );
}
