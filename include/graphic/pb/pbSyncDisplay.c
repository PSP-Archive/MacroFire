/* pbSyncDisplay.c */

#include "pb_types.h"
#include <pspdisplay_kernel.h>

int pbSyncDisplay( int bufsync )
{
	if( __pb_internal_params.options & PB_STEAL_DISPLAY ){
		return sceDisplaySetFrameBufferInternal( 0, __pb_internal_params.display->addr, __pb_internal_params.display->width, __pb_internal_params.display->format, bufsync );
	} else{
		return sceDisplaySetFrameBuf( __pb_internal_params.display->addr, __pb_internal_params.display->width, __pb_internal_params.display->format, bufsync );
	}
}
