/* pbReturnDisplay.c */

#include "pb_types.h"
#include <pspdisplay_kernel.h>

int pbReturnDisplay( int bufsync )
{
	if( ! ( __pb_internal_params.options & PB_STEAL_DISPLAY ) ) return 0;
	return sceDisplaySetFrameBufferInternal( 0, 0, __pb_internal_params.display->width, __pb_internal_params.display->format, bufsync );
}
