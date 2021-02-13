/* __inimgr_read_line.c */

#include "inimgr_types.h"

char *__inimgr_read_line( FiomgrHandle fh, char *buffer, size_t len )
{
	char *line_start = NULL;
	int ret = fiomgrReadln( fh, buffer, len );
	if( ret ) line_start = strutilCounterPbrk( buffer, "\x20\t" );
	return line_start;
}
