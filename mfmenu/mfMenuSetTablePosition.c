/* mfMenuSetTablePosition.c */

#include "mfmenu_types.h"

void mfMenuSetTablePosition( MfMenuTable *menu, unsigned int tableid, int x, int y )
{
	menu[--tableid].x = x;
	menu[tableid].y = y;
}
