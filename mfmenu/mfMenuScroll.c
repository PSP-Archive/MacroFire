/* mfMenuScroll.c */

#include "mfmenu_types.h"

unsigned int mfMenuScroll( int selected, unsigned int viewlines, unsigned int maxlines )
{
	int scroll_lines, line_number;
	
	for( scroll_lines = selected - ( viewlines >> 1 ), line_number = 0; scroll_lines > 0; scroll_lines--, line_number++ ){
		if( maxlines - line_number < viewlines ){
			break;
		}
	}
	
	return line_number;
}
