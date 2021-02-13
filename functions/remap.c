/*=========================================================
	
	function/remap.c
	
	ボタン再割り当て。
	
=========================================================*/
#include "remap.h"

/*=========================================================
	ローカル型宣言
=========================================================*/
struct remap_params {
	DmemUID dmem;
	PadutilRemap *remapList;
	PadutilRemap *selected;
};

struct remap_inisave_arg {
	struct remap_params *params;
	unsigned int index;
};

/*=========================================================
	ローカル関数
=========================================================*/
static bool remap_add( struct remap_params *params );
static bool remap_delete( struct remap_params *params );
static bool remap_clear( struct remap_params *params );

static int remap_control_load( MfMessage message, const char *label, void *var, void *pref, void *ex );
static int remap_control_save( MfMessage message, const char *label, void *var, void *pref, void *ex );
static int remap_menu_clear( MfMessage message );

static int remap_ini_handle( MfCtrlDefIniAction action, const char *path );
static int remap_save( struct remap_params *params, const char *path );
static int remap_load( struct remap_params *params, const char *path );

static int remap_inicb_load( InimgrContext ctx, void *arg );
static int remap_inicb_save( InimgrContext ctx, void *arg );

/*=========================================================
	ローカル変数
=========================================================*/
static struct remap_params st_params;

/*=========================================================
	関数
=========================================================*/
void *remapProc( MfMessage message )
{
	switch( message ){
		case MF_MS_INIT:
			break;
		case MF_MS_INI_LOAD:
			return remapIniLoad;
		case MF_MS_INI_CREATE:
			return remapIniCreate;
		case MF_MS_TERM:
			break;
		case MF_MS_HOOK:
			return remapMain;
		case MF_MS_MENU:
			return remapMenu;
		default:
			break;
	}
	return NULL;
}

void remapIniLoad( InimgrUID ini, char *buf, size_t len )
{
	if( CG_IS_OK( mfIniGetString( ini, "Remap", buf, len ) ) ){
		remap_load( &st_params, (const char *)buf );
	}
}

void remapIniCreate( InimgrUID ini, char *buf, size_t len )
{
	inimgrSetString( ini, MF_INI_SECTION_DEFAULT, "Remap", "" );
}

void remapMain( MfHookAction action, SceCtrlData *pad, MfHprmKey *hk )
{
	if( st_params.remapList ){
		padutilRemap( st_params.remapList, padutilSetPad( pad->Buttons | padutilGetAnalogStickDirection( pad->Lx, pad->Ly, 0 ) ) | padutilSetHprm( *hk ), pad, hk, false );
	}
}

int remapMenu( MfMenuMessage message )
{
	static MfMenuTable *menu;
	static MfCtrlDefIniPref *inipref;
	static char column;
	
	switch( mfMenuMaskMainMessage( message ) ){
		case MF_MM_INIT:
			dbgprint( "Creating remap menu" );
			menu = mfMenuCreateTables(
				1,
				4, 1
			);
			if( menu ){
				inipref = mfMenuMemoryCalloc( sizeof( MfCtrlDefIniPref ) );
				if( ! inipref || ! mfMenuRegisterTempMemory( remapMenu, 2, menu, inipref ) ){
					mfMenuDestroyTables( menu );
					if( inipref ) mfMenuMemoryFree( inipref );
					return CG_ERROR_NOT_ENOUGH_MEMORY;
				}
				
				inipref->title        = NULL;
				inipref->initDir      = "ms0:";
				inipref->initFilename = NULL;
				inipref->pathMax      = MF_PATH_MAX;
				
				mfMenuSetTablePosition( menu, 1, pbOffsetChar( 57 ), pbOffsetLine( 7 ) );
				mfMenuSetTableEntry( menu, 1, 1, 1, MF_STR_LOAD, remap_control_load, remap_ini_handle, inipref );
				mfMenuSetTableEntry( menu, 1, 2, 1, MF_STR_SAVE, remap_control_save, remap_ini_handle, inipref );
				mfMenuSetTableEntry( menu, 1, 4, 1, MF_STR_REMAP_CLEAR, mfCtrlDefExtra, remap_menu_clear, NULL );
				mfMenuInitTable( menu );
				break;
			}
			return CG_ERROR_NOT_ENOUGH_MEMORY;
		case MF_MM_TERM:
			break;
		default:
		{
			uint8_t viewlines = 0;
			unsigned int buttons = mfMenuGetCurrentButtons();
			
			pbPrint( pbOffsetChar( 3 ), pbOffsetLine(  4 ), MF_COLOR_TEXT_FG, MF_COLOR_TEXT_BG, MF_STR_REMAP_DESC );
			
			if( ! st_params.remapList ){
				mfMenuDeactivateTableEntry( menu, 1, 2, 1 );
				pbPrintf( pbOffsetChar( 3 ), pbOffsetLine( 7 ), MF_COLOR_TEXT_FC, MF_COLOR_TEXT_BG, MF_STR_REMAP_SUGGEST, PB_SYM_PSP_TRIANGLE );
			} else{
				PadutilRemap *entry;
				char         real_buttons[64];
				char         remap_buttons[64];
				unsigned int pos, maxpos, line, skip;
				uint8_t      linepos;
				
				mfMenuActivateTableEntry( menu, 1, 2, 1 );
				
				/* リマップされている個数と選択中のエントリの位置を数える */
				for( entry = st_params.remapList, pos = 0, maxpos = 0; entry; entry = entry->next, maxpos++ ){
					if( entry == st_params.selected ) pos = maxpos;
				}
				maxpos--;
				
				/* スクロール */
				skip = mfMenuScroll( pos, REMAP_LINES_PER_PAGE, maxpos );
				for( entry = st_params.remapList, line = 0; skip; entry = entry->next, line++, skip-- );
				
				/* 表示 */
				for( linepos = 0, viewlines = REMAP_LINES_PER_PAGE; viewlines && entry; entry = entry->next, line++, viewlines--, linepos++ ){
					if( ! entry->realButtons ) continue;
					
					real_buttons[0]  = '\0';
					
					mfMenuButtonsSymbolList( entry->realButtons,  real_buttons,  sizeof( real_buttons ) );
					if( pos == line ){
						pbPrint( pbOffsetChar( 3 ), pbOffsetLine( 7 + linepos ), MF_COLOR_TEXT_FC, MF_COLOR_TEXT_BG, real_buttons );
					} else{
						pbPrint( pbOffsetChar( 3 ), pbOffsetLine( 7 + linepos ), MF_COLOR_TEXT_FG, MF_COLOR_TEXT_BG, real_buttons );
					}
				}
							
				remap_buttons[0] = '\0';
				mfMenuButtonsSymbolList( st_params.selected->remapButtons, remap_buttons, sizeof( remap_buttons ) );
				pbLineRectRel( pbOffsetChar( 15 ), pbOffsetLine( 6 + REMAP_LINES_PER_PAGE + 3 ), pbOffsetChar( 64 ), pbOffsetLine( 3 ), MF_COLOR_TEXT_FG );
				pbPrint( pbOffsetChar( 2 ), pbOffsetLine( 6 + REMAP_LINES_PER_PAGE + 4 ), MF_COLOR_TEXT_FG, MF_COLOR_TEXT_BG, MF_STR_REMAP_BUTTON );
				pbPrint( pbOffsetChar( 16 ), pbOffsetLine( 6 + REMAP_LINES_PER_PAGE + 4 ), MF_COLOR_TEXT_FC, MF_COLOR_TEXT_BG, remap_buttons[0] != '\0' ? remap_buttons : MF_STR_REMAP_BUTTON_DISABLE );
			}
			
			if( column == REMAP_COLUMN_LIST ){
				pbLineRectRel( pbOffsetChar(  2 ), pbOffsetLine( 6 ), pbOffsetChar( 53 ), pbOffsetLine( REMAP_LINES_PER_PAGE + 2 ), MF_COLOR_TEXT_FC );
				pbLineRectRel( pbOffsetChar( 56 ), pbOffsetLine( 6 ), pbOffsetChar( 23 ), pbOffsetLine( REMAP_LINES_PER_PAGE + 2 ), MF_COLOR_TEXT_FG );
				
				mfMenuDrawTable( menu, MF_MENU_DISPLAY_ONLY );
				
				if( message & MF_DM_FINISH ){
					if( ! mfDialogLastResult() && ! st_params.selected->realButtons ){
						remap_delete( &st_params );
						mfMenuResetKeyRepeat();
					}
				} else if( ! mfMenuMaskDialogMessage( message ) ){
					if( ! st_params.remapList ){
						mfMenuSetInfoText( MF_MENU_INFOTEXT_COMMON_CTRL | MF_MENU_INFOTEXT_MOVABLEPAGE_CTRL | MF_MENU_INFOTEXT_SET_LOWER_LINE, "(L/R)%s, (%s)%s", MF_STR_REMAP_CTRL_SWITCH_COLUMN, PB_SYM_PSP_TRIANGLE, MF_STR_REMAP_CTRL_ADD );
					} else{
						mfMenuSetInfoText( MF_MENU_INFOTEXT_COMMON_CTRL | MF_MENU_INFOTEXT_MOVABLEPAGE_CTRL | MF_MENU_INFOTEXT_SET_LOWER_LINE, "(L/R)%s, (%s)%s, (%s)%s, (%s)%s", MF_STR_REMAP_CTRL_SWITCH_COLUMN, PB_SYM_PSP_TRIANGLE, MF_STR_REMAP_CTRL_ADD, PB_SYM_PSP_SQUARE, MF_STR_REMAP_CTRL_DELETE, mfMenuAcceptSymbol(), MF_STR_REMAP_CTRL_EDIT_REMAP );
					}
					if( buttons ){
						if( buttons & mfMenuCancelButton() ){
							return MF_MENU_BACK;
						} else if( ( buttons & PSP_CTRL_TRIANGLE ) && remap_add( &st_params ) ){
							mfDialogDetectbuttons( MF_STR_REMAP_DIAGBUTTON_REAL, MF_HOTKEY_BUTTONS, &(st_params.selected->realButtons) );
						} else if( buttons & ( PSP_CTRL_LTRIGGER | PSP_CTRL_RTRIGGER ) ){
							column = REMAP_COLUMN_MENU;
						} else if( st_params.selected ){
							if( buttons & mfMenuAcceptButton() ){
								mfDialogDetectbuttons( MF_STR_REMAP_DIAGBUTTON_REMAP, MF_TARGET_BUTTONS, &(st_params.selected->remapButtons) );
							} else if( buttons & PSP_CTRL_SQUARE ){
								remap_delete( &st_params );
							} else if( buttons & PSP_CTRL_UP ){
								if( st_params.selected->prev ){
									st_params.selected = st_params.selected->prev;
								} else{
									for( ; st_params.selected->next; st_params.selected = st_params.selected->next );
								}
							} else if( buttons & PSP_CTRL_DOWN ){
								if( st_params.selected->next ){
									st_params.selected = st_params.selected->next;
								} else{
									st_params.selected = st_params.remapList;
								}
							} else if( buttons & PSP_CTRL_RIGHT ){
								for( viewlines = REMAP_LINES_PER_PAGE; viewlines && st_params.selected->next; viewlines--, st_params.selected = st_params.selected->next );
							} else if( buttons & PSP_CTRL_LEFT ){
								for( viewlines = REMAP_LINES_PER_PAGE; viewlines && st_params.selected->prev; viewlines--, st_params.selected = st_params.selected->prev );
							}
						}
					}
				}
			} else{
				pbLineRectRel( pbOffsetChar(  2 ), pbOffsetLine( 6 ), pbOffsetChar( 53 ), pbOffsetLine( REMAP_LINES_PER_PAGE + 2 ), MF_COLOR_TEXT_FG );
				pbLineRectRel( pbOffsetChar( 56 ), pbOffsetLine( 6 ), pbOffsetChar( 23 ), pbOffsetLine( REMAP_LINES_PER_PAGE + 2 ), MF_COLOR_TEXT_FC );
				
				if( message & MF_MM_PROC ){
					if( buttons & ( PSP_CTRL_LTRIGGER | PSP_CTRL_RTRIGGER ) ){
						column = REMAP_COLUMN_LIST;
					}
				}
				return mfMenuDrawTable( menu, MF_MENU_NO_OPTIONS );
			}
			break;
		}
	}
	return MF_MENU_OK;
}

static bool remap_add( struct remap_params *params )
{
	PadutilRemap *new_entry;
	PadutilRemap *last = NULL;
	
	if( ! params->remapList ){
		params->dmem = dmemNew( 512, MEMORY_USER, PSP_SMEM_Low );
		if( ! CG_IS_VALID_UID( params->dmem ) ) return false;
		
		params->remapList = dmemAlloc( params->dmem, sizeof( PadutilRemap ) );
		if( ! params->remapList ){
			dmemDestroy( params->dmem );
			return false;
		}
		new_entry = params->remapList;
	} else{
		/* 末尾まで移動 */
		for( last = params->remapList; last->next; last = last->next );
		
		last->next = dmemAlloc( params->dmem, sizeof( PadutilRemap ) );
		new_entry = last->next;
	}
	
	if( new_entry ){
		new_entry->realButtons  = 0;
		new_entry->remapButtons = 0;
		new_entry->prev         = last;
		new_entry->next         = NULL;
		
		params->selected = new_entry;
		
		return true;
	} else{
		return false;
	}
}

static bool remap_delete( struct remap_params *params )
{
	if( params->selected ){
		PadutilRemap *target = params->selected;
		if( target->next ){
			target->next->prev = target->prev;
			params->selected = target->next;
		} else{
			params->selected = target->prev;
		}
		if( target->prev ) target->prev->next = target->next;
		
		if( target == params->remapList ){
			params->remapList = params->selected;
			if( ! params->remapList ){
				dmemDestroy( params->dmem );
				params->selected = NULL;
			} else{
				dmemFree( params->dmem, target );
			}
		}
	}
	return true;
}

static bool remap_clear( struct remap_params *params )
{
	if( params->selected ){
		dmemDestroy( params->dmem );
		params->remapList = NULL;
		params->selected  = NULL;
	}
	return true;
}

static int remap_save( struct remap_params *params, const char *path )
{
	InimgrUID ini = inimgrNew();
	
	if( ini ){
		int ret;
		struct remap_inisave_arg arg = { params, 1 };
		
		/* 起点となるセクションを作成 */
		inimgrAddSection( ini, REMAP_SECTION_DATA"1" );
		
		if( params->remapList ){
			PadutilRemap *backup_selected = params->selected;
			params->selected = params->remapList;
			
			/* コールバック登録 */
			inimgrRegisterCallback( ini, REMAP_SECTION_DATA"*", remap_inicb_save, (void *)&arg );
			
			/* セーブ開始 */
			ret = inimgrSave( ini, path, REMAP_INI_SIGNATURE, inimgrMakeVersion( REMAP_INI_VERSION, 0, 0 ) );
			
			params->selected = backup_selected;
		}
		
		inimgrDestroy( ini );
		
		return ret;
	}
	return CG_ERROR_NOT_ENOUGH_MEMORY;
}

static int remap_load( struct remap_params *params, const char *path )
{
	InimgrUID ini = inimgrNew();
	
	if( ini ){
		int ret;
		
		inimgrRegisterCallback( ini, REMAP_SECTION_DATA"*", remap_inicb_load, (void *)params );
		ret = inimgrLoad( ini, path, REMAP_INI_SIGNATURE, inimgrMakeVersion( REMAP_INI_VERSION, 0, 0 ) );
		inimgrLoadAllSections( ini );
		inimgrDestroy( ini );
		
		return ret;
	}
	return CG_ERROR_NOT_ENOUGH_MEMORY;
}

/*-----------------------------------------------
	セーブロードコールバック
-----------------------------------------------*/
static int remap_inicb_load( InimgrContext ctx, void *arg )
{
	struct remap_params *params = (struct remap_params *)arg;
	char *name, *value;
	bool valid_data = false;
	
	if( ! mfConvertButtonReady() ){
		return CG_ERROR_NOT_ENOUGH_MEMORY;
	}
	
	if( ! remap_add( params ) ){
		mfConvertButtonFinish();
		return CG_ERROR_NOT_ENOUGH_MEMORY;
	}
	
	while( inimgrReadEntry( ctx, &name, &value ) ){
		if( strcasecmp( name, "RealButtons" ) == 0 ){
			if( ! params->selected->realButtons ){
				params->selected->realButtons = mfConvertButtonN2C( value );
				valid_data = true;
			}
		} else if( strcasecmp( name, "RemapButtons" ) == 0 ){
			if( ! params->selected->remapButtons ){
				params->selected->remapButtons = mfConvertButtonN2C( value );
			}
		}
	}
	
	if( ! valid_data ) remap_delete( params );
	mfConvertButtonFinish();
	
	return CG_ERROR_OK;
}

static int remap_inicb_save( InimgrContext ctx, void *arg )
{
	struct remap_inisave_arg *iniarg = (struct remap_inisave_arg *)arg;
	char tmpbuf[128];
	
	if( ! mfConvertButtonReady() ) return CG_ERROR_NOT_ENOUGH_MEMORY;
	
	if( iniarg->params->selected ){
		/* データ書き込み */
		mfConvertButtonC2N( iniarg->params->selected->realButtons, tmpbuf, sizeof( tmpbuf ) );
		inimgrWriteEntry( ctx, "RealButtons", tmpbuf );
		mfConvertButtonC2N( iniarg->params->selected->remapButtons, tmpbuf, sizeof( tmpbuf ) );
		inimgrWriteEntry( ctx, "RemapButtons", tmpbuf );
	}
	
	if( iniarg->params->selected->next ){
		iniarg->index++;
		iniarg->params->selected = iniarg->params->selected->next;
		snprintf( tmpbuf, sizeof( tmpbuf ), "%s%d", REMAP_SECTION_DATA, iniarg->index );
		inimgrAddSection( inimgrGetCurrentUID( ctx ), tmpbuf );
	}
	
	mfConvertButtonFinish();
	
	return CG_ERROR_OK;
}

/*-----------------------------------------------
	メニューコントロール
-----------------------------------------------*/
static int remap_control_load( MfMessage message, const char *label, void *var, void *pref, void *ex )
{
	if( ( message & MF_CM_PROC ) && mfMenuIsPressed( mfMenuAcceptButton() ) ){
		((MfCtrlDefIniPref *)pref)->title        = "Load remap list";
		((MfCtrlDefIniPref *)pref)->initFilename = "";
	}
	return mfCtrlDefIniLoad( message, label, var, pref, ex );
}

static int remap_control_save( MfMessage message, const char *label, void *var, void *pref, void *ex )
{
	if( ( message & MF_CM_PROC ) && mfMenuIsPressed( mfMenuAcceptButton() ) ){
		((MfCtrlDefIniPref *)pref)->title        = "Save remap list";
		((MfCtrlDefIniPref *)pref)->initFilename = "remap.ini";
	}
	return mfCtrlDefIniSave( message, label, var, pref, ex );
}

static int remap_ini_handle( MfCtrlDefIniAction action, const char *path )
{
	int ret;
	
	switch( action ){
		case MF_CTRL_INI_LOAD:
			remap_clear( &st_params );
			ret = remap_load( &st_params, path );
			if( CG_IS_ERROR( ret ) ) remap_clear( &st_params );
			return ret;
		case MF_CTRL_INI_SAVE:
			return remap_save( &st_params, path );
	}
	return 0;
}

static int remap_menu_clear( MfMessage message )
{
	if( message & MF_MM_INIT ){
		return mfDialogMessage( MF_STR_REMAP_CLEAR_WARN_TITLE, MF_STR_REMAP_CLEAR_WARN_MSG, 0, true );
	} else if( message & MF_DM_FINISH ){
		if( mfDialogLastResult() ) remap_clear( &st_params );
		mfMenuExitExtra();
	}
	return MF_MENU_OK;

}

