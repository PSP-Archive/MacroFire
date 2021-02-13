/* mfMenuIniLoad.c */

#include "mfmenu_types.h"

void mfMenuIniLoad( InimgrUID ini, char *buf, size_t len )
{
	unsigned int i, j;
	PadutilButtonName button_names[] = { MF_MENU_ALT_BUTTONS };
	
	inimgrLoadSection( ini, "AlternativeButtons" );
	
	for( i = 0, j = 0; i < MF_MENU_ALT_BUTTONS_COUNT; i++ ){
		if( inimgrGetString( ini, "AlternativeButtons", button_names[i].name, buf, len ) > 0 ){
			if( ! __mfmenu__gPref.Remap ) __mfmenu__gPref.Remap = padutilCreateRemapArray( MF_MENU_ALT_BUTTONS_COUNT );
			__mfmenu__gPref.Remap[j].realButtons = mfConvertButtonN2C( buf );
			__mfmenu__gPref.Remap[j].remapButtons  = button_names[i].button;
		}
	}
	mfIniGetBool( ini, "ForcedLowMemoryMode", &(__mfmenu__gPref.LowMemoryMode) );
}
