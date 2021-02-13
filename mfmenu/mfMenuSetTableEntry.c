/* mfMenuSetTableEntry.c */

#include "mfmenu_types.h"

void mfMenuSetTableEntry( MfMenuTable *menu, unsigned int tableid, unsigned int row, unsigned int col, char *label, MfControl ctrl, void *var, void *arg )
{
	menu[--tableid].entry[--row][--col].active = ctrl ? true : false;
	menu[tableid].entry[row][col].label        = label;
	menu[tableid].entry[row][col].ctrl         = ctrl;
	menu[tableid].entry[row][col].var          = var;
	menu[tableid].entry[row][col].arg          = arg;
}
