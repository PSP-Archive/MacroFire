/* mfMenuDeactivateTableEntry.c */

#include "mfmenu_types.h"

void mfMenuDeactivateTableEntry( MfMenuTable *menu, unsigned int tableid, unsigned int row, unsigned int col )
{
	__mfmenu_table_entry_activator( &(menu[tableid - 1]), row, col, false );
}
