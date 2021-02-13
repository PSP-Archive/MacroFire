/* mfMenuTermTables.c */

#include "mfmenu_types.h"

void mfMenuTermTables( MfMenuTable menu[], unsigned int menucnt )
{
	__mfmenu_send_message_for_items( menu, menucnt, MF_CM_TERM );
}
