/*=========================================================

	cdialog/message.c

	メッセージダイアログ。

=========================================================*/
#include "message.h"

/*=========================================================
	ローカル型宣言
=========================================================*/

/*=========================================================
	ローカル関数
=========================================================*/
static void cdialog_message_count_width_and_height( const char *str, unsigned int *width, unsigned int *height );
static void cdialog_message_draw( struct cdialog_dev_base_params *base, CdialogMessageData *data );

/*=========================================================
	ローカル変数
=========================================================*/
static CdialogMessageParams *st_params;

/*=========================================================
	関数
=========================================================*/
int cdialogMessageInit( CdialogMessageParams *params )
{
	if( st_params ) return CG_ERROR_ALREADY_INITIALIZED;
	
	if( params ){
		st_params = params;
		st_params->destroySelf = false;
	} else{
		st_params = (CdialogMessageParams *)memoryExalloc( "MessageParams", MEMORY_USER, 0, sizeof( CdialogMessageParams ), PSP_SMEM_High, NULL );
		if( ! st_params ) return CG_ERROR_NOT_ENOUGH_MEMORY;
		st_params->destroySelf = true;
	}
	
	cdialogDevInitBaseParams( &(st_params->base) );
	
	/* 初期化 */
	st_params->data.title[0]   = '\0';
	st_params->data.message[0] = '\0';
	st_params->data.errorcode  = 0;
	st_params->data.options    = 0;
	st_params->data.width      = 0;
	st_params->data.height     = 0;
	st_params->data.callback   = NULL;
	
	return CG_ERROR_OK;
}

CdialogMessageData *cdialogMessageGetData( void )
{
	return &(st_params->data);
}

CdialogStatus cdialogMessageGetStatus( void )
{
	if( ! st_params ) return CDIALOG_NONE;
	return st_params->base.status;
}

CdialogResult cdialogMessageGetResult( void )
{
	return st_params->base.result;
}

int cdialogMessageStart( unsigned int x, unsigned int y )
{
	int ret;
	
	if( ! cdialogDevLock() ) return CG_ERROR_ALREADY_RUNNING;
	ret = cdialogMessageStartNoLock( x, y );
	
	if( ret < 0 ) cdialogDevUnlock();
	
	return ret;
}

int cdialogMessageStartNoLock( unsigned int x, unsigned int y )
{
	int ret;
	unsigned int w, h;
	
	st_params->base.status = CDIALOG_INIT;
	
	/* ダイアログの幅と高さを検出 */
	cdialog_message_count_width_and_height( st_params->data.message, &w, &h );
	
	/* 固定縦幅/横幅が指定されているか調べる */
	if( st_params->data.width  ) w = st_params->data.width;
	if( st_params->data.height ) h = st_params->data.height;
	
	/* 固定幅と固定行を加算 */
	st_params->base.width  = w + pbOffsetChar( 2 );
	st_params->base.height = h + pbOffsetLine( 6 );
	
	/* エラー出力の場合はエラーコード表示用の高さを確保 */
	if( st_params->data.options & CDIALOG_MESSAGE_ERROR ) st_params->base.height += pbOffsetLine( 2 );
	
	st_params->base.x = pbOffsetChar( x );
	st_params->base.y = pbOffsetLine( y );
	
	ret = cdialogDevPrepareToStart( &(st_params->base), st_params->data.options );
	if( ret != CG_ERROR_OK ) return ret;
	
	st_params->base.status = CDIALOG_VISIBLE;
	
	return 0;
}

int cdialogMessageUpdate( void )
{
	SceCtrlData pad;
	
	if( st_params->base.result != CDIALOG_UNKNOWN ){
		st_params->base.status = CDIALOG_SHUTDOWN;
		return 0;
	}
	
	cdialog_message_draw( &(st_params->base), &(st_params->data) );
	cdialogDevReadCtrlBuffer( &(st_params->base), &pad, NULL );
	
	if( pad.Buttons & cdialogDevAcceptButton() ){
		st_params->base.result = CDIALOG_ACCEPT;
	} else if( ( pad.Buttons & cdialogDevCancelButton() ) && ( st_params->data.options & CDIALOG_MESSAGE_YESNO ) ){
		st_params->base.result = CDIALOG_CANCEL;
	}
	
	return 0;
}

int cdialogMessageShutdownStartNoLock( void )
{
	if( st_params->base.status != CDIALOG_SHUTDOWN ){
		st_params->base.result = CDIALOG_CANCEL;
		st_params->base.status = CDIALOG_SHUTDOWN;
	}
	
	cdialogDevPrepareToFinish( &(st_params->base) );
	
	return 0;
}

int cdialogMessageShutdownStart( void )
{
	int ret = cdialogMessageShutdownStartNoLock();
	cdialogDevUnlock();
	return ret;
}

void cdialogMessageDestroy( void )
{
	if( st_params->destroySelf ){
		memoryFree( st_params );
	}
	st_params = NULL;
}

static void cdialog_message_count_width_and_height( const char *str, unsigned int *width, unsigned int *height )
{
	unsigned int cur_width = 0;
	const char *start = str;
	const char *end   = NULL;
	
	*width  = 0;
	*height = PB_CHAR_HEIGHT;
	
	while( ( end = strchr( start, '\n' ) ) ){
		*height += PB_CHAR_HEIGHT;
		cur_width = pbMeasureNString( start, (unsigned int)( end - start ) );
		
		if( cur_width > *width ) *width = cur_width;
		
		start = end + 1;
	}
	
	cur_width = pbMeasureString( start );
	if( cur_width > *width ) *width = cur_width;
}

static void cdialog_message_draw( struct cdialog_dev_base_params *base, CdialogMessageData *data )
{
	char buf[32];
	
	/* 枠を描画 */
	pbFillRectRel( base->x, base->y, base->width, base->height, base->color->bg );
	pbLineRectRel( base->x, base->y, base->width, base->height, base->color->border );
	
	/* タイトル描画 */
	pbPrint(
		base->x + ( base->width >> 1 ) - ( pbMeasureString( data->title ) >> 1 ),
		base->y + pbOffsetLine( 1 ),
		base->color->title,
		PB_TRANSPARENT,
		data->title
	);
	
	pbPrint(
		base->x + pbOffsetChar( 1 ),
		base->y + pbOffsetLine( 3 ),
		base->color->fg,
		PB_TRANSPARENT,
		data->message
	);
	
	if( data->callback ) ( data->callback )( base->x + pbOffsetChar( 1 ), base->y + pbOffsetLine( 3 ), base->color );
	
	if( data->options & CDIALOG_MESSAGE_ERROR ){
		snprintf( buf, sizeof( buf ), "(%X)", data->errorcode );
		pbPrintf(
			base->x + ( base->width >> 1 ) - ( pbMeasureString( buf ) >> 1 ),
			base->y + + base->height - pbOffsetLine( 4 ),
			base->color->fcfg,
			PB_TRANSPARENT,
			buf
		);
	}
	
	if( data->options & CDIALOG_MESSAGE_YESNO ){
		snprintf( buf, sizeof( buf ), "[%s]%s [%s]%s", cdialogDevAcceptSymbol(), CDIALOG_STR_MESSAGE_YES, cdialogDevCancelSymbol(), CDIALOG_STR_MESSAGE_NO );
	} else{
		snprintf( buf, sizeof( buf ), "[%s]%s", cdialogDevAcceptSymbol(), CDIALOG_STR_MESSAGE_OK );
	}
	
	pbPrint(
		base->x + ( base->width >> 1 ) - ( pbMeasureString( buf ) >> 1 ),
		base->y + + base->height - pbOffsetLine( 2 ),
		base->color->fcfg,
		PB_TRANSPARENT,
		buf
	);
}
