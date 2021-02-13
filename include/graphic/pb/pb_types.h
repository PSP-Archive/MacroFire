/* pb_types.h                     */
/* 内部で使用。外から参照しない。 */

#include "../pb.h"

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "cgerrno.h"

#define PB_SWAP( a, b ) { \
	int t = *(int *)a; \
	*(int *)a = *(int *)b; \
	*(int *)b = t; \
}

#define PB_BLEND_FUNC( src_ch, dst_ch, alpha, maxbits ) ( ( ( ( src_ch ) + 1 - ( dst_ch ) ) * ( alpha ) >> ( maxbits ) ) + ( dst_ch ) )

#define PB_PUT_PIXEL( dst_addr, pbcolor ) { \
	unsigned int fc; \
	if( __pb_internal_params.draw->colorConv ) fc = ( __pb_internal_params.draw->colorConv )( pbcolor, (void *)dst_addr ); \
	memcpy( (void *)dst_addr, (void *)&fc, __pb_internal_params.draw->pixelSize ); \
}

typedef unsigned int ( *color_convert  )( PbColor*, void* );

struct pb_frame_buffer {
	int  format;
	void *addr;
	int  width;
	
	unsigned char pixelSize;
	unsigned int  lineSize;
	
	color_convert   colorConv;
};

struct pb_params {
	struct pb_frame_buffer frame0, frame1;
	struct pb_frame_buffer *display, *draw;
	unsigned int   options;
};

extern struct pb_params __pb_internal_params;

void *__pb_buf_addr( struct pb_frame_buffer *fb, int x, int y );
