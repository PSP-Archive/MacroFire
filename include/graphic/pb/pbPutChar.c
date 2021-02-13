/* pbPutChar.c */

#include "pb_types.h"

int pbPutChar( int x, int y, unsigned int fg, unsigned int bg, const char chr )
{
	const char str[] = { chr, '\0' };
	return pbPrint( x, y, fg, bg, str );
}
