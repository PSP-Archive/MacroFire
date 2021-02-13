/*=========================================================

	cdialog/numedit.c

	数値入力。

=========================================================*/
#include "numedit.h"

/*=========================================================
	ローカル宣言
=========================================================*/
enum cdialog_numedit_rotate_direction {
	CDIALOG_NUMEDIT_ROTATE_UP,
	CDIALOG_NUMEDIT_ROTATE_DOWN
};

/*=========================================================
	ローカル関数
=========================================================*/
void cdialog_numedit_draw( struct cdialog_dev_base_params *base, CdialogNumeditData *data, struct cdialog_numedit_work *work );
uint32_t cdialog_numedit_get_number( void *num, uint32_t max );
void cdialog_numedit_set_number( void *num, uint32_t max, uint32_t *value );
void cdialog_numedit_rotate( enum cdialog_numedit_rotate_direction rotate, struct cdialog_numedit_work *work, uint32_t max );

/*=========================================================
	ローカル変数
=========================================================*/
static CdialogNumeditParams *st_params;

/*=========================================================
	関数
=========================================================*/
int cdialogNumeditInit( CdialogNumeditParams *params )
{
	if( st_params ) return CG_ERROR_ALREADY_INITIALIZED;
	
	if( params ){
		st_params = params;
		st_params->destroySelf = false;
	} else{
		st_params = (CdialogNumeditParams *)memoryExalloc( "NumeditParams", MEMORY_USER, 0, sizeof( CdialogNumeditParams ), PSP_SMEM_High, NULL );
		if( ! st_params ) return CG_ERROR_NOT_ENOUGH_MEMORY;
		st_params->destroySelf = true;
	}
	
	cdialogDevInitBaseParams( &(st_params->base) );
	
	/* 初期化 */
	st_params->data.title[0] = '\0';
	st_params->data.options  = 0;
	st_params->data.unit[0]  = '\0';
	st_params->data.max      = 0;
	
	return CG_ERROR_OK;
}

CdialogNumeditData *cdialogNumeditGetData( void )
{
	return &(st_params->data);
}

CdialogStatus cdialogNumeditGetStatus( void )
{
	if( ! st_params ) return CDIALOG_NONE;
	return st_params->base.status;
}

CdialogResult cdialogNumeditGetResult( void )
{
	return st_params->base.result;
}

int cdialogNumeditStart( unsigned int x, unsigned int y )
{
	int ret;
	
	if( ! cdialogDevLock() ) return CG_ERROR_ALREADY_RUNNING;
	ret = cdialogNumeditStartNoLock( x, y );
	
	if( ret < 0 ) cdialogDevUnlock();
	
	return ret;
}

int cdialogNumeditStartNoLock( unsigned int x, unsigned int y )
{
	int ret;
	unsigned int width;
	
	st_params->base.status = CDIALOG_INIT;
	
	st_params->showMessage = false;
	
	st_params->work.maxdigits = 0;
	st_params->work.pos       = 0;
	st_params->work.numbers   = NULL;
	st_params->work.buf[0]    = '\0';
	
	snprintf( st_params->work.buf, CDIALOG_NUMEDIT_MAX_NUMBER_OF_DIGITS + 1, "%010u", cdialog_numedit_get_number( st_params->data.num, st_params->data.max ) );
	st_params->work.maxdigits = log10( st_params->data.max ) + 1;
	st_params->work.pos       = st_params->work.maxdigits - 1;
	if( st_params->work.maxdigits > CDIALOG_NUMEDIT_MAX_NUMBER_OF_DIGITS ) st_params->work.maxdigits = CDIALOG_NUMEDIT_MAX_NUMBER_OF_DIGITS;
	st_params->work.numbers = st_params->work.buf + ( CDIALOG_NUMEDIT_MAX_NUMBER_OF_DIGITS - st_params->work.maxdigits );
	
	width = st_params->work.maxdigits + strlen( st_params->data.unit ) + 1;
	
	st_params->base.x      = pbOffsetChar( x );
	st_params->base.y      = pbOffsetLine( y );
	st_params->base.width  = strlen( st_params->data.title );
	if( st_params->base.width < width ) st_params->base.width = width;
	if( st_params->base.width < 10    ) st_params->base.width = 10;
	st_params->base.width  = pbOffsetChar( st_params->base.width + 2 );
	st_params->base.height = pbOffsetLine( 7 );
	
	ret = cdialogDevPrepareToStart( &(st_params->base), st_params->data.options );
	if( ret != CG_ERROR_OK ) return ret;
	cdialogDevSetRepeatButtons( &(st_params->base), PSP_CTRL_UP | PSP_CTRL_RIGHT | PSP_CTRL_DOWN | PSP_CTRL_LEFT );
	
	st_params->base.status = CDIALOG_VISIBLE;
	
	return 0;
}

int cdialogNumeditUpdate( void )
{
	SceCtrlData pad;
	
	if( st_params->base.result != CDIALOG_UNKNOWN ){
		st_params->base.status = CDIALOG_SHUTDOWN;
		return 0;
	}
	
	cdialog_numedit_draw( &(st_params->base), &(st_params->data), &(st_params->work) );
	
	if( st_params->showMessage ){
		CdialogStatus status;
		
		cdialogMessageUpdate();
		status = cdialogMessageGetStatus();
		if( status == CDIALOG_SHUTDOWN ){
			cdialogMessageShutdownStartNoLock();
			cdialogMessageDestroy();
			st_params->showMessage = false;
			padctrlResetRepeat( st_params->base.paduid );
			memoryFree( st_params->base.help );
		}
		return 0;
	}
	
	cdialogDevReadCtrlBuffer( &(st_params->base), &pad, NULL );
	
	if( pad.Buttons & ( cdialogDevAcceptButton() | PSP_CTRL_START ) ){
		uint32_t value = strtoul( st_params->work.numbers, NULL, 10 );
		cdialog_numedit_set_number( st_params->data.num, st_params->data.max, &value );
		st_params->base.result = CDIALOG_ACCEPT;
	} else if( pad.Buttons & cdialogDevCancelButton() ){
		st_params->base.result = CDIALOG_CANCEL;
	} else if( pad.Buttons & PSP_CTRL_RIGHT ){
		if( st_params->work.pos == st_params->work.maxdigits - 1 ){
			st_params->work.pos = 0;
		} else{
			st_params->work.pos++;
		}
	} else if( pad.Buttons & PSP_CTRL_LEFT ){
		if( st_params->work.pos == 0 ){
			st_params->work.pos = st_params->work.maxdigits - 1;
		} else{
			st_params->work.pos--;
		}
	} else if( pad.Buttons & PSP_CTRL_UP ){
		cdialog_numedit_rotate( CDIALOG_NUMEDIT_ROTATE_UP, &(st_params->work), st_params->data.max );
	} else if( pad.Buttons & PSP_CTRL_DOWN ){
		cdialog_numedit_rotate( CDIALOG_NUMEDIT_ROTATE_DOWN, &(st_params->work), st_params->data.max );
	} else if( pad.Buttons & PSP_CTRL_HOME ){
		st_params->base.help = memoryAlloc( sizeof( CdialogDevHelp ) * 8 );
		if( st_params->base.help && cdialogMessageInit( NULL ) == 0 ){
			CdialogMessageData *data = cdialogMessageGetData();
			data->options = CDIALOG_DISPLAY_CENTER;
			strutilCopy( data->title, CDIALOG_STR_HELP_LABEL, CDIALOG_MESSAGE_TITLE_LENGTH );
			data->width  = pbOffsetChar( CDIALOG_NUMEDIT_HELP_WIDTH );
			data->height = pbOffsetLine(  5 );
			data->callback = cdialogDevDrawHelp;
			cdialogDevSetHelp( st_params->base.help, 8 );
			
			cdialogDevHelp( &st_params->base.help[0], 0,                 0,                 PB_SYM_PSP_LEFT PB_SYM_PSP_RIGHT );
			cdialogDevHelp( &st_params->base.help[1], pbOffsetChar( 4 ), 0,                 CDIALOG_STR_NUMEDIT_HELP_MOVE );
			cdialogDevHelp( &st_params->base.help[2], 0,                 pbOffsetLine( 1 ), PB_SYM_PSP_UP PB_SYM_PSP_DOWN );
			cdialogDevHelp( &st_params->base.help[3], pbOffsetChar( 4 ), pbOffsetLine( 1 ), CDIALOG_STR_NUMEDIT_HELP_CHANGEVALUE );
			cdialogDevHelp( &st_params->base.help[4], 0,                 pbOffsetLine( 3 ), PB_SYM_PSP_START );
			cdialogDevHelp( &st_params->base.help[5], pbOffsetChar( 6 ), pbOffsetLine( 3 ), CDIALOG_STR_HELP_ACCEPT );
			cdialogDevHelp( &st_params->base.help[6], 0,                 pbOffsetLine( 4 ), cdialogDevCancelSymbol() );
			cdialogDevHelp( &st_params->base.help[7], pbOffsetChar( 6 ), pbOffsetLine( 4 ), CDIALOG_STR_HELP_CANCEL );
			
			if( cdialogMessageStartNoLock( 0, 0 ) < 0 ){
				cdialogMessageShutdownStartNoLock();
			} else{
				st_params->showMessage = true;
			}
		}
	}
	
	return 0;
}

int cdialogNumeditShutdownStartNoLock( void )
{
	if( st_params->base.status != CDIALOG_SHUTDOWN ){
		st_params->base.result = CDIALOG_CANCEL;
		st_params->base.status = CDIALOG_SHUTDOWN;
	}
	
	cdialogDevPrepareToFinish( &(st_params->base) );
	
	return 0;
}

int cdialogNumeditShutdownStart( void )
{
	int ret = cdialogNumeditShutdownStartNoLock();
	cdialogDevUnlock();
	return ret;
}

void cdialogNumeditDestroy( void )
{
	if( st_params->destroySelf ){
		memoryFree( st_params );
	}
	st_params = NULL;
}

void cdialog_numedit_draw( struct cdialog_dev_base_params *base, CdialogNumeditData *data, struct cdialog_numedit_work *work )
{
	unsigned int i;
	unsigned int avail_offset;
	unsigned int display_x_start = base->width - pbOffsetChar( work->maxdigits + strlen( data->unit ) + 2 );
	
	{
		char *digits = strutilCounterPbrk( work->numbers, "0" );
		if( ! digits ){
			avail_offset = work->maxdigits;
		} else{
			avail_offset = (unsigned int)digits - (unsigned int)work->numbers;
		}
		if( avail_offset > work->pos ) avail_offset = work->pos;
	}
	
	/* 枠を描画 */
	pbFillRectRel( base->x, base->y, base->width, base->height, base->color->bg );
	pbLineRectRel( base->x, base->y, base->width, base->height, base->color->border );
	
	/* タイトル描画 */
	pbPrint(
		base->x + ( base->width >> 1 ) -  ( pbMeasureString( data->title ) >> 1 ),
		base->y + pbOffsetLine( 1 ),
		base->color->title,
		PB_TRANSPARENT,
		data->title
	);
	
	pbPrintf(
		base->x + pbOffsetChar( 1 ),
		base->y + pbOffsetLine( 5 ),
		base->color->help,
		PB_TRANSPARENT,
		"HOME: %s",
		CDIALOG_STR_HELP_LABEL
	);
	
	for( i = 0; i < work->maxdigits; i++ ){
		if( work->pos == i ){
			char num_current[16];
			snprintf( num_current, sizeof( num_current ), "%s\n%c\n%s",PB_SYM_PSP_UP, work->numbers[i], PB_SYM_PSP_DOWN );
			pbPrint(
				base->x + display_x_start + pbOffsetChar( i ),
				base->y + pbOffsetLine( 2 ),
				base->color->fcfg,
				PB_TRANSPARENT,
				num_current
			);
		} else if( i >= avail_offset ){
			pbPutChar(
				base->x + display_x_start + pbOffsetChar( i ),
				base->y + pbOffsetLine( 3 ),
				base->color->fg,
				PB_TRANSPARENT,
				work->numbers[i]
			);
		} else{
			pbPutChar(
				base->x + display_x_start + pbOffsetChar( i ),
				base->y + pbOffsetLine( 3 ),
				( base->color->fg & 0x00ffffff ) | 0x55000000,
				PB_TRANSPARENT,
				'0'
			);
		}
	}
	
	/* 単位表示 */
	pbPrintf(
		base->x + display_x_start + pbOffsetChar( i + 1 ),
		base->y + pbOffsetLine( 3 ),
		base->color->fg,
		PB_TRANSPARENT,
		"%s",
		data->unit
	);
}

uint32_t cdialog_numedit_get_number( void *num, uint32_t max )
{
	if( max <= UINT8_MAX ){
		return *((uint8_t *)num);
	} else if( max <= UINT16_MAX ){
		return *((uint16_t *)num);
	} else{
		return *((uint32_t *)num);
	}
}

void cdialog_numedit_set_number( void *num, uint32_t max, uint32_t *value )
{
	if( *value > max ) *value = max;
	
	if( max <= UINT8_MAX ){
		*((uint8_t *)num)  = *value;
	} else if( max <= UINT16_MAX ){
		*((uint16_t *)num) = *value;
	} else{
		*((uint32_t *)num) = *value;
	}
}

void cdialog_numedit_rotate( enum cdialog_numedit_rotate_direction rotate, struct cdialog_numedit_work *work, uint32_t max )
{
	char max_str[CDIALOG_NUMEDIT_MAX_NUMBER_OF_DIGITS + 1];
	unsigned int offset = CDIALOG_NUMEDIT_MAX_NUMBER_OF_DIGITS - work->maxdigits;
	
	snprintf( max_str, sizeof( max_str ), "%010u", max );
	
	if( rotate == CDIALOG_NUMEDIT_ROTATE_UP ){
		if( work->numbers[work->pos] >= '9' ){
			work->numbers[work->pos] = '0';
		} else{
			work->numbers[work->pos]++;
		}
	} else if( rotate == CDIALOG_NUMEDIT_ROTATE_DOWN ){
		if( work->numbers[work->pos] <= '0' ){
			work->numbers[work->pos] = '9';
		} else{
			work->numbers[work->pos]--;
		}
	}
	
	if( work->numbers[0] >= max_str[offset] ){
		unsigned int i;
		for( i = 0; i <= work->maxdigits; i++ ){
			if(
				( i == 0 && work->numbers[i] > max_str[offset + i] ) ||
				( work->numbers[i - 1] >= max_str[offset + i - 1] && work->numbers[i] > max_str[offset + i] )
			){
				if( work->pos == i ){
					if( rotate == CDIALOG_NUMEDIT_ROTATE_UP ){
						work->numbers[i] = '0';
					} else if( rotate == CDIALOG_NUMEDIT_ROTATE_DOWN ){
						work->numbers[i] = max_str[offset + i];
					}
				} else{
					work->numbers[i] = max_str[offset + i];
				}
			}
		}
	}
}
