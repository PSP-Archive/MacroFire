/* __mfmenu_table_entry_activator.c */

#include "mfmenu_types.h"

void __mfmenu_table_entry_activator( MfMenuTable *menu, unsigned int row, unsigned int col, bool mode )
{
	if( ! row && ! col ){
		for( row = 0; row < menu->rows; row++ ){
			for( col = 0; col < menu->cols; col++ ){
				menu->entry[row][col].active = mode;
			}
		}
	} else{
		row--;
		col--;
		menu->entry[row][col].active = mode;
	}
}