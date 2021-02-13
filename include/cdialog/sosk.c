/*=========================================================

	cdialog/sosk.c

	簡易OSK。

=========================================================*/
#include "sosk.h"

/*=========================================================
	ローカルマクロ
=========================================================*/
#define CDIALOG_SOSK_LAST_INPUTTYPE CDIALOG_SOSK_INPUTTYPE_FILENAME_SYMBOL
#define CDIALOG_SOSK_BASE_HEIGHT    pbOffsetLine( 7 )
#define CDIALOG_SOSK_OVERLAY_HEIGHT pbOffsetLine( 1 )

/*=========================================================
	ローカル関数
=========================================================*/
static bool cdialog_sosk_select_next_table( unsigned int types, struct cdialog_sosk_work_chtab *chtab );
static int  cdialog_sosk_calc_height( unsigned int types );
static void cdialog_sosk_draw( struct cdialog_dev_base_params *base, CdialogSoskData *data, struct cdialog_sosk_work *work );
static void cdialog_sosk_input_char( struct cdialog_sosk_work_text *text, char chr, size_t len );
static void cdialog_sosk_input_bs( struct cdialog_sosk_work_text *text );
static void cdialog_sosk_cursor_forward( struct cdialog_sosk_work_text *text );
static void cdialog_sosk_cursor_back( struct cdialog_sosk_work_text *text );
static void cdialog_sosk_focus_normalize( struct cdialog_sosk_work_chtab *chtab );
static void cdialog_sosk_focus_left( struct cdialog_sosk_work_chtab *chtab );
static void cdialog_sosk_focus_right( struct cdialog_sosk_work_chtab *chtab );
static void cdialog_sosk_focus_up( struct cdialog_sosk_work_chtab *chtab );
static void cdialog_sosk_focus_down( struct cdialog_sosk_work_chtab *chtab );

/*=========================================================
	ローカル変数
=========================================================*/
static CdialogSoskCharTable st_chartab[] = {
	{ "abcdefghijklmnopqrstuvwxyz",         13, 1, 1 },
	{ "ABCDEFGHIJKLMNOPQRSTUVWXYZ",         13, 1, 1 },
	{ "0123456789",                          0, 1, 1 },
	{ "!\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~", 13, 1, 1 },
	{ "!#$%&'()*+,-.;=@[]^_`{}~",           13, 1, 1 }
};

static CdialogSoskParams *st_params;

/*=========================================================
	関数
=========================================================*/
int cdialogSoskInit( CdialogSoskParams *params )
{
	if( st_params ) return CG_ERROR_ALREADY_INITIALIZED;
	
	if( params ){
		st_params = params;
		st_params->destroySelf = false;
	} else{
		st_params = (CdialogSoskParams *)memoryExalloc( "SoskParams", MEMORY_USER, 0, sizeof( CdialogSoskParams ), PSP_SMEM_High, NULL );
		if( ! st_params ) return CG_ERROR_NOT_ENOUGH_MEMORY;
		st_params->destroySelf = true;
	}
	
	cdialogDevInitBaseParams( &(st_params->base) );
	
	/* 初期化 */
	st_params->data.title[0] = '\0';
	st_params->data.options  = 0;
	st_params->data.text     = NULL;
	st_params->data.textMax  = 0;
	st_params->data.types    = 0;
	
	return CG_ERROR_OK;
}

CdialogSoskData *cdialogSoskGetData( void )
{
	return &(st_params->data);
}

CdialogStatus cdialogSoskGetStatus( void )
{
	if( ! st_params ) return CDIALOG_NONE;
	return st_params->base.status;
}

CdialogResult cdialogSoskGetResult( void )
{
	return st_params->base.result;
}

int cdialogSoskStart( unsigned int x, unsigned int y )
{
	int ret;
	
	if( ! cdialogDevLock() ) return CG_ERROR_ALREADY_RUNNING;
	ret = cdialogSoskStartNoLock( x, y );
	
	if( ret < 0 ) cdialogDevUnlock();
	
	return ret;
}

int cdialogSoskStartNoLock( unsigned int x, unsigned int y )
{
	int ret;
	
	st_params->base.status = CDIALOG_INIT;
	
	st_params->edited      = false;
	st_params->showMessage = false;
	st_params->cancel      = false;
	
	st_params->work.text.buf       = NULL;
	st_params->work.text.length    = 0;
	st_params->work.text.offset    = 0;
	st_params->work.text.cursorPos = 0;
	
	st_params->work.chtab.type  = 0;
	st_params->work.chtab.data  = NULL;
	st_params->work.chtab.count = 0;
	st_params->work.chtab.pos   = 0;
	
	if( ! st_params->data.types || ! st_params->data.text ) return CG_ERROR_INVALID_ARGUMENT;
	
	st_params->work.text.buf  = memoryAlloc( st_params->data.textMax );
	if( ! st_params->work.text.buf ) return CG_ERROR_NOT_ENOUGH_MEMORY;
	strutilCopy( st_params->work.text.buf, st_params->data.text, st_params->data.textMax );
	
	st_params->work.text.length = strlen( st_params->work.text.buf ) + 1;
	if( st_params->work.text.length > CDIALOG_SOSK_INPUT_AREA_WIDTH ){
		st_params->work.text.offset    = st_params->work.text.length - CDIALOG_SOSK_INPUT_AREA_WIDTH;
		st_params->work.text.cursorPos = CDIALOG_SOSK_INPUT_AREA_WIDTH - 1;
	} else{
		st_params->work.text.offset    = 0;
		st_params->work.text.cursorPos = st_params->work.text.length - 1;
	}
	
	st_params->work.chtab.lastPos = 0;
	st_params->work.chtab.pos     = 0;
	if( ! cdialog_sosk_select_next_table( st_params->data.types, &(st_params->work.chtab) ) ) return CG_ERROR_INVALID_ARGUMENT;
	
	st_params->base.x      = pbOffsetChar( x );
	st_params->base.y      = pbOffsetLine( y );
	st_params->base.width  = pbOffsetChar( CDIALOG_SOSK_INPUT_AREA_WIDTH + 4 );
	st_params->base.height = cdialog_sosk_calc_height( st_params->data.types );
	
	ret = cdialogDevPrepareToStart( &(st_params->base), st_params->data.options );
	if( ret != CG_ERROR_OK ) return ret;
	cdialogDevSetRepeatButtons(
		&(st_params->base),
		PSP_CTRL_UP | PSP_CTRL_RIGHT | PSP_CTRL_DOWN | PSP_CTRL_LEFT |
		cdialogDevAcceptButton() | PSP_CTRL_SQUARE | PSP_CTRL_TRIANGLE |
		PSP_CTRL_LTRIGGER | PSP_CTRL_RTRIGGER
	);
	
	st_params->base.status = CDIALOG_VISIBLE;
	
	return 0;
}

int cdialogSoskUpdate( void )
{
	SceCtrlData pad;
	
	if( st_params->base.result != CDIALOG_UNKNOWN ){
		st_params->base.status = CDIALOG_SHUTDOWN;
		return 0;
	}
	
	cdialog_sosk_draw( &(st_params->base), &(st_params->data), &(st_params->work) );
	
	if( st_params->showMessage ){
		CdialogStatus status;
		
		cdialogMessageUpdate();
		status = cdialogMessageGetStatus();
		if( status == CDIALOG_SHUTDOWN ){
			cdialogMessageShutdownStartNoLock();
			if( st_params->cancel ){
				st_params->cancel = cdialogMessageGetResult() == CDIALOG_ACCEPT ? true : false;
			}
			cdialogMessageDestroy();
			st_params->showMessage = false;
			padctrlResetRepeat( st_params->base.paduid );
			memoryFree( st_params->base.help );
		} else{
			return 0;
		}
	}
	
	if( st_params->cancel ){
		st_params->base.result = CDIALOG_CANCEL;
		return 0;
	}
	
	cdialogDevReadCtrlBuffer( &(st_params->base), &pad, NULL );
	
	if( pad.Buttons & cdialogDevCancelButton() ){
		st_params->cancel = true;
		if( st_params->edited && cdialogMessageInit( NULL ) == 0 ){
			CdialogMessageData *data = cdialogMessageGetData();
			strutilCopy( data->title,   st_params->data.title, CDIALOG_MESSAGE_TITLE_LENGTH );
			strutilCopy( data->message, CDIALOG_STR_SOSK_CANCEL, CDIALOG_MESSAGE_LENGTH );
			data->options = CDIALOG_DISPLAY_CENTER | CDIALOG_MESSAGE_YESNO;
			if( cdialogMessageStartNoLock( 0, 0 ) < 0 ){
				cdialogMessageShutdownStartNoLock();
			} else{
				st_params->showMessage = true;
			}
		}
	} else if( pad.Buttons & PSP_CTRL_START ){
		strcpy( st_params->data.text, st_params->work.text.buf );
		st_params->base.result = CDIALOG_ACCEPT;
	} else if( pad.Buttons & cdialogDevAcceptButton() ){
		cdialog_sosk_input_char( &(st_params->work.text), st_params->work.chtab.data->codes[st_params->work.chtab.pos], st_params->data.textMax );
		st_params->edited = true;
	} else if( pad.Buttons & PSP_CTRL_SQUARE ){
		cdialog_sosk_input_bs( &(st_params->work.text) );
		st_params->edited = true;
	} else if( pad.Buttons & PSP_CTRL_TRIANGLE && ! ( st_params->data.options & CDIALOG_SOSK_DENY_WHITESPACE ) ){
		cdialog_sosk_input_char( &(st_params->work.text), '\x20', st_params->data.textMax );
		st_params->edited = true;
	} else if( pad.Buttons & PSP_CTRL_LTRIGGER ){
		cdialog_sosk_cursor_back( &(st_params->work.text) );
	} else if( pad.Buttons & PSP_CTRL_RTRIGGER ){
		cdialog_sosk_cursor_forward( &(st_params->work.text) );
	} else if( pad.Buttons & PSP_CTRL_LEFT ){
		cdialog_sosk_focus_left( &(st_params->work.chtab) );
	} else if( pad.Buttons & PSP_CTRL_RIGHT ){
		cdialog_sosk_focus_right( &(st_params->work.chtab) );
	} else if( pad.Buttons & PSP_CTRL_UP ){
		cdialog_sosk_focus_up( &(st_params->work.chtab) );
	} else if( pad.Buttons & PSP_CTRL_DOWN ){
		cdialog_sosk_focus_down( &(st_params->work.chtab) );
	} else if( pad.Buttons & PSP_CTRL_SELECT ){
		cdialog_sosk_select_next_table( st_params->data.types, &(st_params->work.chtab) );
	} else if( pad.Buttons & PSP_CTRL_HOME ){
		st_params->base.help = memoryAlloc( sizeof( CdialogDevHelp ) * 16 );
		if( st_params->base.help && cdialogMessageInit( NULL ) == 0 ){
			CdialogMessageData *data = cdialogMessageGetData();
			data->options = CDIALOG_DISPLAY_CENTER;
			strutilCopy( data->title, CDIALOG_STR_HELP_LABEL, CDIALOG_MESSAGE_TITLE_LENGTH );
			data->width  = pbOffsetChar( CDIALOG_SOSK_HELP_WIDTH );
			data->height = pbOffsetLine( 11 );
			data->callback = cdialogDevDrawHelp;
			cdialogDevSetHelp( st_params->base.help, 16 );
			
			cdialogDevHelp( &st_params->base.help[0],  0,                 0,                 PB_SYM_PSP_UP PB_SYM_PSP_RIGHT PB_SYM_PSP_DOWN PB_SYM_PSP_LEFT );
			cdialogDevHelp( &st_params->base.help[1],  pbOffsetChar( 6 ), 0,                 CDIALOG_STR_SOSK_HELP_MOVE );
			cdialogDevHelp( &st_params->base.help[2],  0,                 pbOffsetLine(  1 ), PB_SYM_PSP_LTRIGGER "/" PB_SYM_PSP_RTRIGGER );
			cdialogDevHelp( &st_params->base.help[3],  pbOffsetChar( 6 ), pbOffsetLine(  1 ), CDIALOG_STR_SOSK_HELP_MOVECURSOR );
			cdialogDevHelp( &st_params->base.help[4],  0,                 pbOffsetLine(  3 ), cdialogDevAcceptSymbol() );
			cdialogDevHelp( &st_params->base.help[5],  pbOffsetChar( 3 ), pbOffsetLine(  3 ), CDIALOG_STR_SOSK_HELP_INPUT );
			cdialogDevHelp( &st_params->base.help[6],  0,                 pbOffsetLine(  4 ), PB_SYM_PSP_TRIANGLE );
			cdialogDevHelp( &st_params->base.help[7],  pbOffsetChar( 3 ), pbOffsetLine(  4 ), CDIALOG_STR_SOSK_HELP_WHITESPACE );
			cdialogDevHelp( &st_params->base.help[8],  0,                 pbOffsetLine(  5 ), PB_SYM_PSP_SQUARE );
			cdialogDevHelp( &st_params->base.help[9],  pbOffsetChar( 3 ), pbOffsetLine(  5 ), CDIALOG_STR_SOSK_HELP_BACKSPACE );
			cdialogDevHelp( &st_params->base.help[10], 0,                 pbOffsetLine(  7 ), PB_SYM_PSP_SELECT );
			cdialogDevHelp( &st_params->base.help[11], pbOffsetChar( 7 ), pbOffsetLine(  7 ), CDIALOG_STR_SOSK_HELP_CHANGELAYOUT );
			cdialogDevHelp( &st_params->base.help[12], 0,                 pbOffsetLine(  9 ), PB_SYM_PSP_START );
			cdialogDevHelp( &st_params->base.help[13], pbOffsetChar( 6 ), pbOffsetLine(  9 ), CDIALOG_STR_HELP_ACCEPT );
			cdialogDevHelp( &st_params->base.help[14], 0,                 pbOffsetLine( 10 ), cdialogDevCancelSymbol() );
			cdialogDevHelp( &st_params->base.help[15], pbOffsetChar( 6 ), pbOffsetLine( 10 ), CDIALOG_STR_HELP_CANCEL );
			
			if( cdialogMessageStartNoLock( 0, 0 ) < 0 ){
				cdialogMessageShutdownStartNoLock();
			} else{
				st_params->showMessage = true;
			}
		}
	}
	
	return 0;
}

int cdialogSoskShutdownStartNoLock( void )
{
	if( st_params->base.status != CDIALOG_SHUTDOWN ){
		st_params->base.result = CDIALOG_CANCEL;
		st_params->base.status = CDIALOG_SHUTDOWN;
	}
	
	memoryFree( st_params->work.text.buf );
	cdialogDevPrepareToFinish( &(st_params->base) );
	
	return 0;
}

int cdialogSoskShutdownStart( void )
{
	int ret = cdialogSoskShutdownStartNoLock();
	cdialogDevUnlock();
	return ret;
}

void cdialogSoskDestroy( void )
{
	if( st_params->destroySelf ){
		memoryFree( st_params );
	}
	st_params = NULL;
}

static bool cdialog_sosk_select_next_table( unsigned int types, struct cdialog_sosk_work_chtab *chtab )
{
	unsigned int index;
	unsigned int current_type;
	
	if( ! types ) return false;
	
	if( chtab->type == 1 ){
		current_type = types & ~1;
	} else{
		current_type = types & ~( chtab->type | ( chtab->type - 1 ) );
		if( ! current_type ) current_type = types;
	}
	current_type =( ( current_type - 1 ) ^ current_type ) & current_type;
	
	if( ! current_type || current_type > CDIALOG_SOSK_LAST_INPUTTYPE ) current_type = types;
	current_type = ( ( current_type - 1 ) - current_type ) & current_type;
	
	chtab->type = current_type;
	
	for( index = 0; current_type ^ 1; index++, current_type >>= 1 );
	
	chtab->data  = &(st_chartab[index]);
	chtab->count = strlen( chtab->data->codes ) - 1;
	if( ! chtab->lastPos ) chtab->lastPos = chtab->pos;
	
	if( chtab->lastPos <= chtab->count ){
		chtab->pos = chtab->lastPos;
	} else if( chtab->pos > chtab->count ){
		chtab->pos = chtab->count;
	}
	
	return true;
}

static int cdialog_sosk_calc_height( unsigned int types )
{
	struct cdialog_sosk_work_chtab chtab;
	CdialogSoskInputType type;
	unsigned int height = 0, current_height;
	
	cdialog_sosk_select_next_table( types, &chtab );
	type = chtab.type;
	
	do{
		if( chtab.data->width ){
			current_height = ( chtab.count / chtab.data->width ) * 2 + ( ( chtab.count % chtab.data->width ) ? 2 : 0 );
		} else{
			current_height = 1 + chtab.data->lineHeight;
		}
		if( current_height > height ) height = current_height;
		
		cdialog_sosk_select_next_table( types, &chtab );
	} while( chtab.type != type );
	
	return pbOffsetLine( height + 1 ) + CDIALOG_SOSK_BASE_HEIGHT;
}

static void cdialog_sosk_draw( struct cdialog_dev_base_params *base, CdialogSoskData *data, struct cdialog_sosk_work *work )
{
	char textblock[CDIALOG_SOSK_INPUT_AREA_WIDTH + 1];
	unsigned int i;
	unsigned int kbx = 0, kby = 0;
	unsigned int x = 0, y = 0;
	
	strutilCopy( textblock, work->text.buf + work->text.offset, CDIALOG_SOSK_INPUT_AREA_WIDTH + 1 );
	
	
	if( work->chtab.data->width ){
		x = work->chtab.data->width;
		y = ( work->chtab.count / work->chtab.data->width ) + ( work->chtab.count % work->chtab.data->width ? 1 : 0 );
	} else{
		x = work->chtab.count + 1;
		y = 1;
	}
	
	x   = pbOffsetChar( x * ( work->chtab.data->letterSpace + 1 ) + 1 );
	y   = pbOffsetLine( y * ( work->chtab.data->lineHeight  + 1 ) + 1 );
	kbx = base->x + ( base->width  >> 1 ) - ( x >> 1 );
	kby = base->y + CDIALOG_SOSK_BASE_HEIGHT + ( pbOffsetLine( 1 ) >> 1 );
	
	pbFillRectRel( kbx, kby - CDIALOG_SOSK_OVERLAY_HEIGHT, x, y + CDIALOG_SOSK_OVERLAY_HEIGHT + pbOffsetLine( 2 ), base->color->bg );
	pbLineRectRel( kbx, kby - CDIALOG_SOSK_OVERLAY_HEIGHT, x, y + CDIALOG_SOSK_OVERLAY_HEIGHT + pbOffsetLine( 2 ), base->color->border );
	
	pbPrintf( kbx + x - pbOffsetChar( 12 ), kby + y, base->color->help, PB_TRANSPARENT, "SELECT: [%c]", work->chtab.data->codes[0] );
	
	/* 枠を描画 */
	pbFillRectRel( base->x, base->y, base->width, CDIALOG_SOSK_BASE_HEIGHT, base->color->bg );
	pbLineRectRel( base->x, base->y, base->width, CDIALOG_SOSK_BASE_HEIGHT, base->color->border );
	
	/* タイトル描画 */
	pbPrint(
		base->x + ( base->width >> 1 ) - pbOffsetChar( strlen( data->title ) >> 1 ),
		base->y + pbOffsetLine( 1 ),
		base->color->title,
		PB_TRANSPARENT,
		data->title
	);
	
	/* 文字の残りを表示 */
	if( work->text.offset ){
		pbPrint(
			base->x + pbOffsetChar( 1 ) - ( pbOffsetChar( 1 ) >> 1 ),
			base->y + pbOffsetLine( 3 ),
			base->color->fg,
			PB_TRANSPARENT,
			"\x83"
		);
	}
	if( work->text.length > CDIALOG_SOSK_INPUT_AREA_WIDTH && work->text.offset < work->text.length - CDIALOG_SOSK_INPUT_AREA_WIDTH ){
		pbPrint(
			base->x + pbOffsetChar( 2 + CDIALOG_SOSK_INPUT_AREA_WIDTH ) + ( pbOffsetChar( 1 ) >> 1 ),
			base->y + pbOffsetLine( 3 ),
			base->color->fg,
			PB_TRANSPARENT,
			"\x81"
		);
	}
	
	/* 入力欄表示 */
	pbLineRel( 
		base->x + pbOffsetChar( 2 ),
		base->y + pbOffsetLine( 4 ) + 1,
		pbOffsetChar( CDIALOG_SOSK_INPUT_AREA_WIDTH ),
		0,
		base->color->fg
	);
	
	/* カーソル表示 */
	pbPrint(
		base->x + pbOffsetChar( 2 + work->text.cursorPos ),
		base->y + pbOffsetLine( 3 ),
		PB_TRANSPARENT,
		base->color->fcbg,
		" "
	);
	
	pbPrint(
		base->x + pbOffsetChar( 2 + work->text.cursorPos ),
		base->y + pbOffsetLine( 3 ) + 2,
		base->color->fcfg,
		PB_TRANSPARENT,
		"_"
	);
	
	/* 文字列表示 */
	pbPrint(
		base->x + pbOffsetChar( 2 ),
		base->y + pbOffsetLine( 3 ),
		base->color->fg,
		PB_TRANSPARENT,
		textblock
	);
	
	/* 文字数表示 */
	pbPrintf(
		base->x + pbOffsetChar( 1 ),
		base->y + pbOffsetLine( 4 ) + ( pbOffsetLine( 1 ) >> 1 ),
		base->color->fg,
		PB_TRANSPARENT,
		"( %d / %d )",
		work->text.length - 1,
		data->textMax - 1
	);
	
	/* ヘルプ案内表示 */
	pbPrintf(
		base->x + pbOffsetChar( 24 ),
		base->y + pbOffsetLine( 4 ) + ( pbOffsetLine( 1 ) >> 1 ),
		base->color->help,
		PB_TRANSPARENT,
		"HOME: %s",
		CDIALOG_STR_HELP_LABEL
	);
	
	for( i = 0, x = 0, y = 0; i <= work->chtab.count; i++ ){
		if( work->chtab.pos == i ){
			pbPutChar(
				kbx + pbOffsetChar( 1 + x ),
				kby + pbOffsetLine( 1 + y ),
				base->color->fcfg,
				base->color->fcbg,
				work->chtab.data->codes[i]
			);
		} else{
			pbPutChar(
				kbx + pbOffsetChar( 1 + x ),
				kby + pbOffsetLine( 1 + y ),
				base->color->fg,
				PB_TRANSPARENT,
				work->chtab.data->codes[i]
			);
		}
		x += 1 + work->chtab.data->letterSpace;
		if( i && work->chtab.data->width && ! ( ( i + 1 ) % work->chtab.data->width ) ){
			x = 0;
			y += 1 + work->chtab.data->lineHeight;
		}
	}
}

static void cdialog_sosk_cursor_forward( struct cdialog_sosk_work_text *text )
{
	if( text->offset + text->cursorPos >= text->length - 1 ) return;
	
	if( text->cursorPos >= CDIALOG_SOSK_INPUT_AREA_WIDTH - 4 && text->offset + CDIALOG_SOSK_INPUT_AREA_WIDTH <= text->length - 1 ){
		text->offset++;
	} else{
		text->cursorPos++;
	}
}

static void cdialog_sosk_cursor_back( struct cdialog_sosk_work_text *text )
{
	if( text->offset + text->cursorPos <= 0 ) return;
	
	if( text->cursorPos <= 3 && text->offset ){
		text->offset--; 
	} else{
		text->cursorPos--;
	}
}

static void cdialog_sosk_input_char( struct cdialog_sosk_work_text *text, char chr, size_t len )
{
	int offset = text->offset + text->cursorPos;
	
	if( text->length + 1 > len ) return;
	
	if( text->buf[offset] == '\0' ){
		text->buf[offset + 1] = '\0';
	} else{
		memmove( text->buf + offset + 1, text->buf + offset, text->length - offset );
	}
	text->buf[offset] = chr;
	
	text->length++;
	
	cdialog_sosk_cursor_forward( text );
}

static void cdialog_sosk_input_bs( struct cdialog_sosk_work_text *text )
{
	int offset = text->offset + text->cursorPos;
	
	if( ! offset ) return;
	
	if( text->buf[offset] == '\0' ){
		text->buf[offset - 1] = '\0';
	} else{
		memmove( text->buf + offset - 1, text->buf + offset, text->length - offset );
	}
	
	text->length--;
	
	cdialog_sosk_cursor_back( text );
}

static void cdialog_sosk_focus_normalize( struct cdialog_sosk_work_chtab *chtab )
{
	chtab->lastPos = 0;
	
	if( chtab->pos < 0 ){
		chtab->pos = 0;
	} else if( chtab->pos > chtab->count ){
		chtab->pos = chtab->count;
	}
}

static void cdialog_sosk_focus_left( struct cdialog_sosk_work_chtab *chtab )
{
	if( chtab->data->width && ( ! chtab->pos || chtab->pos % chtab->data->width == 0 ) ){
		chtab->pos += chtab->data->width - 1;
	} else{
		if( ! chtab->pos ){
			chtab->pos = chtab->count;
		} else{
			chtab->pos--;
		}
	}
	cdialog_sosk_focus_normalize( chtab );
}

static void cdialog_sosk_focus_right( struct cdialog_sosk_work_chtab *chtab )
{
	chtab->pos++;
	
	if( chtab->data->width && ( chtab->pos % chtab->data->width == 0 ) ){
		chtab->pos -= chtab->data->width;
	} else if( chtab->pos > chtab->count ){
		if( ! chtab->data->width ){
			chtab->pos = 0;
		} else{
			chtab->pos = chtab->data->width * ( chtab->pos / chtab->data->width );
		}
	}
	cdialog_sosk_focus_normalize( chtab );
}

static void cdialog_sosk_focus_up( struct cdialog_sosk_work_chtab *chtab )
{
	if( ! chtab->data->width ) return;
	
	if( chtab->pos >= chtab->data->width ){
		chtab->pos -= chtab->data->width;
	} else{
		chtab->pos = ( chtab->count - ( chtab->count % chtab->data->width ) ) + ( chtab->pos % chtab->data->width );
		while( chtab->pos > chtab->count ) chtab->pos -= chtab->data->width;
	}
	cdialog_sosk_focus_normalize( chtab );
}

static void cdialog_sosk_focus_down( struct cdialog_sosk_work_chtab *chtab )
{
	if( ! chtab->data->width ) return;
	
	if( chtab->count - chtab->pos >= chtab->data->width ){
		chtab->pos += chtab->data->width;
	} else{
		chtab->pos = chtab->pos % chtab->data->width;
	}
	cdialog_sosk_focus_normalize( chtab );
}
