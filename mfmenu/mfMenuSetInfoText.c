/* mfMenuSetInfoText.c */

#include "mfmenu_types.h"

void mfMenuSetInfoText( unsigned int options, char *format, ... )
{
	va_list ap;
	char *strtop = __mfmenu__gParams->Info.Text;
	
	__mfmenu__gParams->Info.Options = options;
	
	if( ( options & MF_MENU_INFOTEXT_COMMON_CTRL ) ){
		if( options & MF_MENU_INFOTEXT_MULTICOLUMN_CTRL ){
			strtop += snprintf( strtop, MF_MENU_INFOTEXT_LENGTH, "(%s%s%s%s)%s, ", PB_SYM_PSP_UP, PB_SYM_PSP_RIGHT, PB_SYM_PSP_DOWN, PB_SYM_PSP_LEFT, MF_STR_CTRL_MOVE );
		} else if( options & MF_MENU_INFOTEXT_MOVABLEPAGE_CTRL ){
			strtop += snprintf( strtop, MF_MENU_INFOTEXT_LENGTH, "(%s%s)%s, (%s%s)%s, ", PB_SYM_PSP_UP, PB_SYM_PSP_DOWN, MF_STR_CTRL_MOVE, PB_SYM_PSP_LEFT, PB_SYM_PSP_RIGHT, MF_STR_CTRL_PAGEMOVE );
		} else{
			strtop += snprintf( strtop, MF_MENU_INFOTEXT_LENGTH, "(%s%s)%s, ", PB_SYM_PSP_UP, PB_SYM_PSP_DOWN, MF_STR_CTRL_MOVE );
		}
		
		strtop += snprintf( strtop, MF_MENU_INFOTEXT_LENGTH, "(%s)%s, (START/HOME)%s", mfMenuCancelSymbol(), MF_STR_CTRL_BACK, MF_STR_CTRL_EXIT );
	}
	
	if( options & MF_MENU_INFOTEXT_SET_LOWER_LINE ){
		*strtop = '\n';
		strtop++;
	}
	
	va_start( ap, format );
	vsnprintf( strtop, MF_MENU_INFOTEXT_LENGTH - (unsigned int)( strtop - __mfmenu__gParams->Info.Text ), format, ap );
	va_end( ap );
}
