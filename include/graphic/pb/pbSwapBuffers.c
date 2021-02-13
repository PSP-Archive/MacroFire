/* pbSwapBuffers.c */

#include "pb_types.h"

void *pbSwapBuffers( int bufsync )
{
	if( __pb_internal_params.draw->addr != __pb_internal_params.display->addr ){
		struct pb_frame_buffer *swap = __pb_internal_params.display;
		__pb_internal_params.display = __pb_internal_params.draw;
		__pb_internal_params.draw = swap;
		pbSyncDisplay( bufsync );
	}
	return __pb_internal_params.draw->addr;
}
