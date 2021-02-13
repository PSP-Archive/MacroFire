/*=========================================================
	
	pb.h
	
	ピクセルの転送を行う。
	
	色のブレンドは( SRC_ALPHA, ONE_MINUS_SRC_ALPHA )相当のアルファブレンドのみ可能。
	
	計算式は
		http://elku.at.infoseek.co.jp/memo/alpha.html
	を参考にさせてもらった。
	
=========================================================*/
#ifndef PB_H
#define PB_H

#include <pspkernel.h>
#include <pspdisplay.h>
#include <pspge.h>
#include <stdbool.h>

/*=========================================================
	定数
=========================================================*/
#define PB_SCREEN_WIDTH  480
#define PB_SCREEN_HEIGHT 272
#define PB_VRAM_WIDTH    512

#ifndef SCR_WIDTH
#define SCR_WIDTH PB_SCREEN_WIDTH
#endif

#ifndef SCR_HEIGHT
#define SCR_HEIGHT PB_SCREEN_HEIGHT
#endif

#ifndef BUF_WIDTH
#define BUF_WIDTH PB_VRAM_WIDTH
#endif

/* 文字幅 */
#define PB_CHAR_WIDTH   6
#define PB_CHAR_HEIGHT  8
#define PB_WCHAR_WIDTH  8
#define PB_WCHAR_HEIGHT PB_CHAR_HEIGHT

/* ピクセルフォーマットの別名 */
#define PB_PXF_5650 PSP_DISPLAY_PIXEL_FORMAT_565
#define PB_PXF_5551 PSP_DISPLAY_PIXEL_FORMAT_5551
#define PB_PXF_4444 PSP_DISPLAY_PIXEL_FORMAT_4444
#define PB_PXF_8888 PSP_DISPLAY_PIXEL_FORMAT_8888

/* フレームバッファ同期? */
#define PB_BUFSYNC_IMMEDIATE PSP_DISPLAY_SETBUF_IMMEDIATE
#define PB_BUFSYNC_NEXTFRAME PSP_DISPLAY_SETBUF_NEXTFRAME

#define PB_DISABLE_CACHE 0x40000000
#define PB_TRANSPARENT 0

/* フレームバッファ設定のクリア */
#define PB_CLEAR_FRAMEBUF 0, NULL, 0

/* PSPボタン文字 */
#ifdef PB_SJIS_SUPPORT
#define PB_SYM_PSP_UP       "↑"
#define PB_SYM_PSP_RIGHT    "→"
#define PB_SYM_PSP_DOWN     "↓"
#define PB_SYM_PSP_LEFT     "←"
#define PB_SYM_PSP_TRIANGLE "△"
#define PB_SYM_PSP_CIRCLE   "○"
#define PB_SYM_PSP_CROSS    "×"
#define PB_SYM_PSP_SQUARE   "□"
#define PB_SYM_PSP_NOTE     "♪"
#else
#define PB_SYM_PSP_UP       "\x80"
#define PB_SYM_PSP_RIGHT    "\x81"
#define PB_SYM_PSP_DOWN     "\x82"
#define PB_SYM_PSP_LEFT     "\x83"
#define PB_SYM_PSP_TRIANGLE "\x84"
#define PB_SYM_PSP_CIRCLE   "\x85"
#define PB_SYM_PSP_CROSS    "\x86"
#define PB_SYM_PSP_SQUARE   "\x87"
#define PB_SYM_PSP_NOTE     "\x88"
#endif
#define PB_SYM_PSP_START    "START"
#define PB_SYM_PSP_SELECT   "SELECT"
#define PB_SYM_PSP_LTRIGGER "L"
#define PB_SYM_PSP_RTRIGGER "R"
#define PB_SYM_PSP_HOME     "HOME"

#ifdef __cplusplus
extern "C" {
#endif

/*=========================================================
	型宣言
=========================================================*/
typedef enum {
	PB_NO_CACHE      = 0x00000001,
	PB_BLEND         = 0x00000002,
	PB_DOUBLE_BUFFER = 0x00000004,
	PB_STEAL_DISPLAY = 0x00000008,
	PB_NO_DRAW       = 0x80000000
} PbOptions;

typedef struct {
	uint8_t red;
	uint8_t green;
	uint8_t blue;
	uint8_t alpha;
} PbColor;

typedef uintptr_t PbContext;

/*=========================================================
	関数
=========================================================*/
void pbInit( void );
void pbSaveContext( PbContext context );
void pbRestoreContext( PbContext context );
void pbSetDisplayBuffer( int format, void *fbp, int width );
void pbSetDrawBuffer( int format, void *fbp, int width );
void pbSetDisplayBufferUndef( void );
void pbSetDrawBufferUndef( void );
void pbGetCurrentDisplayBuffer( int *__restrict__ format, void **__restrict__ fbp, int *__restrict__ width );
void pbGetCurrentDrawBuffer( int *__restrict__ format, void **__restrict__ fbp, int *__restrict__ width );
void *pbGetCurrentDisplayBufferAddr( void );
void *pbGetCurrentDrawBufferAddr( void );
void pbSetOptions( unsigned int opt );
unsigned int pbGetOptions( void );
void pbEnable( unsigned int opt );
void pbDisable( unsigned int opt );
bool pbIsEnabled( unsigned int opt );
int pbApply( void );
int pbSyncDisplay( int bufsync );
int pbReturnDisplay( int bufsync );
void *pbSwapBuffers( int bufsync );
int pbGetPixelDataSize( int format );
int pbGetFrameBufferDataSize( int format, int width );
unsigned int pbMeasureNString( const char *str, int n );
#define pbMeasureString( s ) pbMeasureNString( ( s ), -1 )
void pbCopyData( void *addr, size_t lenght );
int pbPrint( int x, int y, unsigned int fg, unsigned int bg, const char *str );
int pbPutChar( int x, int y, unsigned int fg, unsigned int bg, const char chr );
int pbPrintf( int x, int y, unsigned int fg, unsigned int bg, const char *format, ... );
void pbPoint( int x, int y, unsigned int color );
void pbLine( int sx, int sy, int ex, int ey, uint32_t color );
void pbLineRect( int sx, int sy, int ex, int ey, uint32_t color );
void pbFillRect( int sx, int sy, int ex, int ey, uint32_t color );
void pbLineCircle( int x, int y, unsigned int radius, uint32_t color );
void pbColorParse5650( PbColor *cd, unsigned int color );
void pbColorParse5551( PbColor *cd, unsigned int color );
void pbColorParse4444( PbColor *cd, unsigned int color );
void pbColorParse8888( PbColor *cd, unsigned int color );
unsigned int pbColorGet5650( PbColor *cd );
unsigned int pbColorGet5551( PbColor *cd );
unsigned int pbColorGet4444( PbColor *cd );
unsigned int pbColorGet8888( PbColor *cd );

#define pbOffsetChar( n ) ( ( n ) * ( 6 ) )
#define pbOffsetLine( n ) ( ( n ) * 8 )

#define pbLineRel( x, y, e, f, c )     pbLine( x, y, ( x ) + ( e ), ( y ) + ( f ), c )
#define pbLineRectRel( x, y, w, h, c ) pbLineRect( x, y, ( x ) + ( w ), ( y ) + ( h ), c )
#define pbFillRectRel( x, y, w, h, c ) pbFillRect( x, y, ( x ) + ( w ), ( y ) + ( h ), c )

#ifdef __cplusplus
}
#endif

#endif
