/* mfMenuButtonsSymbolList.c */

#include "mfmenu_types.h"

char *mfMenuButtonsSymbolList( PadutilButtons buttons, char *str, size_t len )
{
	padutilGetButtonNamesByCode( __mfmenu__gParams->ButtonSymbolTable, buttons, " ", 0, str, len );
	return str;
}
