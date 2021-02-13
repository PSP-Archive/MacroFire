/* __mfmenu_send_message_for_items.c */

#include "mfmenu_types.h"

void __mfmenu_send_message_for_items( MfMenuTable *menu, unsigned int menucnt, MfMessage message )
{
	unsigned int i, row, col;
	
	dbgprintf( "Sending message %X to menu items...", message );
	for( i = 0; i < menucnt; i++ ){
		for( row = 0; row < menu[i].rows; row++ ){
			for( col = 0; col < menu[i].cols; col++ ){
				dbgprintf( "Init \"%s\" (%d,%d,%d)", menu[i].entry[row][col].label, i, row, col );
				if( menu[i].entry[row][col].ctrl ) ( menu[i].entry[row][col].ctrl )(
					message,
					menu[i].entry[row][col].label,
					menu[i].entry[row][col].var,
					menu[i].entry[row][col].arg,
					NULL
				);
			}
		}
	}
	dbgprint( "Completed" );
}