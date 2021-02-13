/* mfMenuSetColumnWidth.c */

#include "mfmenu_types.h"

void mfMenuSetColumnWidth( MfMenuTable *menu, unsigned int tableid, unsigned int col, unsigned int width )
{
	if( ! menu[--tableid].colsWidth ){
		dbgprintf( "Table %d of menu(%p) is not multi-columns", tableid, menu );
		return;
	}
	menu[tableid].colsWidth[--col] = width;
}
