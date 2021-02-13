/* mfMenuCancelSymbol.c */

#include "mfmenu_types.h"

char *mfMenuCancelSymbol( void )
{
	return __mfmenu__gParams->Ctrl.Cancel.Symbol;
}
