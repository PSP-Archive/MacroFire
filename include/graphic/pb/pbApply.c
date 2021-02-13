/* pbApply.c */

#include "pb_types.h"

static unsigned int pb_color_8888( PbColor *pc, void *null )
{
	return pbColorGet8888( pc );
}

static unsigned int pb_color_8888_5650( PbColor *pc, void *null )
{
	return pbColorGet5650( pc );
}

static unsigned int pb_color_8888_5551( PbColor *pc, void *null )
{
	return pbColorGet5551( pc );
}

static unsigned int pb_color_8888_4444( PbColor *pc, void *null )
{
	return pbColorGet4444( pc );
}

static unsigned int pb_color_blend_8888( PbColor *pc, void *dst_addr )
{
	uint32_t dst = *((uint32_t *)dst_addr);
	
	if( ! pc->alpha ){
		return dst;
	} else if( pc->alpha != 0xFF ){
		PbColor color;
		color.red   = PB_BLEND_FUNC( pc->red,   ( dst       ) & 0xFF, pc->alpha, 8 );
		color.green = PB_BLEND_FUNC( pc->green, ( dst >>  8 ) & 0xFF, pc->alpha, 8 );
		color.blue  = PB_BLEND_FUNC( pc->blue,  ( dst >> 16 ) & 0xFF, pc->alpha, 8 );
		return pbColorGet8888( &color );
	} else{
		return pbColorGet8888( pc );
	}
}

static unsigned int pb_color_blend_4444( PbColor *pc, void *dst_addr )
{
	uint16_t dst = *((uint16_t *)dst_addr);
	
	if( ! pc->alpha ){
		return dst;
	} else if( pc->alpha != 0xFF ){
		PbColor color;
		color.red   = PB_BLEND_FUNC( pc->red,   ( dst & 0xF   ) << 4, pc->alpha, 8 );
		color.green = PB_BLEND_FUNC( pc->green, ( dst & 0xF0  ),      pc->alpha, 8 );
		color.blue  = PB_BLEND_FUNC( pc->blue,  ( dst & 0xF00 ) >> 4, pc->alpha, 8 );
		return pbColorGet4444( &color );
	} else{
		return pbColorGet4444( pc );
	}
}

static unsigned int pb_color_blend_5551( PbColor *pc, void *dst_addr )
{
	uint16_t dst = *((uint16_t *)dst_addr);
	
	if( ! pc->alpha ){
		return dst;
	} else if( pc->alpha != 0xFF ){
		PbColor color;
		color.red   = PB_BLEND_FUNC( pc->red,   ( dst & 0x1F   ) << 3, pc->alpha, 8 );
		color.green = PB_BLEND_FUNC( pc->green, ( dst & 0x3E0  ) >> 2, pc->alpha, 8 );
		color.blue  = PB_BLEND_FUNC( pc->blue,  ( dst & 0x7C00 ) >> 7, pc->alpha, 8 );
		return pbColorGet5551( &color );
	} else{
		return pbColorGet5551( pc );
	}
}

static unsigned int pb_color_blend_5650( PbColor *pc, void *dst_addr )
{
	uint16_t dst = *((uint16_t *)dst_addr);
	
	if( ! pc->alpha ){
		return dst;
	} else if( pc->alpha != 0xFF ){
		PbColor color;
		color.red   = PB_BLEND_FUNC( pc->red,   ( dst & 0x1F   ) << 3, pc->alpha, 8 );
		color.green = PB_BLEND_FUNC( pc->green, ( dst & 0x7E0  ) >> 3, pc->alpha, 8 );
		color.blue  = PB_BLEND_FUNC( pc->blue,  ( dst & 0xF800 ) >> 8, pc->alpha, 8 );
		return pbColorGet5650( &color );
	} else{
		return pbColorGet5650( pc );
	}
}

static color_convert pb_get_color_converter( int format )
{
	switch( format ){
		case PB_PXF_8888: return pb_color_8888;
		case PB_PXF_4444: return pb_color_8888_4444;
		case PB_PXF_5551: return pb_color_8888_5551;
		case PB_PXF_5650: return pb_color_8888_5650;
		default:          return NULL;
	}
}

static color_convert pb_get_color_blender( int format )
{
	switch( format ){
		case PB_PXF_8888: return pb_color_blend_8888;
		case PB_PXF_4444: return pb_color_blend_4444;
		case PB_PXF_5551: return pb_color_blend_5551;
		case PB_PXF_5650: return pb_color_blend_5650;
		default:          return NULL;
	}
}

static void pb_set_framebuf_conf( struct pb_frame_buffer *fb, unsigned int opt )
{
	fb->pixelSize = pbGetPixelDataSize( fb->format );
	fb->lineSize = fb->width * fb->pixelSize;
	if( opt & PB_NO_CACHE ){
		fb->addr = (void *)( (uintptr_t)fb->addr | PB_DISABLE_CACHE );
	} else{
		fb->addr = (void *)( (uintptr_t)fb->addr & ~PB_DISABLE_CACHE );
	}
	if( opt & PB_BLEND ){
		fb->colorConv = pb_get_color_blender( fb->format );
	} else{
		fb->colorConv = pb_get_color_converter( fb->format );
	}
}

int pbApply( void )
{
	if( ! __pb_internal_params.display->addr ){
		return CG_ERROR_INVALID_ARGUMENT;
	} else{
		pb_set_framebuf_conf( __pb_internal_params.display, __pb_internal_params.options );
	}
	
	if( __pb_internal_params.options & PB_DOUBLE_BUFFER ){
		pb_set_framebuf_conf( __pb_internal_params.draw, __pb_internal_params.options );
	} else{
		*(__pb_internal_params.draw) = *(__pb_internal_params.display);
	}
	
	return 0;
}
