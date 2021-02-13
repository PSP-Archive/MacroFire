/* pbGetFrameBufferSize.c */

#include "pb_types.h"

int pbGetFrameBufferDataSize( int format, int width )
{
	return width * PB_SCREEN_HEIGHT * pbGetPixelDataSize( format );
}
