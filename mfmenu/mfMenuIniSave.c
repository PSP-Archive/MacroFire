/* mfMenuIniSave.c */

#include "mfmenu_types.h"

void mfMenuIniSave( InimgrUID ini, char *buf, size_t len )
{
	unsigned int i;
	PadutilButtonName button_names[] = { MF_MENU_ALT_BUTTONS };
	
	inimgrAddSection( ini, "AlternativeButtons" );
	
	for( i = 0; i < MF_MENU_ALT_BUTTONS_COUNT; i++ ){
		strlwr( button_names[i].name );
		button_names[i].name[0] = toupper( button_names[i].name[0] );
		
		inimgrSetString( ini, "AlternativeButtons", button_names[i].name, "" );
	}
	
	inimgrSetBool( ini, MF_INI_SECTION_DEFAULT, "ForcedLowMemoryMode", false );
}
