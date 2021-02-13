/* pbPrint.c */

#include "pb_types.h"

/*-----------------------------------------------
	文字コード変換
	
	参考にしたサイト: http://xaiax.at.infoseek.co.jp/ConvertUnicode_java.html
-----------------------------------------------*/
#ifdef PB_SJIS_SUPPORT
#include "fonts/misaki8x8.c"
static unsigned int pb_get_glyph_index_by_sjis( unsigned char sjis_hi, unsigned char sjis_lo )
{
	unsigned char kuten_hi = sjis_hi - 0x81;
	unsigned int  kuten_lo = sjis_lo - 0x40;
	
	if( sjis_hi > 0x9F ) kuten_hi -= 0x40;
	if( sjis_lo > 0x7E ) kuten_lo--;
	
	kuten_hi <<= 1;
	if( kuten_lo > 0x5D ) kuten_hi++;
	
	kuten_lo %= 0x5E;
	
	/* 16区以降であれば、未定義の9～15区分を引く */
	if( kuten_hi >= 15 ){
		kuten_hi -= 7;
		kuten_lo--;
	}
	
	return kuten_hi * 94 + kuten_lo;
}
#else
#include "fonts/font_cg.c"
#endif

int pbPrint( int x, int y, unsigned int fg, unsigned int bg, const char *str )
{
	PbColor        *pc, f_pc, b_pc;
	uintptr_t      start_addr, put_addr;
	unsigned int   i, glyph_index;
	unsigned int   glyph_x, glyph_y;
	const char     *glyph;
	unsigned char  glyph_line_data, chr_width, chr_width_bytes;
	
	if( __pb_internal_params.options & PB_NO_DRAW ) return 0;
	
	pbColorParse8888( &f_pc, fg );
	pbColorParse8888( &b_pc, bg );
	
#ifndef PB_SJIS_SUPPORT
	chr_width       = PB_CHAR_WIDTH;
	chr_width_bytes = PB_CHAR_WIDTH * __pb_internal_params.draw->pixelSize;
#endif
	
	start_addr = (uintptr_t)__pb_buf_addr( __pb_internal_params.draw, x, y );
	put_addr   = start_addr;
	for( i = 0; str[i]; i++ ){
		if( (unsigned char)str[i] == '\n' ){
			start_addr += __pb_internal_params.draw->pixelSize * __pb_internal_params.draw->width * PB_CHAR_HEIGHT;
			put_addr = start_addr;
			continue;
		}
		
		/* グリフ取得 */
#ifdef PB_SJIS_SUPPORT
		if( (unsigned char)str[i] < 0x80 ){
			glyph = &(font_ascii[(unsigned char)str[i] * PB_CHAR_HEIGHT]);
			chr_width = PB_CHAR_WIDTH;
		} else if( (unsigned char)str[i] >= 0xA0 && (unsigned char)str[i] <= 0xDF ){
			glyph = &(font_ascii[(unsigned char)(str[i] - 0xA0) + 0x80]);
			chr_width = PB_CHAR_WIDTH;
		} else if( str[i + 1] ){
			glyph_index = pb_get_glyph_index_by_sjis( str[i], str[i + 1] );
			glyph = &(font_misaki[glyph_index * PB_CHAR_HEIGHT]);
			chr_width = PB_WCHAR_WIDTH;
			i++;
		} else{
			continue;
		}
		chr_width_bytes = chr_width * __pb_internal_params.draw->pixelSize;
#else
		glyph_index = (unsigned char)str[i] * PB_CHAR_HEIGHT;
		if( glyph_index < sizeof( font_cg ) ){
			glyph = &(font_cg[glyph_index]);
		} else{
			glyph = &(font_cg['?' * PB_CHAR_HEIGHT]);
		}
#endif
		
		/* 描画 */
		for( glyph_y = 0; glyph_y < PB_CHAR_HEIGHT; glyph_y++, put_addr += __pb_internal_params.draw->lineSize - chr_width_bytes ){
			glyph_line_data = glyph[glyph_y];
			for( glyph_x = 0; glyph_x < chr_width; glyph_x++, glyph_line_data <<= 1, put_addr += __pb_internal_params.draw->pixelSize ){
				pc = NULL;
				if( glyph_line_data & 0x80 ){
					if( fg != PB_TRANSPARENT ) pc = &f_pc;
				} else{
					if( bg != PB_TRANSPARENT ) pc = &b_pc;
				}
				if( pc ) PB_PUT_PIXEL( put_addr, pc );
			}
		}
		put_addr -= __pb_internal_params.draw->pixelSize * __pb_internal_params.draw->width * PB_CHAR_HEIGHT - chr_width_bytes;
	}
	return i;
}
