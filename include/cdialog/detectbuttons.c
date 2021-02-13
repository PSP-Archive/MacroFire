/*=========================================================

	detectbuttons.c

	ボタンの組み合わせ取得。

=========================================================*/
#include "detectbuttons.h"

/*=========================================================
	マクロ
=========================================================*/
#define CDIALOG_DETECTBUTTONS_SET_FLAGS( var, flags )   ( ( var ) |= ( flags ) )
#define CDIALOG_DETECTBUTTONS_UNSET_FLAGS( var, flags ) ( ( var ) &= ~( flags ) )

#define CDIALOG_DETECTBUTTONS_LIST_ROW 6

/*=========================================================
	ローカル型宣言
=========================================================*/
enum cdialog_detectbuttons_flags {
	CDIALOG_DETECTBUTTONS_DETECTING      = 0x00000001,
	CDIALOG_DETECTBUTTONS_WAITFORRELEASE = 0x00000002,
	CDIALOG_DETECTBUTTONS_CHOOSING       = 0x00000004
};

/*=========================================================
	ローカル関数
=========================================================*/
static void cdialog_detectbuttons_draw( struct cdialog_dev_base_params *base, CdialogDetectbuttonsData *data, struct cdialog_detectbuttons_work *work );

/*=========================================================
	ローカル変数
=========================================================*/
static CdialogDetectbuttonsParams *st_params;

/*=========================================================
	関数
=========================================================*/
int cdialogDetectbuttonsInit( CdialogDetectbuttonsParams *params )
{
	if( st_params ) return CG_ERROR_ALREADY_INITIALIZED;
	
	if( params ){
		st_params = params;
		st_params->destroySelf = false;
	} else{
		st_params = (CdialogDetectbuttonsParams *)memoryExalloc( "DetectbuttonsParams", MEMORY_USER, 0, sizeof( CdialogDetectbuttonsParams ) + sizeof( PadutilButtonName ** ) * PADUTIL_ARRAY_END, PSP_SMEM_High, NULL );
		if( ! st_params ) return CG_ERROR_NOT_ENOUGH_MEMORY;
		st_params->destroySelf = true;
	}
	
	cdialogDevInitBaseParams( &(st_params->base) );
	
	/* 初期化 */
	st_params->data.title[0]     = '\0';
	st_params->data.options      = 0;
	st_params->data.availButtons = 0;
	st_params->data.buttons      = NULL;
	
	return CG_ERROR_OK;
}

CdialogDetectbuttonsData *cdialogDetectbuttonsGetData( void )
{
	return &(st_params->data);
}

CdialogStatus cdialogDetectbuttonsGetStatus( void )
{
	return st_params->base.status;
}

CdialogResult cdialogDetectbuttonsGetResult( void )
{
	return st_params->base.result;
}

int cdialogDetectbuttonsStart( unsigned int x, unsigned int y )
{
	int ret;
	
	if( ! cdialogDevLock() ) return CG_ERROR_ALREADY_RUNNING;
	ret = cdialogDetectbuttonsStartNoLock( x, y );
	
	if( ret < 0 ) cdialogDevUnlock();
	
	return ret;
}

int cdialogDetectbuttonsStartNoLock( unsigned int x, unsigned int y )
{
	int ret;
	unsigned char i;
	
	st_params->base.status         = CDIALOG_INIT;
	st_params->work.flags          = 0;
	st_params->work.buttons        = *(st_params->data.buttons);
	st_params->work.buf[0]         = '\0';
	st_params->work.buttonNames    = padutilCreateButtonSymbols();
	if( ! st_params->work.buttonNames ) return CG_ERROR_NOT_ENOUGH_MEMORY;
	padutilGetButtonNamesByCode( st_params->work.buttonNames, st_params->work.buttons, " ", 0, st_params->work.buf, CDIALOG_DETECTBUTTONS_BUF_LENGTH );
	
	/* 利用可能なボタンリストの抽出 */
	st_params->work.availButtonNames = (PadutilButtonName **)( (uintptr_t)st_params + sizeof( CdialogDetectbuttonsParams ) );
	for( i = 0, st_params->work.availButtonCount = 0; i < PADUTIL_ARRAY_END; i++ ){
		if( st_params->data.availButtons & st_params->work.buttonNames[i].button ){
			st_params->work.availButtonNames[st_params->work.availButtonCount++] = &(st_params->work.buttonNames[i]);
		}
	}
	st_params->work.availButtonNames[st_params->work.availButtonCount] = &(st_params->work.buttonNames[PADUTIL_ARRAY_END]);
	
	st_params->base.x      = pbOffsetChar( x );
	st_params->base.y      = pbOffsetLine( y );
	st_params->base.width  = strlen( st_params->data.title );
	if( st_params->base.width < 35 ) st_params->base.width = 35;
	st_params->base.width  = pbOffsetChar( st_params->base.width + 2 );
	st_params->base.height = pbOffsetLine( 15 );
	
	ret = cdialogDevPrepareToStart( &(st_params->base), st_params->data.options );
	if( ret != CG_ERROR_OK ) return ret;
	cdialogDevSetRepeatButtons( &(st_params->base), PSP_CTRL_UP | PSP_CTRL_RIGHT | PSP_CTRL_DOWN | PSP_CTRL_LEFT );
	
	st_params->base.status = CDIALOG_VISIBLE;
	
	return 0;
}

int cdialogDetectbuttonsUpdate( void )
{
	SceCtrlData pad;
	PadutilButtons realbuttons;
	
	if( st_params->base.result != CDIALOG_UNKNOWN ){
		st_params->base.status = CDIALOG_SHUTDOWN;
		return 0;
	}
	
	cdialog_detectbuttons_draw( &(st_params->base), &(st_params->data), &(st_params->work) );
	realbuttons = cdialogDevReadCtrlBuffer( &(st_params->base), &pad, NULL ) & st_params->data.availButtons;
	
	if( st_params->work.flags & CDIALOG_DETECTBUTTONS_DETECTING ){
		PadutilButtons target_buttons = realbuttons;
		bool detect = false;
		
		if( ! target_buttons ){
			if( st_params->work.flags & CDIALOG_DETECTBUTTONS_WAITFORRELEASE ){
				CDIALOG_DETECTBUTTONS_UNSET_FLAGS( st_params->work.flags, CDIALOG_DETECTBUTTONS_WAITFORRELEASE );
			}
		} else if( ! ( st_params->work.flags & CDIALOG_DETECTBUTTONS_WAITFORRELEASE ) ){
			 if( target_buttons & st_params->work.buttons ){
			 	 CDIALOG_DETECTBUTTONS_UNSET_FLAGS( st_params->work.flags, CDIALOG_DETECTBUTTONS_DETECTING );
			} else{
				detect = true;
			}
		} else{
			detect = true;
		}
		
		/* 最初のボタン入力を無視する */
		if( detect && ( st_params->work.buttons || ! ( st_params->work.flags & CDIALOG_DETECTBUTTONS_WAITFORRELEASE ) ) && ( ( st_params->work.buttons | target_buttons ) > st_params->work.buttons ) ){
			CDIALOG_DETECTBUTTONS_SET_FLAGS( st_params->work.flags, CDIALOG_DETECTBUTTONS_WAITFORRELEASE );
			st_params->work.buttons |= target_buttons;
			padutilGetButtonNamesByCode( st_params->work.buttonNames, st_params->work.buttons, " ", 0, st_params->work.buf, CDIALOG_DETECTBUTTONS_BUF_LENGTH );
		}
	} else if( st_params->work.flags & CDIALOG_DETECTBUTTONS_CHOOSING ){
		if( pad.Buttons & PSP_CTRL_UP ){
			if( ! st_params->work.index || ! ( st_params->work.index % CDIALOG_DETECTBUTTONS_LIST_ROW ) ){
				st_params->work.index += CDIALOG_DETECTBUTTONS_LIST_ROW - 1;
				if( st_params->work.index > st_params->work.availButtonCount ) st_params->work.index = st_params->work.availButtonCount - 1;
			} else{
				st_params->work.index--;
			}
		} else if( pad.Buttons & PSP_CTRL_DOWN ){
			if( st_params->work.index  == st_params->work.availButtonCount - 1 || ! ( ( st_params->work.index + 1 ) % CDIALOG_DETECTBUTTONS_LIST_ROW ) ){
				st_params->work.index = ( (unsigned char)( st_params->work.index / CDIALOG_DETECTBUTTONS_LIST_ROW ) ) * CDIALOG_DETECTBUTTONS_LIST_ROW;
			} else{
				st_params->work.index++;
			}
		} else if( pad.Buttons & PSP_CTRL_RIGHT ){
			if( st_params->work.index + CDIALOG_DETECTBUTTONS_LIST_ROW > st_params->work.availButtonCount - 1 ){
				while( st_params->work.index >= CDIALOG_DETECTBUTTONS_LIST_ROW ) st_params->work.index -= CDIALOG_DETECTBUTTONS_LIST_ROW;
			} else{
				st_params->work.index += CDIALOG_DETECTBUTTONS_LIST_ROW;
			}
		} else if( pad.Buttons & PSP_CTRL_LEFT ){
			if( st_params->work.index < CDIALOG_DETECTBUTTONS_LIST_ROW ){
				while( st_params->work.index + CDIALOG_DETECTBUTTONS_LIST_ROW < st_params->work.availButtonCount ) st_params->work.index += CDIALOG_DETECTBUTTONS_LIST_ROW;
			} else{
				st_params->work.index -= CDIALOG_DETECTBUTTONS_LIST_ROW;
			}
		} else if( pad.Buttons & cdialogDevAcceptButton() ){
			st_params->work.buttons ^= (st_params->work.availButtonNames[st_params->work.index])->button;
			padutilGetButtonNamesByCode( st_params->work.buttonNames, st_params->work.buttons, " ", 0, st_params->work.buf, CDIALOG_DETECTBUTTONS_BUF_LENGTH );
		} else if( pad.Buttons & ( cdialogDevCancelButton() | PSP_CTRL_START ) ){
			CDIALOG_DETECTBUTTONS_UNSET_FLAGS( st_params->work.flags, CDIALOG_DETECTBUTTONS_CHOOSING );
		}
	} else{
		if( pad.Buttons & cdialogDevAcceptButton() ){
			CDIALOG_DETECTBUTTONS_SET_FLAGS( st_params->work.flags, CDIALOG_DETECTBUTTONS_DETECTING | CDIALOG_DETECTBUTTONS_WAITFORRELEASE );
			st_params->work.buttons = 0;
			st_params->work.buf[0]  = '\0';
		} else if( pad.Buttons & cdialogDevCancelButton() ){
			st_params->base.result = CDIALOG_CANCEL;
		} else if( pad.Buttons & PSP_CTRL_SQUARE ){
			st_params->work.buttons = 0;
			st_params->work.buf[0]  = '\0';
		} else if( pad.Buttons & PSP_CTRL_TRIANGLE ){
			st_params->work.index = 0;
			CDIALOG_DETECTBUTTONS_SET_FLAGS( st_params->work.flags, CDIALOG_DETECTBUTTONS_CHOOSING );
		} else if( pad.Buttons & PSP_CTRL_START ){
			*(st_params->data.buttons) = st_params->work.buttons;
			st_params->base.result = CDIALOG_ACCEPT;
		}
	}
	
	return 0;
}

int cdialogDetectbuttonsShutdownStartNoLock( void )
{
	if( st_params->base.status != CDIALOG_SHUTDOWN ){
		st_params->base.result = CDIALOG_CANCEL;
		st_params->base.status = CDIALOG_SHUTDOWN;
	}
	
	padutilDestroyButtonSymbols();
	cdialogDevPrepareToFinish( &(st_params->base) );
	
	return 0;
}

int cdialogDetectbuttonsShutdownStart( void )
{
	int ret = cdialogDetectbuttonsShutdownStartNoLock();
	cdialogDevUnlock();
	return ret;
}

void cdialogDetectbuttonsDestroy( void )
{
	if( st_params->destroySelf ){
		memoryFree( st_params );
	}
	st_params = NULL;
}

static void cdialog_detectbuttons_draw( struct cdialog_dev_base_params *base, CdialogDetectbuttonsData *data, struct cdialog_detectbuttons_work *work )
{
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
	
	pbLineRel(
		base->x + pbOffsetChar( 1 ),
		base->y + pbOffsetLine( 11 ),
		base->width - ( pbOffsetChar( 2 ) ),
		0,
		base->color->fg
	);
	
	pbPrint(
		base->x + pbOffsetChar( 1 ),
		base->y + pbOffsetLine( 12 ),
		base->color->fg,
		PB_TRANSPARENT,
		CDIALOG_STR_DETECTBUTTONS_CURRENT_BUTTONS
	);
	
	pbPrint(
		base->x + pbOffsetChar( 3 ),
		base->y + pbOffsetLine( 13 ),
		base->color->fg,
		base->color->fcbg,
		work->buf
	);
	
	if( work->flags & CDIALOG_DETECTBUTTONS_DETECTING ){
		pbPrint(
			base->x + pbOffsetChar( 1 ),
			base->y + pbOffsetLine( 3 ),
			base->color->fg,
			PB_TRANSPARENT,
			CDIALOG_STR_DETECTBUTTONS_DETECT_DESC
		);
		
		pbPrint(
			base->x + pbOffsetChar( 18 ),
			base->y + pbOffsetLine( 12 ),
			base->color->fcfg,
			PB_TRANSPARENT,
			CDIALOG_STR_DETECTBUTTONS_NOW_DETECTING
		);
	} else if( work->flags & CDIALOG_DETECTBUTTONS_CHOOSING ){
		unsigned char i, x, y;
		
		pbPrintf(
			base->x + pbOffsetChar( 18 ),
			base->y + pbOffsetLine( 12 ),
			base->color->fcfg,
			PB_TRANSPARENT,
			"%s/%s: %s",
			PB_SYM_PSP_START,
			cdialogDevCancelSymbol(),
			CDIALOG_STR_DETECTBUTTONS_CHOOSE_EXIT
		);
		
		for( i = 0, x = 0, y = 0; work->availButtonNames[i]->button; i++ ){
			if( y >= CDIALOG_DETECTBUTTONS_LIST_ROW ){
				y = 0;
				x += 7;
			}
			if( work->index == i ){
				pbFillRectRel( base->x + pbOffsetChar( 1 + x ), base->y + pbOffsetLine( 4 + y ), pbOffsetChar( 6 ), pbOffsetLine( 1 ), base->color->fcbg );
				pbPrint( base->x + pbOffsetChar( 1 + x ), base->y + pbOffsetLine( 4 + y++ ), base->color->fcfg, PB_TRANSPARENT, (work->availButtonNames[i])->name );
			} else{
				pbPrint( base->x + pbOffsetChar( 1 + x ), base->y + pbOffsetLine( 4 + y++ ), base->color->fg, PB_TRANSPARENT, (work->availButtonNames[i])->name );
			}
		}
	} else{
		pbPrint(
			base->x + pbOffsetChar( 1 ),
			base->y + pbOffsetLine( 3 ),
			base->color->fg,
			PB_TRANSPARENT,
			CDIALOG_STR_DETECTBUTTONS_DESC
		);
		
		pbPrintf(
			base->x + pbOffsetChar( 1 ),
			base->y + pbOffsetLine( 5 ),
			base->color->fg,
			PB_TRANSPARENT,
			"  %s  : %s\n"
			"  %s  : %s\n"
			"  %s  : %s\n"
			"  %s  : %s\n"
			"%s: %s",
			cdialogDevAcceptSymbol(), CDIALOG_STR_DETECTBUTTONS_START,
			PB_SYM_PSP_TRIANGLE,      CDIALOG_STR_DETECTBUTTONS_CHOOSE,
			PB_SYM_PSP_SQUARE,        CDIALOG_STR_DETECTBUTTONS_CLEAR,
			cdialogDevCancelSymbol(), CDIALOG_STR_DETECTBUTTONS_CANCEL,
			PB_SYM_PSP_START,         CDIALOG_STR_DETECTBUTTONS_ACCEPT
		);
	}
}
