/* fiomgrWriteln.c */

#include "fiomgr_types.h"

int fiomgrWriteln( FiomgrHandle fh, void *data, size_t length )
{
	int ret = fiomgrWrite( fh, data, length );
	if( ret >= 0 ) ret += fiomgrWrite( fh, "\x0D\x0A", 2 );
	return ret;
}
