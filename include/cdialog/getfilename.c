/*=========================================================

	getfilename.c

	任意のファイルパスの取得。

=========================================================*/
#include "getfilename.h"

/*=========================================================
	ローカル宣言
=========================================================*/
#define CDIALOG_GETFILENAME_WIDTH       30
#define CDIALOG_GETFILENAME_HEIGHT      21
#define CDIALOG_GETFILENAME_LIST_LINES  10
#define CDIALOG_GETFILENAME_INPUT_LINES  1

/*=========================================================
	ローカル関数
=========================================================*/
static void cdialog_getfilename_draw( struct cdialog_dev_base_params *base, CdialogGetfilenameData *data, struct cdialog_getfilename_work *work );
static void cdialog_getfilename_set_filename( const char *path, size_t length, struct cdialog_getfilename_work *work, const char *filename );
static void cdialog_getfilename_path_forward( char *path, size_t length, struct cdialog_getfilename_work *work, const char *dir );
static void cdialog_getfilename_path_back( char *path, struct cdialog_getfilename_work *work );
static bool cdialog_getfilename_init_help( CdialogDevHelp **help );
static bool cdialog_getfilename_init_message( const char *title, const char *message, bool yesno );
static bool cdialog_getfilename_init_keyboard( const char *title, char *text, size_t length, unsigned int kb );
static CdialogResult cdialog_getfilename_draw_dialog( enum cdialog_getfilename_show_dialog dialog );
static DirhFileType cdialog_getfilename_get_filetype( const char *path );
static inline void cdialog_getfilename_str_shrink_with_extender( char *str, size_t len );

/*=========================================================
	ローカル変数
=========================================================*/
static CdialogGetfilenameParams *st_params;

/*=========================================================
	関数
=========================================================*/
int cdialogGetfilenameInit( CdialogGetfilenameParams *params )
{
	if( st_params ) return CG_ERROR_ALREADY_INITIALIZED;
	
	if( params ){
		st_params = params;
		st_params->destroySelf = false;
	} else{
		st_params = (CdialogGetfilenameParams *)memoryExalloc( "GetfilenameParams", MEMORY_USER, 0, sizeof( CdialogGetfilenameParams ), PSP_SMEM_High, NULL );
		if( ! st_params ) return CG_ERROR_NOT_ENOUGH_MEMORY;
		st_params->destroySelf = true;
	}
	
	cdialogDevInitBaseParams( &(st_params->base) );
	
	/* 初期化 */
	st_params->data.title[0]    = '\0';
	st_params->data.options     = 0;
	st_params->data.initialDir  = NULL;
	st_params->data.initialName = NULL;
	st_params->data.path        = NULL;
	st_params->data.pathMax     = 0;
	
	return CG_ERROR_OK;
}

CdialogGetfilenameData *cdialogGetfilenameGetData( void )
{
	return &(st_params->data);
}

CdialogStatus cdialogGetfilenameGetStatus( void )
{
	if( ! st_params ) return CDIALOG_NONE;
	return st_params->base.status;
}

CdialogResult cdialogGetfilenameGetResult( void )
{
	return st_params->base.result;
}

int cdialogGetfilenameStart( unsigned int x, unsigned int y )
{
	int ret;
	
	if( ! cdialogDevLock() ) return CG_ERROR_ALREADY_RUNNING;
	ret = cdialogGetfilenameStartNoLock( x, y );
	
	if( ret < 0 ) cdialogDevUnlock();
	
	return ret;
}

int cdialogGetfilenameStartNoLock( unsigned int x, unsigned int y )
{
	int ret;
	
	st_params->base.status = CDIALOG_INIT;
	
	st_params->showDialog = CDIALOG_GETFILENAME_DIALOG_NONE;
	st_params->shutdown   = false;
	
	st_params->work.dirh       = 0;
	st_params->work.area       = CDIALOG_GETFILENAME_AREA_LIST;
	st_params->work.allEntries = 0;
	st_params->work.selectPos  = 0;
	st_params->work.filename   = NULL;
	
	st_params->base.x      = pbOffsetChar( x );
	st_params->base.y      = pbOffsetLine( y );
	st_params->base.width  = pbOffsetChar( CDIALOG_GETFILENAME_WIDTH + 2 );
	st_params->base.height = pbOffsetLine( CDIALOG_GETFILENAME_HEIGHT );
	
	ret = cdialogDevPrepareToStart( &(st_params->base), st_params->data.options );
	if( ret != CG_ERROR_OK ) return ret;
	cdialogDevSetRepeatButtons( &(st_params->base), PSP_CTRL_UP | PSP_CTRL_RIGHT | PSP_CTRL_DOWN | PSP_CTRL_LEFT );
	
	st_params->base.status = CDIALOG_VISIBLE;
	
	return 0;
}

int cdialogGetfilenameUpdate( void )
{
	SceCtrlData pad;
	
	if( st_params->shutdown ){
		char *delim = st_params->work.filename - 1;
		*delim = '/';
		st_params->base.result = CDIALOG_ACCEPT;
	}
	
	if( st_params->base.result != CDIALOG_UNKNOWN ){
		st_params->base.status = CDIALOG_SHUTDOWN;
		return 0;
	}
	
	/* ディレクトリエントリが空であれば再取得 */
	if( ! st_params->work.allEntries ){
		int ret;
		
		if( ! st_params->work.dirh ){
			st_params->work.dirh = dirhNew( st_params->data.pathMax, DIRH_O_ALLOC_HIGH );
			if( ! CG_IS_VALID_UID( st_params->work.dirh ) ) return CG_ERROR_NOT_ENOUGH_MEMORY;
			
			strutilCopy( st_params->data.path, st_params->data.initialDir ? st_params->data.initialDir : CDIALOG_GETFILENAME_DEFAULT_PATH, st_params->data.pathMax );
		}
		
		ret = dirhChdir( st_params->work.dirh, st_params->data.path, 3000000 );
		if( CG_IS_ERROR( ret ) ) return ret;
		
		if( ! st_params->work.filename ){
			st_params->work.filename    = st_params->data.path + strlen( st_params->data.path ) + 1;
			st_params->work.filename[0] = '\0';
			
			if( st_params->data.initialName ) cdialog_getfilename_set_filename( st_params->data.path, st_params->data.pathMax, &(st_params->work), st_params->data.initialName );
		}
		
		dirhSeek( st_params->work.dirh, DIRH_SEEK_END, 0 );
		st_params->work.allEntries = dirhTell( st_params->work.dirh );
		dirhSeek( st_params->work.dirh, DIRH_SEEK_SET, 0 );
		st_params->work.selectPos = 0;
		
		dirhSort( st_params->work.dirh, NULL );
	}
	
	cdialog_getfilename_draw( &(st_params->base), &(st_params->data), &(st_params->work) );
	cdialogDevReadCtrlBuffer( &(st_params->base), &pad, NULL );
	
	if( st_params->showDialog ){
		CdialogResult result = cdialog_getfilename_draw_dialog( st_params->showDialog );
		if( result != CDIALOG_UNKNOWN ){
			if( st_params->showDialog == CDIALOG_GETFILENAME_DIALOG_HELP ){
				memoryFree( st_params->base.help );
			} else if( st_params->showDialog == CDIALOG_GETFILENAME_DIALOG_CONFIRM ){
				st_params->shutdown = result == CDIALOG_ACCEPT ? true : false;
			}
			st_params->showDialog = CDIALOG_GETFILENAME_DIALOG_NONE;
		} else{
			return 0;
		}
	} else if( pad.Buttons & PSP_CTRL_START ){
		DirhFileType ftype;
		char *delim;
		
		if( strpbrk( st_params->work.filename, "\\/:*?\"<>|" ) ){
			if( cdialog_getfilename_init_message( CDIALOG_STR_GETFILENAME_ERROR_INVALID_FILENAME, CDIALOG_STR_GETFILENAME_ERROR_ILLEGAL_CHAR, false ) ){
				st_params->showDialog = CDIALOG_GETFILENAME_DIALOG_MESSAGE;
				return 0;
			}
		} else if( st_params->work.filename[0] == '\0' ){
			if( cdialog_getfilename_init_message( CDIALOG_STR_GETFILENAME_ERROR_INVALID_FILENAME, CDIALOG_STR_GETFILENAME_ERROR_EMPTY, false ) ){
				st_params->showDialog = CDIALOG_GETFILENAME_DIALOG_MESSAGE;
				return 0;
			}
		}
		
		delim = st_params->work.filename - 1;
		*delim = '/';
		ftype = cdialog_getfilename_get_filetype( st_params->data.path );
		*delim = '\0';
		
		if( ftype == DIRH_FILE ){
			if( st_params->data.options & CDIALOG_GETFILENAME_OVERWRITEPROMPT ){
				if( cdialog_getfilename_init_message( CDIALOG_STR_GETFILENAME_CONFIRM_OVERWRITE_LABEL, CDIALOG_STR_GETFILENAME_CONFIRM_OVERWRITE, true ) ){
					st_params->showDialog = CDIALOG_GETFILENAME_DIALOG_CONFIRM;
					return 0;
				}
			}
		} else if( ftype == DIRH_DIR ){
			if( cdialog_getfilename_init_message( CDIALOG_STR_GETFILENAME_ERROR_INVALID_FILENAME, CDIALOG_STR_GETFILENAME_ERROR_SAME_AS_DIR_NAME, false ) ){
				st_params->showDialog = CDIALOG_GETFILENAME_DIALOG_MESSAGE;
				return 0;
			}
		} else{
			if( st_params->data.options & CDIALOG_GETFILENAME_FILEMUSTEXIST ){
				if( cdialog_getfilename_init_message( CDIALOG_STR_GETFILENAME_ERROR_INVALID_FILENAME, CDIALOG_STR_GETFILENAME_ERROR_NOT_FOUND, false ) ){
					st_params->showDialog = CDIALOG_GETFILENAME_DIALOG_MESSAGE;
					return 0;
				}
			} else if( st_params->data.options & CDIALOG_GETFILENAME_CREATEPROMPT ){
				if( cdialog_getfilename_init_message( CDIALOG_STR_GETFILENAME_CONFIRM_CREATE_LABEL, CDIALOG_STR_GETFILENAME_CONFIRM_CREATE, true ) ){
					st_params->showDialog = CDIALOG_GETFILENAME_DIALOG_CONFIRM;
					return 0;
				}
			}
		}
		st_params->shutdown = true;
	} else if( pad.Buttons & cdialogDevCancelButton() ){
		st_params->data.path[0] = '\0';
		st_params->base.result = CDIALOG_CANCEL;
	} else if( pad.Buttons & PSP_CTRL_HOME ){
		if( cdialog_getfilename_init_help( &st_params->base.help ) ){
			st_params->showDialog = CDIALOG_GETFILENAME_DIALOG_HELP;
		}
	} else if( pad.Buttons & ( PSP_CTRL_LTRIGGER | PSP_CTRL_RTRIGGER ) ){
		st_params->work.area = st_params->work.area == CDIALOG_GETFILENAME_AREA_LIST ? CDIALOG_GETFILENAME_AREA_INPUT : CDIALOG_GETFILENAME_AREA_LIST;
	} else if( st_params->work.area == CDIALOG_GETFILENAME_AREA_LIST ){
		if( pad.Buttons & PSP_CTRL_UP ){
			st_params->work.selectPos--;
			if( st_params->work.selectPos < 0 ) st_params->work.selectPos = st_params->work.allEntries - 1;
		} else if( pad.Buttons & PSP_CTRL_DOWN ){
			st_params->work.selectPos++;
			if( st_params->work.selectPos >= st_params->work.allEntries ) st_params->work.selectPos = 0;
		} else if( pad.Buttons & PSP_CTRL_LEFT ){
			st_params->work.selectPos -= CDIALOG_GETFILENAME_LIST_LINES;
			if( st_params->work.selectPos < 0 ) st_params->work.selectPos = 0;
		} else if( pad.Buttons & PSP_CTRL_RIGHT ){
			st_params->work.selectPos += CDIALOG_GETFILENAME_LIST_LINES;
			if( st_params->work.selectPos >= st_params->work.allEntries ) st_params->work.selectPos = st_params->work.allEntries - 1;
		} else if( pad.Buttons & cdialogDevAcceptButton() ){
			DirhFileInfo *info;
			dirhSeek( st_params->work.dirh, DIRH_SEEK_SET, st_params->work.selectPos );
			info = dirhRead( st_params->work.dirh );
			
			if( info ){
				if( strcmp( info->name, ".." ) == 0 ){
					cdialog_getfilename_path_back( st_params->data.path, &(st_params->work) );
					st_params->work.allEntries = 0;
				} else if( info->type == DIRH_FILE ){
					cdialog_getfilename_set_filename( st_params->data.path, st_params->data.pathMax, &(st_params->work), info->name );
				} else if( info->type == DIRH_DIR ){
					cdialog_getfilename_path_forward( st_params->data.path, st_params->data.pathMax, &(st_params->work), info->name );
					st_params->work.allEntries = 0;
				}
			}
		} else if( pad.Buttons & PSP_CTRL_TRIANGLE ){
			cdialog_getfilename_path_back( st_params->data.path, &(st_params->work) );
			st_params->work.allEntries = 0;
		} else if( pad.Buttons & PSP_CTRL_SELECT ){
			if( cdialog_getfilename_init_message( CDIALOG_STR_GETFILENAME_EXTRA_MENU_LABEL, CDIALOG_STR_GETFILENAME_EXTRA_MENU, false ) ){
				st_params->showDialog = CDIALOG_GETFILENAME_DIALOG_MESSAGE;
			}
		}
	} else if( st_params->work.area == CDIALOG_GETFILENAME_AREA_INPUT ){
		if( pad.Buttons & cdialogDevAcceptButton() ){
			if( cdialog_getfilename_init_keyboard( CDIALOG_STR_GETFILENAME_INPUT, st_params->work.filename, st_params->data.path + st_params->data.pathMax - st_params->work.filename, CDIALOG_SOSK_INPUTTYPE_FILENAME ) ){
				st_params->showDialog = CDIALOG_GETFILENAME_DIALOG_KEYBOARD;
			}
		}
	}
	
	return 0;
}

int cdialogGetfilenameShutdownStartNoLock( void )
{
	if( st_params->base.status != CDIALOG_SHUTDOWN ){
		st_params->base.result = CDIALOG_CANCEL;
		st_params->base.status = CDIALOG_SHUTDOWN;
	}
	
	if( CG_IS_VALID_UID( st_params->work.dirh ) ) dirhDestroy( st_params->work.dirh );
	cdialogDevPrepareToFinish( &(st_params->base) );
	
	return 0;
}

int cdialogGetfilenameShutdownStart( void )
{
	int ret = cdialogGetfilenameShutdownStartNoLock();
	cdialogDevUnlock();
	return ret;
}

void cdialogGetfilenameDestroy( void )
{
	if( st_params->destroySelf ){
		memoryFree( st_params );
	}
	st_params = NULL;
}

static void cdialog_getfilename_draw( struct cdialog_dev_base_params *base, CdialogGetfilenameData *data, struct cdialog_getfilename_work *work )
{
	unsigned int line, line_offset;
	unsigned int buflen;
	DirhFileInfo *file;
	char *cwd;
	char buf[CDIALOG_GETFILENAME_WIDTH + 2];
	
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
	
	/* ファイルリストボーダー */
	pbLineRect(
		base->x + pbOffsetChar( 1 ),
		base->y + pbOffsetLine( 4 ),
		base->x + base->width - pbOffsetChar( 1 ),
		base->y + pbOffsetLine( 4 ) + pbOffsetLine( CDIALOG_GETFILENAME_LIST_LINES + 1 ),
		work->area == CDIALOG_GETFILENAME_AREA_LIST ? base->color->fcfg : base->color->border
	);
	
	/* ファイル名ボーダー */
	pbLineRect(
		base->x + pbOffsetChar( 1 ),
		base->y + base->height - pbOffsetLine( 4 ),
		base->x + base->width - pbOffsetChar( 1 ),
		base->y + base->height - pbOffsetLine( 4 ) + pbOffsetLine( CDIALOG_GETFILENAME_INPUT_LINES + 1 ),
		work->area == CDIALOG_GETFILENAME_AREA_INPUT ? base->color->fcfg : base->color->border
	);
	
	/* HELP */
	pbPrintf(
		base->x + base->width - pbOffsetChar( 11 ),
		base->y + base->height - ( pbOffsetLine( 1 ) + ( pbOffsetLine( 1 ) >> 1 ) ),
		base->color->help,
		PB_TRANSPARENT,
		"HOME: %s",
		CDIALOG_STR_HELP_LABEL
	);
	
	/* カレントディレクトリを取得し、文字数を揃える */
	cwd = dirhGetCwd( work->dirh );
	if( strchr( cwd, '/' ) ){
		buflen = strutilCopy( buf, strrchr( cwd, '/' ) + 1, sizeof( buf ) );
	} else{
		buflen = strutilCopy( buf, cwd, sizeof( buf ) );
	}
	if( buflen - 1 > CDIALOG_GETFILENAME_WIDTH - 9 ) cdialog_getfilename_str_shrink_with_extender( buf, CDIALOG_GETFILENAME_WIDTH - 9 );
	
	pbPrintf( 
		base->x + pbOffsetChar( 1 ),
		base->y + pbOffsetLine( 3 ),
		base->color->fg,
		PB_TRANSPARENT,
		"%s: %s",
		data->options & CDIALOG_GETFILENAME_SAVE ? CDIALOG_STR_GETFILENAME_SAVE_IN : CDIALOG_STR_GETFILENAME_LOOK_IN,
		buf
	);
	
	pbPrint(
		base->x + pbOffsetChar( 1 ),
		base->y + base->height - pbOffsetLine( 5 ),
		base->color->fg,
		PB_TRANSPARENT,
		CDIALOG_STR_GETFILENAME_FILENAME
	);
	
	/* ファイル名の長さを揃える */
	buflen = strutilCopy( buf, work->filename, sizeof( buf ) ) - 1;
	if( buflen > CDIALOG_GETFILENAME_WIDTH ) cdialog_getfilename_str_shrink_with_extender( buf, CDIALOG_GETFILENAME_WIDTH );
	
	pbPrint(
		base->x + pbOffsetChar( 1 ) + ( pbOffsetChar( 1 ) >> 1 ),
		base->y + base->height - ( pbOffsetLine( 3 ) + ( pbOffsetChar( 1 ) >> 1 ) ),
		base->color->fcfg,
		PB_TRANSPARENT,
		buf
	);
	
	if( work->allEntries < CDIALOG_GETFILENAME_LIST_LINES || work->selectPos <  ( CDIALOG_GETFILENAME_LIST_LINES >> 1 ) ){
		line = 0;
	} else if( work->selectPos > work->allEntries - ( CDIALOG_GETFILENAME_LIST_LINES >> 1 ) ){
		line = work->allEntries - CDIALOG_GETFILENAME_LIST_LINES;
	} else{
		line = work->selectPos - ( CDIALOG_GETFILENAME_LIST_LINES >> 1 );
	}
	
	if( line ){
		pbPutChar(
			base->x + base->width - ( pbOffsetChar( 2 ) + ( pbOffsetChar( 1 ) >> 1 ) ),
			base->y + ( pbOffsetLine( 1 ) >> 1 ) + pbOffsetLine( 4 ),
			base->color->fg,
			PB_TRANSPARENT,
			'\x80'
		);
	}
	if( line + CDIALOG_GETFILENAME_LIST_LINES < work->allEntries ){
		pbPutChar(
			base->x + base->width - ( pbOffsetChar( 2 ) + ( pbOffsetChar( 1 ) >> 1 ) ),
			base->y + ( pbOffsetLine( 1 ) >> 1 ) + pbOffsetLine( 3 + CDIALOG_GETFILENAME_LIST_LINES ),
			base->color->fg,
			PB_TRANSPARENT,
			'\x82'
		);
	}
	
	dirhSeek( work->dirh, DIRH_SEEK_SET, line );
	
	for( line_offset = 0; line_offset < CDIALOG_GETFILENAME_LIST_LINES; line++, line_offset++ ){
		if( ( file = dirhRead( work->dirh ) ) ){
			buflen = strutilCopy( buf, file->name, sizeof( buf ) ) - 1;
			if( buflen > CDIALOG_GETFILENAME_WIDTH ) cdialog_getfilename_str_shrink_with_extender( buf, CDIALOG_GETFILENAME_WIDTH );
			
			pbPrintf(
				base->x + ( pbOffsetChar( 1 ) >> 1 ) + pbOffsetChar( 1 ),
				base->y + ( pbOffsetLine( 1 ) >> 1 ) + pbOffsetLine( 4 + line_offset ),
				work->selectPos == line ? base->color->fcfg : file->type == DIRH_DIR ? base->color->extra : base->color->fg,
				PB_TRANSPARENT,
				file->type == DIRH_DIR ? "%s/" : "%s",
				buf
			);
		}
	}
}

static void cdialog_getfilename_set_filename( const char *path, size_t length, struct cdialog_getfilename_work *work, const char *filename )
{
	if( ( path + length ) - work->filename > strlen( filename ) ) strcpy( work->filename, filename );
}

static void cdialog_getfilename_path_forward( char *path, size_t length, struct cdialog_getfilename_work *work, const char *dir )
{
	int dirname_len = strlen( dir ) + 1;
	
	if( ( path + length ) - ( 1 + work->filename + strlen( work->filename ) + 1 ) > strlen( dir ) + 1 ){
		memmove( work->filename + dirname_len, work->filename, strlen( work->filename ) + 1 );
		work->filename += dirname_len;
		strcat( path, "/" );
		strcat( path, dir );
	}
}

static void cdialog_getfilename_path_back( char *path, struct cdialog_getfilename_work *work )
{
	char *endpoint = strrchr( path, '/' );
	
	if( ! endpoint ){
		return;
	} else{
		*endpoint = '\0';
		memmove( endpoint + 1, work->filename, strlen( work->filename ) + 1 );
		work->filename = endpoint + 1;
	}
}

static bool cdialog_getfilename_init_message( const char *title, const char *message, bool yesno )
{
	if( cdialogMessageInit( NULL ) == 0 ){
		CdialogMessageData *data = cdialogMessageGetData();
		strutilCopy( data->title,   title,   CDIALOG_MESSAGE_TITLE_LENGTH );
		strutilCopy( data->message, message, CDIALOG_MESSAGE_LENGTH );
		data->options = CDIALOG_DISPLAY_CENTER;
		if( yesno ) data->options |= CDIALOG_MESSAGE_YESNO;
		
		if( cdialogMessageStartNoLock( 0, 0 ) < 0 ){
			cdialogMessageShutdownStartNoLock();
		} else{
			return true;
		}
	}
	return false;
}

static bool cdialog_getfilename_init_help( CdialogDevHelp **help )
{
	if( ! help ) return false;
	
	*help = memoryAlloc( sizeof( CdialogDevHelp ) * 20 );
	if( *help && cdialogMessageInit( NULL ) == 0 ){
		CdialogMessageData *data = cdialogMessageGetData();
		data->options = CDIALOG_DISPLAY_CENTER;
		strutilCopy( data->title, CDIALOG_STR_HELP_LABEL, CDIALOG_MESSAGE_TITLE_LENGTH );
		
		data->width  = pbOffsetChar( CDIALOG_GETFILENAME_HELP_WIDTH );
		data->height = pbOffsetLine( 15 );
		data->callback = cdialogDevDrawHelp;
		cdialogDevSetHelp( *help, 20 );
		
		/* 共通操作 */
		cdialogDevHelp( &(*help)[0], 0,                 0,                 PB_SYM_PSP_LTRIGGER "/" PB_SYM_PSP_RTRIGGER );
		cdialogDevHelp( &(*help)[1], pbOffsetChar( 6 ), 0,                 CDIALOG_STR_GETFILENAME_HELP_COMMON_MOVEFOCUS );
		cdialogDevHelp( &(*help)[2], 0,                 pbOffsetLine( 1 ), PB_SYM_PSP_START );
		cdialogDevHelp( &(*help)[3], pbOffsetChar( 6 ), pbOffsetLine( 1 ), CDIALOG_STR_HELP_ACCEPT );
		cdialogDevHelp( &(*help)[4], 0,                 pbOffsetLine( 2 ), cdialogDevCancelSymbol() );
		cdialogDevHelp( &(*help)[5], pbOffsetChar( 6 ), pbOffsetLine( 2 ), CDIALOG_STR_HELP_CANCEL );
		
		/* ファイルリスト */
		cdialogDevHelp( &(*help)[6],  0,                 pbOffsetLine(  4 ), CDIALOG_STR_GETFILENAME_HELP_FILELIST_LABEL );
		cdialogDevHelp( &(*help)[7],  pbOffsetChar( 2 ), pbOffsetLine(  6 ), PB_SYM_PSP_UP PB_SYM_PSP_DOWN );
		cdialogDevHelp( &(*help)[8],  pbOffsetChar( 9 ), pbOffsetLine(  6 ), CDIALOG_STR_GETFILENAME_HELP_FILELIST_MOVE );
		cdialogDevHelp( &(*help)[9],  pbOffsetChar( 2 ), pbOffsetLine(  7 ), PB_SYM_PSP_LEFT PB_SYM_PSP_RIGHT );
		cdialogDevHelp( &(*help)[10], pbOffsetChar( 9 ), pbOffsetLine(  7 ), CDIALOG_STR_GETFILENAME_HELP_FILELIST_MOVEPAGE );
		cdialogDevHelp( &(*help)[11], pbOffsetChar( 2 ), pbOffsetLine(  8 ), cdialogDevAcceptSymbol() );
		cdialogDevHelp( &(*help)[12], pbOffsetChar( 9 ), pbOffsetLine(  8 ), CDIALOG_STR_GETFILENAME_HELP_FILELIST_ENTER );
		cdialogDevHelp( &(*help)[13], pbOffsetChar( 2 ), pbOffsetLine(  9 ), PB_SYM_PSP_TRIANGLE );
		cdialogDevHelp( &(*help)[14], pbOffsetChar( 9 ), pbOffsetLine(  9 ), CDIALOG_STR_GETFILENAME_HELP_FILELIST_PARENTDIR );
		cdialogDevHelp( &(*help)[15], pbOffsetChar( 2 ), pbOffsetLine( 10 ), PB_SYM_PSP_SELECT );
		cdialogDevHelp( &(*help)[16], pbOffsetChar( 9 ), pbOffsetLine( 10 ), CDIALOG_STR_GETFILENAME_HELP_FILELIST_EXTRAMENU );
		
		/* ファイル名 */
		cdialogDevHelp( &(*help)[17], 0,                 pbOffsetLine( 12 ), CDIALOG_STR_GETFILENAME_HELP_FILENAME_LABEL );
		cdialogDevHelp( &(*help)[18], pbOffsetChar( 2 ), pbOffsetLine( 14 ), cdialogDevAcceptSymbol() );
		cdialogDevHelp( &(*help)[19], pbOffsetChar( 4 ), pbOffsetLine( 14 ), CDIALOG_STR_GETFILENAME_HELP_FILENAME_INPUT );
		
		if( cdialogMessageStartNoLock( 0, 0 ) < 0 ){
			cdialogMessageShutdownStartNoLock();
		} else{
			return true;
		}
	}
	return false;
}

static bool cdialog_getfilename_init_keyboard( const char *title, char *text, size_t length, unsigned int kb )
{
	if( cdialogSoskInit( NULL ) == 0 ){
		CdialogSoskData *data = cdialogSoskGetData();
		if( title ) strutilCopy( data->title, title, CDIALOG_SOSK_TITLE_LENGTH );
		data->text    = text;
		data->textMax = length;
		data->types   = kb;
		data->options = CDIALOG_DISPLAY_CENTER;
		
		if( cdialogSoskStartNoLock( 0, 0 ) < 0 ){
			cdialogSoskShutdownStartNoLock();
		} else{
			return true;
		}
	}
	return false;
}

static CdialogResult cdialog_getfilename_draw_dialog( enum cdialog_getfilename_show_dialog dialog )
{
	CdialogStatus status;
	CdialogResult ret = CDIALOG_UNKNOWN;
	
	CdialogResult ( *result )( void ) = NULL;
	CdialogStatus ( *state  )( void ) = NULL;
	int ( *update )( void )           = NULL;
	int ( *shutdown )( void )         = NULL;
	void ( *destroy )( void )         = NULL;
	
	switch( dialog ){
		case CDIALOG_GETFILENAME_DIALOG_HELP:
		case CDIALOG_GETFILENAME_DIALOG_MESSAGE:
		case CDIALOG_GETFILENAME_DIALOG_CONFIRM:
			result   = cdialogMessageGetResult;
			state    = cdialogMessageGetStatus;
			update   = cdialogMessageUpdate;
			shutdown = cdialogMessageShutdownStartNoLock;
			destroy  = cdialogMessageDestroy;
			break;
		case CDIALOG_GETFILENAME_DIALOG_KEYBOARD:
			result   = cdialogSoskGetResult;
			state    = cdialogSoskGetStatus;
			update   = cdialogSoskUpdate;
			shutdown = cdialogSoskShutdownStartNoLock;
			destroy  = cdialogSoskDestroy;
			break;
		case CDIALOG_GETFILENAME_DIALOG_NONE:
			break;
	}
	
	update();
	status = state();
	if( status == CDIALOG_SHUTDOWN ){
		shutdown();
		ret = result();
		destroy();
		padctrlResetRepeat( st_params->base.paduid );
	}
	
	return ret;
}

static DirhFileType cdialog_getfilename_get_filetype( const char *path )
{
	SceIoStat stat;
	
	memset( &stat, 0, sizeof( SceIoStat ) );
	if( sceIoGetstat( path, &stat ) < 0 ){
		return DIRH_UNK;
	} else if( FIO_S_ISDIR( stat.st_mode ) ){
		return DIRH_DIR;
	} else if( FIO_S_ISREG( stat.st_mode ) ){
		return DIRH_FILE;
	} else{
		return DIRH_UNK;
	}
}

static inline void cdialog_getfilename_str_shrink_with_extender( char *str, size_t len )
{
	str[len - 3] = '.';
	str[len - 2] = '.';
	str[len - 1] = '.';
	str[len] = '\0';
}
