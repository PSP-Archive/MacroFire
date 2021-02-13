/* mfMenuSetTableLabel.c */

#include "mfmenu_types.h"

void mfMenuSetTableLabel( MfMenuTable *menu, unsigned int tableid, char *label )
{
	menu[--tableid].label = label;
}
