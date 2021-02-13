/*=========================================================

	macro.c

	マクロ。

=========================================================*/
#include "macro.h"

/*=========================================================
	定数/マクロ
=========================================================*/
#define MACRO_HOTKEY_IS_PRESSED( pad, hk, hotkey ) \
	mfIsPressed( padutilSetPad( pad->Buttons | padutilGetAnalogStickDirection( pad->Lx, pad->Ly, 0 ) ) | padutilSetHprm( hk ), hotkey )

#define MACRO_CHECK_HOTKEY( pad, hk, avail ) \
	mfIsAnyPressed( padutilSetPad( pad->Buttons | padutilGetAnalogStickDirection( pad->Lx, pad->Ly, 0 ) ) | padutilSetHprm( hk ), avail )

/*=========================================================
	ローカル型宣言
=========================================================*/
enum macro_action_type {
	MACRO_NTD = 0,
	MACRO_TRACE,
	MACRO_RECORD,
};

struct macro_common_tempdata {
	unsigned int lastButtons;
	unsigned int analogX;
	unsigned int analogY;
	uint64_t     rtc;
};

struct macro_trace_tempdata {
	MfRapidfireUID rfUid;
	int            loopRest;
	uint64_t       rtc;
};

struct macro_tempdata{
	struct macro_common_tempdata common;
	struct macro_trace_tempdata trace;
	MacromgrCommand *cmd;
};
	
struct macro_data {
	MacromgrUID macro;
	char name[MACRO_NAME_LENGTH];
	PadutilButtons hotkey;
	unsigned int loopNum;
	bool analogStickEnable;

	struct macro_data *next, *prev;
};

struct macro_params_main {
	struct macro_data *rootData;
	bool              hotkeyAccept;
};

struct macro_params_menu {
	struct macro_data *selectedData;
	unsigned int selectedIndex;
};

struct macro_params_action {
	DmemUID           memoryUid;
	struct macro_data *actionData;
	enum macro_action_type actionType;
	struct macro_tempdata *temp;
};

struct macro_params {
	struct macro_params_main   main;
	struct macro_params_menu   menu;
	struct macro_params_action work;
};

/*=========================================================
	ローカル関数
=========================================================*/

static void macro_slot_forward( struct macro_params_menu *menu );
static void macro_slot_back( struct macro_params_menu *menu );
static int macro_control_slot( MfMessage message, const char *label, void *var, void *pref, void *ex );
static int macro_control_hotkey( MfMessage message, const char *label, void *var, void *pref, void *ex );
static int macro_control_loop( MfMessage message, const char *label, void *var, void *pref, void *ex );
static int macro_control_analogstick( MfMessage message, const char *label, void *var, void *pref, void *ex );
static int macro_control_load( MfMessage message, const char *label, void *var, void *pref, void *ex );
static int macro_control_save( MfMessage message, const char *label, void *var, void *pref, void *ex );

static void macro_menu_update( struct macro_params *params );
static int macro_menu_add( MfMessage message );
static int macro_menu_remove( MfMessage message );
static int macro_menu_record_start( MfMessage message );
static int macro_menu_append_start( MfMessage message );
static int macro_menu_record_stop( MfMessage message );
static int macro_menu_run( MfMessage message );
static int macro_menu_stop( MfMessage message );
static int macro_menu_edit( MfMessage message );
static int macro_menu_clear( MfMessage message );

static bool macro_add( struct macro_params *params );
static bool macro_remove( struct macro_params *params );

static void macro_clear( struct macro_data *macro );
static int macro_ini_handler( MfCtrlDefIniAction action, const char *path );
static int macro_save( struct macro_data *macro, const char *path );
static int macro_load( struct macro_data *macro, const char *path );

static void macro_record_prepare( struct macro_params_action *work, struct macro_data *macro, MacromgrCommand *start );
static void macro_record_stop( struct macro_params_action *work );
static void macro_record( struct macro_params_action *work, SceCtrlData *pad );
static void macro_trace_prepare( struct macro_params_action *work, struct macro_data *macro, MacromgrCommand *start );
static void macro_trace_stop( struct macro_params_action *work );
static bool macro_trace( struct macro_params_action *work, SceCtrlData *pad );

static unsigned int macro_calc_delayms( MacromgrData base );

static void macro_mfengine_warning( void );

/*=========================================================
	ローカル変数
=========================================================*/
static struct macro_params   st_params;

static MfMenuTable *st_menu;

/*=========================================================
	関数
=========================================================*/
void *macroProc( MfMessage message )
{
	switch( message ){
		case MF_MS_INIT:
			/* この値だけは設定する */
			st_params.main.hotkeyAccept = true;
			break;
		case MF_MS_INI_LOAD:
			return macroIniLoad;
		case MF_MS_INI_CREATE:
			return macroIniCreate;
		case MF_MS_TERM:
			break;
		case MF_MS_TOGGLE:
			if( st_params.work.actionType == MACRO_TRACE ) macro_trace_stop( &(st_params.work) );
			break;
		case MF_MS_HOOK:
			return macroMain;
		case MF_MS_MENU:
			return macroMenu;
		default:
			break;
	}
	return NULL;
}

void macroIniLoad( InimgrUID ini, char *buf, size_t len )
{
	unsigned int entry_i = 0;
	char entryname[32];
	
	while( ++entry_i ){
		/* エントリ名を作成 */
		snprintf( entryname, sizeof( entryname ), "Macro%d", entry_i );
		
		if( CG_IS_OK( mfIniGetString( ini, entryname, buf, len ) ) ){
			if( macro_add( &st_params ) ){
				if( CG_IS_ERROR( macro_load( st_params.menu.selectedData, buf ) ) ) macro_remove( &st_params );
			}
		} else{
			break;
		}
	}
}

void macroIniCreate( InimgrUID ini, char *buf, size_t len )
{
	inimgrSetString( ini, MF_INI_SECTION_DEFAULT, "Macro1", "" );
}

void macroMain( MfHookAction action, SceCtrlData *pad, MfHprmKey *hk )
{
	if( ! st_params.main.rootData || ! mfIsEnabled() || mfIsRunningApp( MF_APP_WEBBROWSER ) ) return;
	
	if( action == MF_KEEP ){
		if( st_params.work.actionType == MACRO_TRACE ) pad->Buttons |= st_params.work.temp->common.lastButtons;
		return;
	}
	
	switch( st_params.work.actionType ){
		case MACRO_RECORD:
			macro_record( &(st_params.work), pad );
			break;
		case MACRO_TRACE:
			if( st_params.work.actionData->hotkey ){
				bool trigger = MACRO_HOTKEY_IS_PRESSED( pad, *hk, st_params.work.actionData->hotkey );
				
				if( ! trigger && ! st_params.main.hotkeyAccept ){
					st_params.main.hotkeyAccept = true;
				} else if( trigger && st_params.main.hotkeyAccept ){
					st_params.main.hotkeyAccept = false;
					if( mfNotificationThreadId() ) mfNotificationPrintf( MF_STR_MACRO_OVMSG_HALT );
					macro_trace_stop( &(st_params.work) );
					break;
				}
			}
			if( ! macro_trace( &(st_params.work), pad ) ){
				macro_trace_stop( &(st_params.work ) );
				if( mfNotificationThreadId() ) mfNotificationPrintf( MF_STR_MACRO_OVMSG_FINISH );
			}
			break;
		default:
			if( st_params.main.hotkeyAccept ){
				if( ! MACRO_CHECK_HOTKEY( pad, *hk, MF_HOTKEY_BUTTONS ) ) break;
				for( st_params.work.actionData = st_params.main.rootData, st_params.menu.selectedIndex = 0; st_params.work.actionData; st_params.work.actionData = st_params.work.actionData->next, st_params.menu.selectedIndex++ ){
					if( st_params.work.actionData->hotkey && MACRO_HOTKEY_IS_PRESSED( pad, *hk, st_params.work.actionData->hotkey ) ){
						/* 実行したマクロをメニューで選択状態にするために実行したマクロのポインタをセット */
						st_params.menu.selectedData = st_params.work.actionData;
						st_params.main.hotkeyAccept = false;
						
						if( mfNotificationThreadId() ) mfNotificationPrintf( MF_STR_MACRO_OVMSG_START, st_params.menu.selectedIndex + 1, st_params.work.actionData->name );
						
						macro_trace_prepare( &(st_params.work), st_params.work.actionData, macromgrSeek( st_params.work.actionData->macro, 0, MACROMGR_SEEK_SET, NULL ) );
						macro_trace( &(st_params.work), pad );
						
						break;
					}
				}
			} else if( ! MACRO_HOTKEY_IS_PRESSED( pad, *hk, st_params.work.actionData->hotkey ) ){
				st_params.main.hotkeyAccept = true;
			}
	}
}

int macroMenu( MfMessage message )
{
	if( mfIsRunningApp( MF_APP_WEBBROWSER ) ){
		if( message == MF_MM_PROC ){
			mfMenuError( CG_OK, MF_STR_LIMIT_WEBBROWSER );
			return MF_MENU_BACK;
		}
	}
	
	switch( mfMenuMaskMainMessage( message ) ){
		case MF_MM_INIT:
			/* 選択されているデータがあればインデックス番号を調べる */
			st_params.menu.selectedIndex = 0;
			if( st_params.menu.selectedData ){
				struct macro_data *macro;
				unsigned int index;
				for( macro = st_params.main.rootData, index = 0; macro; macro = macro->next, index++ ){
					if( macro == st_params.menu.selectedData ){
						st_params.menu.selectedIndex = index;
						break;
					}
				}
			}
			
			dbgprint( "Creating macro menu" );
			st_menu = mfMenuCreateTables(
				6,
				1, 2, /* memory */
				1, 1, /* slot */
				2, 1, /* preference */
				2, 2, /* control */
				3, 2, /* recording */
				2, 1  /* save/load */
			);
			if( st_menu ){
				MfCtrlDefGetButtonsPref *pref_hotkey = mfMenuMemoryCalloc( sizeof( MfCtrlDefGetButtonsPref ) );
				MfCtrlDefGetNumberPref  *pref_loop   = mfMenuMemoryCalloc( sizeof( MfCtrlDefGetNumberPref ) );
				MfCtrlDefIniPref        *pref_ini    = mfMenuMemoryCalloc( sizeof( MfCtrlDefIniPref ) );
				
				if( ! pref_hotkey || ! pref_loop || ! pref_ini || ! mfMenuRegisterTempMemory( macroMenu, 4, st_menu, pref_hotkey, pref_loop, pref_ini ) ){
					mfMenuDestroyTables( st_menu );
					if( pref_hotkey ) mfMenuMemoryFree( pref_hotkey );
					if( pref_loop   ) mfMenuMemoryFree( pref_loop );
					if( pref_ini    ) mfMenuMemoryFree( pref_ini );
					return CG_ERROR_NOT_ENOUGH_MEMORY;
				}
				
				pref_hotkey->availButtons = MF_HOTKEY_BUTTONS;
				pref_loop->max            = 99999;
				pref_ini->title           = NULL;
				pref_ini->initDir         = "ms0:";
				pref_ini->initFilename    = NULL;
				pref_ini->pathMax         = MF_PATH_MAX;
				
				mfMenuSetTablePosition( st_menu, 1, pbOffsetChar( 5 ), pbOffsetLine( 6 ) );
				mfMenuSetColumnWidth( st_menu, 1, 1, pbMeasureString( MF_STR_MACRO_SLOT_ADD ) + pbOffsetChar( 2 ) );
				mfMenuSetTableEntry( st_menu, 1, 1, 1, MF_STR_MACRO_SLOT_ADD, mfCtrlDefExtra, macro_menu_add, NULL );
				mfMenuSetTableEntry( st_menu, 1, 1, 2, MF_STR_MACRO_SLOT_REMOVE, mfCtrlDefExtra, macro_menu_remove, NULL );
				
				mfMenuSetTablePosition( st_menu, 2, pbOffsetChar( 5 ), pbOffsetLine( 9 ) );
				mfMenuSetTableEntry( st_menu, 2, 1, 1, MF_STR_MACRO_SLOT, macro_control_slot, NULL, NULL );
				
				mfMenuSetTablePosition( st_menu, 3, pbOffsetChar( 5 ), pbOffsetLine( 11 ) );
				mfMenuSetTableLabel( st_menu, 3, MF_STR_MACRO_PREF_LABEL );
				mfMenuSetTableEntry( st_menu, 3, 1, 1, MF_STR_MACRO_PREF_RUN_STOP, macro_control_hotkey, NULL, pref_hotkey );
				mfMenuSetTableEntry( st_menu, 3, 2, 1, MF_STR_MACRO_PREF_LOOP_NUM, macro_control_loop, NULL, pref_loop );
				
				mfMenuSetTablePosition( st_menu, 4, pbOffsetChar( 5 ), pbOffsetLine( 15 ) );
				mfMenuSetTableLabel( st_menu, 4, MF_STR_MACRO_CONTROL_LABEL );
				mfMenuSetTableEntry( st_menu, 4, 1, 1, MF_STR_MACRO_CONTROL_RUN,   mfCtrlDefCallback, macro_menu_run, NULL );
				mfMenuSetTableEntry( st_menu, 4, 2, 1, MF_STR_MACRO_CONTROL_STOP,  mfCtrlDefExtra, macro_menu_stop, NULL );
				mfMenuSetTableEntry( st_menu, 4, 1, 2, MF_STR_MACRO_CONTROL_EDIT,  mfCtrlDefCallback, macro_menu_edit, NULL );
				mfMenuSetTableEntry( st_menu, 4, 2, 2, MF_STR_MACRO_CONTROL_CLEAR, mfCtrlDefExtra, macro_menu_clear, NULL );
				
				mfMenuSetTablePosition( st_menu, 5, pbOffsetChar( 5 ), pbOffsetLine( 19 ) );
				mfMenuSetTableLabel( st_menu, 5, MF_STR_MACRO_RECORD_LABEL );
				mfMenuSetTableEntry( st_menu, 5, 1, 1, MF_STR_MACRO_RECORD_START,  mfCtrlDefCallback, macro_menu_record_start, NULL );
				mfMenuSetTableEntry( st_menu, 5, 2, 1, MF_STR_MACRO_RECORD_STOP,   mfCtrlDefExtra, macro_menu_record_stop, NULL );
				mfMenuSetTableEntry( st_menu, 5, 3, 1, MF_STR_MACRO_RECORD_ANALOG ,macro_control_analogstick, NULL, NULL );
				mfMenuSetTableEntry( st_menu, 5, 1, 2, MF_STR_MACRO_RECORD_APPEND, mfCtrlDefCallback, macro_menu_append_start, NULL );
				
				mfMenuSetTablePosition( st_menu, 6, pbOffsetChar( 5 ), pbOffsetLine( 24 ) );
				mfMenuSetTableLabel( st_menu, 6, MF_STR_MACRO_FILE_LABEL );
				mfMenuSetTableEntry( st_menu, 6, 1, 1, MF_STR_LOAD, macro_control_load, macro_ini_handler, pref_ini );
				mfMenuSetTableEntry( st_menu, 6, 2, 1, MF_STR_SAVE, macro_control_save, macro_ini_handler, pref_ini );
				
				mfMenuInitTables( st_menu, 5 );
				macro_menu_update( &st_params );
				break;
			}
			return CG_ERROR_NOT_ENOUGH_MEMORY;
		case MF_MM_TERM:
			break;
		default:
			pbPrint( pbOffsetChar( 3 ), pbOffsetLine(  4 ), MF_COLOR_TEXT_FG, MF_COLOR_TEXT_BG, MF_STR_MACRO_DESC );
			return mfMenuDrawTables( st_menu, 6, MF_MENU_NO_OPTIONS );
	}
	return MF_MENU_OK;
}

/*-----------------------------------------------
	メニューコントロール
-----------------------------------------------*/
static void macro_menu_update( struct macro_params *params )
{
	mfMenuDeactivateTableEntry( st_menu, 1, 1, 2 );
	mfMenuDeactivateTableEntry( st_menu, 2, 1, 1 );
	mfMenuDeactivateTable( st_menu, 3 );
	mfMenuDeactivateTable( st_menu, 4 );
	mfMenuDeactivateTable( st_menu, 5 );
	mfMenuDeactivateTable( st_menu, 6 );
	
	if( params->menu.selectedData ){
		mfMenuActivateTableEntry( st_menu, 1, 1, 2 );
		mfMenuActivateTableEntry( st_menu, 2, 1, 1 );
		mfMenuActivateTableEntry( st_menu, 3, 1, 1 );
		mfMenuActivateTableEntry( st_menu, 5, 3, 1 );
		if( params->work.actionType == MACRO_NTD ){
			mfMenuActivateTableEntry( st_menu, 3, 2, 1 );
			mfMenuActivateTableEntry( st_menu, 4, 1, 2 );
			mfMenuActivateTableEntry( st_menu, 5, 1, 1 );
			mfMenuActivateTableEntry( st_menu, 6, 1, 1 );
			if( params->menu.selectedData->macro ){
				mfMenuActivateTableEntry( st_menu, 4, 1, 1 );
				mfMenuActivateTableEntry( st_menu, 4, 2, 2 );
				mfMenuActivateTableEntry( st_menu, 5, 1, 2 );
				mfMenuActivateTableEntry( st_menu, 6, 2, 1 );
			}
		} else if( params->work.actionType == MACRO_RECORD ){
			mfMenuActivateTableEntry( st_menu, 3, 2, 1 );
			if( params->menu.selectedData == params->work.actionData ){
				mfMenuDeactivateTableEntry( st_menu, 1, 1, 2 );
				mfMenuActivateTableEntry( st_menu, 5, 2, 1 );
			} else{
				mfMenuActivateTableEntry( st_menu, 3, 2, 1 );
				mfMenuActivateTableEntry( st_menu, 4, 1, 2 );
				mfMenuActivateTableEntry( st_menu, 6, 1, 1 );
				if( params->menu.selectedData->macro ){
					mfMenuActivateTableEntry( st_menu, 4, 2, 2 );
					mfMenuActivateTableEntry( st_menu, 6, 2, 1 );
				}
			}
		} else if( params->work.actionType == MACRO_TRACE ){
			if( params->menu.selectedData == params->work.actionData ){
				mfMenuDeactivateTableEntry( st_menu, 1, 1, 2 );
				mfMenuActivateTableEntry( st_menu, 4, 2, 1 );
			} else{
				mfMenuActivateTableEntry( st_menu, 3, 2, 1 );
				mfMenuActivateTableEntry( st_menu, 4, 1, 2 );
				mfMenuActivateTableEntry( st_menu, 6, 1, 1 );
				if( params->menu.selectedData->macro ){
					mfMenuActivateTableEntry( st_menu, 4, 2, 2 );
					mfMenuActivateTableEntry( st_menu, 6, 2, 1 );
				}
			}
		}
	}
}

static void macro_slot_forward( struct macro_params_menu *menu )
{
	menu->selectedIndex++;
	menu->selectedData = menu->selectedData->next;
}

static void macro_slot_back( struct macro_params_menu *menu )
{
	menu->selectedIndex--;
	menu->selectedData = menu->selectedData->prev;
}

static int macro_control_slot( MfMessage message, const char *label, void *nothing, void *null, void *ex )
{
	if( ! st_params.menu.selectedData ) return CG_OK;
	
	switch( mfMenuMaskMainMessage( message ) ){
		case MF_CM_LABEL:
			{
				char *state = NULL;
				if( st_params.work.actionType != MACRO_NTD ){
					if( st_params.menu.selectedData == st_params.work.actionData ){
						if( st_params.work.actionType == MACRO_RECORD ){
							state = MF_STR_MACRO_STAT_RECORDING;
						} else if( st_params.work.actionType == MACRO_TRACE ){
							state = MF_STR_MACRO_STAT_RUNNING;
						}
					} else{
						state = MF_STR_MACRO_STAT_WAITING;
					}
				} else if( st_params.menu.selectedData->macro ){
					state = MF_STR_MACRO_STAT_AVAILABLE;
				} else{
					state = MF_STR_MACRO_STAT_UNASSIGNED;
				}
				mfCtrlSetLabel( ex, "%s: %d [%s] (%s)", label, st_params.menu.selectedIndex + 1, st_params.menu.selectedData->name, state );
			}
			break;
		case MF_CM_INFO:
			mfMenuSetInfoText( MF_MENU_INFOTEXT_COMMON_CTRL | MF_MENU_INFOTEXT_SET_LOWER_LINE, "(%s)%s, (%s)%s, (%s)%s", PB_SYM_PSP_SQUARE, MF_STR_MACRO_CTRL_PREV, PB_SYM_PSP_CIRCLE,  MF_STR_MACRO_CTRL_NEXT, PB_SYM_PSP_TRIANGLE, MF_STR_MACRO_CTRL_EDIT_NAME );
			break;
		case MF_CM_PROC:
			if( mfMenuIsPressed( PSP_CTRL_TRIANGLE ) ){
				mfDialogSosk( MF_STR_MACRO_DIAGSOSK_NAME, st_params.menu.selectedData->name, MACRO_NAME_LENGTH, CDIALOG_SOSK_INPUTTYPE_ASCII );
			} else if( mfMenuIsPressed( PSP_CTRL_SQUARE ) && st_params.menu.selectedData->prev ){
				macro_slot_back( &(st_params.menu) );
				macro_menu_update( &st_params );
			} else if( mfMenuIsPressed( mfMenuAcceptButton() ) && st_params.menu.selectedData->next ){
				macro_slot_forward( &(st_params.menu) );
				macro_menu_update( &st_params );
			}
			break;
	}
	
	return CG_OK;
}

static int macro_control_hotkey( MfMessage message, const char *label, void *null, void *pref, void *ex )
{
	if( ! st_params.menu.selectedData ) return CG_OK;
	return mfCtrlDefGetButtons( message, label, &(st_params.menu.selectedData->hotkey), pref, ex );
}

static int macro_control_loop( MfMessage message, const char *label, void *null, void *pref, void *ex )
{
	if( ! st_params.menu.selectedData ) return CG_OK;
	
	if( ( message & MF_CM_LABEL ) && st_params.menu.selectedData->loopNum == 0 ){
		mfCtrlSetLabel( ex, "%s: 0 (%s)", label, MF_STR_MACRO_LOOP_INFINITY );
		return true;
	} else{
		return mfCtrlDefGetNumber( message, label, &(st_params.menu.selectedData->loopNum), pref, ex );
	}
}

static int macro_control_analogstick( MfMessage message, const char *label, void *null, void *pref, void *ex )
{
	if( ! st_params.menu.selectedData ) return CG_OK;
	return mfCtrlDefBool( message, label, &(st_params.menu.selectedData->analogStickEnable), pref, ex );
}

static int macro_control_load( MfMessage message, const char *label, void *var, void *pref, void *ex )
{
	if( ( message & MF_CM_PROC ) && mfMenuIsPressed( mfMenuAcceptButton() ) ){
		((MfCtrlDefIniPref *)pref)->title        = MF_STR_MACRO_DIAGGETFN_LOAD;
		((MfCtrlDefIniPref *)pref)->initFilename = "";
	} else if( message & MF_CM_CONTINUE ){
		macro_menu_update( &st_params );
	}
	return mfCtrlDefIniLoad( message, label, var, pref, ex );
}

static int macro_control_save( MfMessage message, const char *label, void *var, void *pref, void *ex )
{
	if( ( message & MF_CM_PROC ) && mfMenuIsPressed( mfMenuAcceptButton() ) ){
		((MfCtrlDefIniPref *)pref)->title = MF_STR_MACRO_DIAGGETFN_SAVE;
		((MfCtrlDefIniPref *)pref)->initFilename = mfMenuMemoryAlloc( MACRO_NAME_LENGTH + 4 );
		
		if( (st_params.menu.selectedData->name)[0] != '\0' ){
			snprintf( ((MfCtrlDefIniPref *)pref)->initFilename, MACRO_NAME_LENGTH + 4, "%s.ini", st_params.menu.selectedData->name );
		} else{
			snprintf( ((MfCtrlDefIniPref *)pref)->initFilename, MACRO_NAME_LENGTH + 4, "macro%d.ini", st_params.menu.selectedIndex + 1 );
		}
	} else if( message & MF_CM_CONTINUE ){
		mfMenuMemoryFree( ((MfCtrlDefIniPref *)pref)->initFilename );
	}
	return mfCtrlDefIniSave( message, label, var, pref, ex );
}

/*-----------------------------------------------
	メニュー項目
-----------------------------------------------*/
static int macro_menu_add( MfMessage message )
{
	int ret = CG_OK;
	if( ( message & MF_MM_PROC ) && ! mfMenuMaskDialogMessage( message ) ){
		bool added = macro_add( &st_params );
		macro_menu_update( &st_params );
		
		if( added ){
			mfDialogMessage( MF_STR_MACRO_MSG_SLOT_ADD_TITLE, MF_STR_MACRO_MSG_SLOT_ADD, 0, false );
		} else{
			ret = CG_ERROR_NOT_ENOUGH_MEMORY;
		}
		mfMenuExitExtra();
	}
	return ret;
}

static int macro_menu_remove( MfMessage message )
{
	if( message & MF_MM_INIT && st_params.menu.selectedData->macro ){
		return mfDialogMessage( MF_STR_MACRO_MSG_SLOT_REMOVE_TITLE, MF_STR_MACRO_MSG_SLOT_REMOVE_WARN, 0, true );
	} else if( ( message & MF_MM_PROC ) && ( ! mfMenuMaskDialogMessage( message ) || message & MF_DM_FINISH ) ){
		if( ! st_params.menu.selectedData->macro || mfDialogLastResult() ){
			if( macro_remove( &st_params ) ){
				mfDialogMessage( MF_STR_MACRO_MSG_SLOT_REMOVE_TITLE, MF_STR_MACRO_MSG_SLOT_REMOVE, 0, false );
			}
			macro_menu_update( &st_params );
		}
		mfMenuExitExtra();
	}
	return CG_OK;
}

static int macro_menu_record_start( MfMessage message )
{
	if( message & MF_MM_INIT ){
		if( ! mfIsEnabled() ){
			macro_mfengine_warning();
			return MF_MENU_BACK;
		} else{
			MacromgrCommand *cmd;
			
			if( ! st_params.menu.selectedData->macro ){
				st_params.menu.selectedData->macro = macromgrNew();
				if( ! st_params.menu.selectedData->macro ) return CG_ERROR_NOT_ENOUGH_MEMORY;
			} else{
				macromgrClear( st_params.menu.selectedData->macro );
			}
			
			cmd = macromgrCreateRoot( st_params.menu.selectedData->macro );
			macro_record_prepare( &(st_params.work), st_params.menu.selectedData, cmd );
		}
	} else if( message & MF_MM_PROC ){
		mfMenuQuit();
	}
	return CG_OK;
}

static int macro_menu_append_start( MfMessage message )
{
	if( message & MF_MM_INIT ){
		if( ! mfIsEnabled() ){
			macro_mfengine_warning();
			return MF_MENU_BACK;
		}
	} else if( message & MF_MM_PROC ){
		MacromgrCommand *cmd = macromgrSeek( st_params.menu.selectedData->macro, 0, MACROMGR_SEEK_END, NULL );
		
		cmd = macromgrInsertAfter( st_params.menu.selectedData->macro, cmd );
		macro_record_prepare( &(st_params.work), st_params.menu.selectedData, cmd );
		
		mfMenuQuit();
		
	}
	return CG_OK;
}

static int macro_menu_record_stop( MfMessage message )
{
	if( message & MF_MM_PROC ){
		macro_record_stop( &(st_params.work) );
		macro_menu_update( &st_params );
		mfMenuExitExtra();
	}
	return CG_OK;
}

static int macro_menu_run( MfMessage message )
{
	if( message & MF_MM_PROC ){
		if( ! mfIsEnabled() ){
			macro_mfengine_warning();
		} else{
			macro_trace_prepare( &(st_params.work), st_params.menu.selectedData, macromgrSeek( st_params.menu.selectedData->macro, 0, MACROMGR_SEEK_SET, NULL ) );
			mfMenuQuit();
		}
	}
	return CG_OK;
}

static int macro_menu_stop( MfMessage message )
{
	if( message & MF_MM_PROC ){
		macro_trace_stop( &(st_params.work) );
		macro_menu_update( &st_params );
		mfMenuExitExtra();
	}
	return CG_OK;
}

static int macro_menu_edit( MfMessage message )
{
	if( message & MF_MM_INIT ){
		if( ! st_params.menu.selectedData->macro ){
			dbgprint( "Creating minimize macro" );
			st_params.menu.selectedData->macro = macromgrNew();
			if( ! st_params.menu.selectedData->macro ){
				return CG_ERROR_NOT_ENOUGH_MEMORY;
			} else{
				macromgrCreateRoot( st_params.menu.selectedData->macro );
			}
		}
		dbgprintf( "Init macroeditor: %X", st_params.menu.selectedData->macro );
		if( ! macroeditorInit( st_params.menu.selectedData->macro ) ) return CG_ERROR_NOT_ENOUGH_MEMORY;
	} else if( message & MF_MM_TERM ){
		dbgprint( "Term macroeditor" );
		macroeditorTerm();
		macro_menu_update( &st_params );
	} else{
		if( ! macroeditorMain( message ) ) mfMenuProc( NULL );
	}
	return CG_OK;
}

static int macro_menu_clear( MfMessage message )
{
	if( message & MF_MM_INIT ){
		return mfDialogMessage( MF_STR_MACRO_CLEAR_WARN_TITLE, MF_STR_MACRO_CLEAR_WARN_MSG, 0, true );
	} else if( message & MF_MM_PROC ){
		if( message & MF_DM_FINISH ){
			if( mfDialogLastResult() ){
				macro_clear( st_params.menu.selectedData );
				macro_menu_update( &st_params );
			}
			mfMenuExitExtra();
		}
	}
	return CG_OK;
}

/*-----------------------------------------------
	マクロの操作
-----------------------------------------------*/
static bool macro_add( struct macro_params *params )
{
	struct macro_data *new_macro;
	
	if( ! params->work.memoryUid ){
		params->work.memoryUid = dmemNew( sizeof( struct macro_tempdata ) + sizeof( struct macro_data ) * 10, MEMORY_USER, PSP_SMEM_Low );
		if( ! params->work.memoryUid ) return false;
		
		params->work.temp = dmemAlloc( params->work.memoryUid, sizeof( struct macro_tempdata ) );
		if( ! params->work.temp ){
			dmemDestroy( params->work.memoryUid );
			params->work.memoryUid = 0;
			return false;
		}
	}
	
	new_macro = dmemAlloc( params->work.memoryUid, sizeof( struct macro_data ) );
	if( ! new_macro ) return false;
	
	/* 必要なメンバを初期化 */
	new_macro->macro = 0;
	new_macro->name[0] = '\0';
	new_macro->hotkey = 0;
	new_macro->loopNum = 0;
	new_macro->analogStickEnable = false;
	
	if( ! params->main.rootData ){
		params->menu.selectedIndex = 0;
		params->main.rootData = new_macro;
		new_macro->next = NULL;
		new_macro->prev = NULL;
	} else{
		struct macro_data *last_macro;
		
		/* 末尾を探す */
		for( last_macro = params->main.rootData, params->menu.selectedIndex = 1; last_macro->next; last_macro = last_macro->next, params->menu.selectedIndex++ );
		new_macro->prev  = last_macro;
		new_macro->next  = NULL;
		last_macro->next = new_macro;
	}
	
	params->menu.selectedData = new_macro;
	
	return true;
}

static bool macro_remove( struct macro_params *params )
{
	struct macro_data *macro = params->menu.selectedData;
	
	if( macro->next ) macro->next->prev = macro->prev;
	if( macro->prev ) macro->prev->next = macro->next;
	
	if( macro == params->main.rootData ){
		if( macro->next ){
			params->menu.selectedData = macro->next;
			params->main.rootData = params->menu.selectedData;
			macro_clear( macro );
			dmemFree( params->work.memoryUid, macro );
		} else{
			dmemDestroy( params->work.memoryUid );
			params->work.memoryUid = 0;
			params->work.temp = NULL;
			params->main.rootData = NULL;
			params->menu.selectedData = NULL;
			params->menu.selectedIndex = 0;
		}
	} else{
		if( macro->next ){
			params->menu.selectedData = macro->next;
		} else if( macro->prev ){
			params->menu.selectedIndex--;
			params->menu.selectedData = macro->prev;
		}
		macro_clear( macro );
		dmemFree( params->work.memoryUid, macro );
	}
	
	return true;
}

static void macro_clear( struct macro_data *macro )
{
	if( macro->macro ){
		macromgrDestroy( macro->macro );
		memset( macro, 0, sizeof( struct macro_data ) );
	}
}

static int macro_ini_handler( MfCtrlDefIniAction action, const char *path )
{
	int ret;
	
	switch( action ){
		case MF_CTRL_INI_LOAD:
			macro_clear( st_params.menu.selectedData );
			ret = macro_load( st_params.menu.selectedData, path );
			if( ret != CG_ERROR_OK ) macro_clear( st_params.menu.selectedData );
			return ret;
		case MF_CTRL_INI_SAVE:
			return macro_save( st_params.menu.selectedData, path );
	}
	return 0;
}

static int macro_save( struct macro_data *macro, const char *path )
{
	InimgrUID ini = inimgrNew();
	if( ini ){
		int ret;
		char hotkey[MACRO_HOTKEY_LENGTH];
		
		if( mfConvertButtonReady() ){
			mfConvertButtonC2N( macro->hotkey, hotkey, sizeof( hotkey ) );
			mfConvertButtonFinish();
			
			inimgrAddSection( ini, MACRO_SECTION_INFO );
			inimgrSetString( ini, MACRO_SECTION_INFO, MACRO_ENTRY_NAME, macro->name );
			inimgrSetString( ini, MACRO_SECTION_INFO, MACRO_ENTRY_HOTKEY, hotkey );
			inimgrSetInt( ini, MACRO_SECTION_INFO, MACRO_ENTRY_REPEAT, macro->loopNum );
			
			inimgrAddSection( ini, MACRO_SECTION_DATA );
			inimgrRegisterCallback( ini, MACRO_SECTION_DATA, macromgrSaver, &(macro->macro) );
			
			ret = inimgrSave( ini, path, MACRO_INI_SIGNATURE, inimgrMakeVersion( MACROMGR_MACRO_VERSION, 0, 0 ) );
			inimgrDestroy( ini );
			
			if( ret != 0 ){
				return ret;
			} else{
				return CG_ERROR_OK;
			}
		} else{
			inimgrDestroy( ini );
		}
	}
	return CG_ERROR_NOT_ENOUGH_MEMORY;
}

static int macro_load( struct macro_data *macro, const char *path )
{
	InimgrUID ini;
	
	ini = inimgrNew();
	if( ini ){
		int ret;
		
		if( ! macro->macro ){
			macro->macro = macromgrNew();
			if( ! macro->macro ){
				inimgrDestroy( ini );
				return CG_ERROR_NOT_ENOUGH_MEMORY;
			}
			inimgrRegisterCallback( ini, MACRO_SECTION_DATA, macromgrLoader, &(macro->macro) );
		} else{
			inimgrRegisterCallback( ini, MACRO_SECTION_DATA, macromgrAppendLoader, &(macro->macro) );
		}
		
		ret = inimgrLoad( ini, path, MACRO_INI_SIGNATURE, inimgrMakeVersion( MACROMGR_MACRO_VERSION, 0, 0 ) );
		
		if( CG_IS_ERROR( ret ) ){
			inimgrDestroy( ini );
			return ret;
		} else{
			char hotkey[MACRO_HOTKEY_LENGTH];
			
			inimgrLoadAllSections( ini );
			
			inimgrGetString( ini, MACRO_SECTION_INFO, MACRO_ENTRY_NAME, macro->name, MACRO_NAME_LENGTH );
			inimgrGetString( ini, MACRO_SECTION_INFO, MACRO_ENTRY_HOTKEY, hotkey, sizeof( hotkey ) );
			inimgrGetInt( ini, MACRO_SECTION_INFO, MACRO_ENTRY_REPEAT, (int *)&(macro->loopNum) );
			
			inimgrDestroy( ini );
			
			if( macromgrSeek( macro->macro, 0, MACROMGR_SEEK_SET, NULL ) == NULL ){
				macromgrDestroy( macro->macro );
				macro->macro = 0;
			}
			
			if( mfConvertButtonReady() ){
				macro->hotkey = mfConvertButtonN2C( hotkey );
				mfConvertButtonFinish();
				return CG_ERROR_OK;
			}
		}
	}
	return CG_ERROR_NOT_ENOUGH_MEMORY;
}

/*-----------------------------------------------
	マクロの記録と実行
-----------------------------------------------*/
static unsigned int macro_calc_delayms( MacromgrData base )
{
	MacromgrData rtc, waitms;
	sceRtcGetCurrentTick( &rtc );
	
	waitms = ( rtc - base ) / 1000;
	return waitms > MACROMGR_MAX_DELAY ? MACROMGR_MAX_DELAY : waitms;
}

static void macro_record_prepare( struct macro_params_action *work, struct macro_data *macro, MacromgrCommand *start )
{
	work->actionData = macro;
	work->actionType = MACRO_RECORD;
	
	work->temp->common.lastButtons = 0;
	work->temp->common.analogX     = MACROMGR_ANALOG_CENTER;
	work->temp->common.analogY     = MACROMGR_ANALOG_CENTER;
	work->temp->cmd                = start;
	sceRtcGetCurrentTick( &(work->temp->common.rtc) );
}

static void macro_record_stop( struct macro_params_action *work )
{
	PadutilButtons menu_buttons = padutilGetPad( mfGetMenuButtons() );
	
	if( work->actionType != MACRO_RECORD ) return;
	
	/* 記録されたマクロを逆行してメニュー起動ボタンの入力を削除する */
	if( menu_buttons ){
		MacromgrAction action;
		MacromgrData data;
		MacromgrData sub;
		unsigned int check_buttons = menu_buttons;
		unsigned char x, y;
		
		do {
			macromgrGetCommand( work->temp->cmd, &action, &data, &sub );
			
			switch( action ){
				case MACROMGR_BUTTONS_PRESS:
				case MACROMGR_BUTTONS_RELEASE:
				case MACROMGR_BUTTONS_CHANGE:
					check_buttons ^= data & menu_buttons;
					break;
				case MACROMGR_ANALOG_MOVE:
					x = MACROMGR_GET_ANALOG_X( data );
					y = MACROMGR_GET_ANALOG_Y( data );
					check_buttons ^= padutilGetAnalogStickDirection( x, y, 0 ) & menu_buttons;
					break;
				default:
					break;
			}
			if( check_buttons == 0 ) break;
		} while( ( work->temp->cmd = macromgrPrev( work->temp->cmd ) ) );
		
		if( check_buttons == 0 ){
			work->temp->cmd = macromgrPrev( work->temp->cmd );
			if( ! work->temp->cmd ){
				macro_clear( work->actionData );
			} else{
				MacromgrCommand *cmd;
				while( ( cmd = macromgrNext( work->temp->cmd ) ) ) macromgrRemove( work->actionData->macro, cmd );
			}
		}
	}
	work->actionType       = MACRO_NTD;
	work->temp->cmd        = NULL;
	work->temp->common.rtc = 0;
}

static void macro_record( struct macro_params_action *work, SceCtrlData *pad )
{
	/* 記録開始前にmacro_record_prepare()を呼んで準備すること */
	
	unsigned int current_buttons = pad->Buttons & MF_TARGET_BUTTONS;
	bool         analog_move     = false;
	
	if( work->actionData->analogStickEnable && ( work->temp->common.analogX != pad->Lx || work->temp->common.analogY != pad->Ly ) ){
		analog_move = true;
	}
	
	if( current_buttons == work->temp->common.lastButtons && ! analog_move ){
		/* ボタンもアナログスティックも前回と同じならばディレイをセットして終了 */
		if( work->temp->cmd->prev ) macromgrSetDelay( work->temp->cmd, macro_calc_delayms( work->temp->common.rtc ) );
	} else{
		/* ボタンかアナログスティックに動きがあった場合 */
		MacromgrAction action;
		MacromgrData data;
		MacromgrData sub;
		
		unsigned int press_buttons   = ( work->temp->common.lastButtons ^ current_buttons ) & current_buttons;
		unsigned int release_buttons = ( work->temp->common.lastButtons ^ current_buttons ) & work->temp->common.lastButtons;
		
		macromgrGetCommand( work->temp->cmd, &action, &data, &sub );
		
		if( action == MACROMGR_DELAY ){
			if( data > 0 ){
				/* 現在のコマンドがディレイで時間が0より大きい場合は、ディレイを更新して次のコマンド作成 */
				macromgrSetDelay( work->temp->cmd, macro_calc_delayms( work->temp->common.rtc ) );
				work->temp->cmd = macromgrInsertAfter( work->actionData->macro, work->temp->cmd );
			}
		} else{
			/* 現在のコマンドがディレイじゃない場合は次のコマンド作成 */
			work->temp->cmd = macromgrInsertAfter( work->actionData->macro, work->temp->cmd );
		}
		
		/* コマンドの追加に失敗していれば記録終了 */
		if( ! work->temp->cmd ) goto ABORT;
		
		/* ボタンが異なっていればボタン変更コマンドをセット */
		if( press_buttons && release_buttons ){
			macromgrSetButtonsChange( work->temp->cmd, current_buttons );
			work->temp->cmd = macromgrInsertAfter( work->actionData->macro, work->temp->cmd );
		} else if( press_buttons ){
			macromgrSetButtonsPress( work->temp->cmd, press_buttons );
			work->temp->cmd = macromgrInsertAfter( work->actionData->macro, work->temp->cmd );
		} else if( release_buttons ){
			macromgrSetButtonsRelease( work->temp->cmd, release_buttons );
			work->temp->cmd = macromgrInsertAfter( work->actionData->macro, work->temp->cmd );
		}
		
		/* コマンドの追加に失敗していれば記録終了 */
		if( ! work->temp->cmd ) goto ABORT;
		
		/* アナログスティックが動いていれば移動コマンドをセット */
		if( analog_move ){
			macromgrSetAnalogMove( work->temp->cmd, pad->Lx, pad->Ly );
			work->temp->cmd = macromgrInsertAfter( work->actionData->macro, work->temp->cmd );
			
			/* コマンドの追加に失敗していれば記録終了 */
			if( ! work->temp->cmd ) goto ABORT;
		}
		
		/* 一時データを更新する */
		work->temp->common.lastButtons = current_buttons;
		work->temp->common.analogX     = pad->Lx;
		work->temp->common.analogY     = pad->Ly;
		sceRtcGetCurrentTick( &(work->temp->common.rtc) );
	}
	
	return;
	
	ABORT:
		macro_record_stop( work );
		return;
}

static void macro_trace_prepare( struct macro_params_action *work, struct macro_data *macro, MacromgrCommand *start )
{
	work->actionData = macro;
	work->actionType = MACRO_TRACE;
	
	work->temp->common.lastButtons = 0;
	work->temp->common.analogX     = MACROMGR_ANALOG_CENTER;
	work->temp->common.analogY     = MACROMGR_ANALOG_CENTER;
	
	work->temp->trace.loopRest = work->actionData->loopNum;
	work->temp->trace.rfUid    = mfRapidfireNew();
	
	sceRtcGetCurrentTick( &(work->temp->common.rtc) );
	work->temp->cmd = start;
}

static void macro_trace_stop( struct macro_params_action *work )
{
	if( work->actionType != MACRO_TRACE ) return;
	
	if( work->temp->trace.rfUid ){
		mfRapidfireDestroy( work->temp->trace.rfUid );
		work->temp->trace.rfUid = 0;
	}
	
	work->actionType       = MACRO_NTD;
	work->temp->cmd        = NULL;
	work->temp->common.rtc = 0;
	work->temp->trace.rtc  = 0;
}

static bool macro_trace( struct macro_params_action *work, SceCtrlData *pad )
{
	/* 記録開始前にmacro_trace_prepare()を呼んで準備すること */
	
	MacromgrAction action;
	MacromgrData   data;
	MacromgrData   sub;
	
	do {
		macromgrGetCommand( work->temp->cmd, &action, &data, &sub );
		if( ! data ) continue;
		
		switch( action ){
			case MACROMGR_DELAY:
				if( ! work->temp->trace.rtc ) sceRtcTickAddMicroseconds( &(work->temp->trace.rtc), &(work->temp->common.rtc), data * 1000 );
				sceRtcGetCurrentTick( &(work->temp->common.rtc) );
				if( sceRtcCompareTick( &(work->temp->common.rtc), &(work->temp->trace.rtc) ) >= 0 ){
					work->temp->trace.rtc = 0;
					continue;
				}
				break;
			case MACROMGR_BUTTONS_PRESS:
				work->temp->common.lastButtons |= data;
				break;
			case MACROMGR_BUTTONS_RELEASE:
				work->temp->common.lastButtons ^= data;
				break;
			case MACROMGR_BUTTONS_CHANGE:
				work->temp->common.lastButtons = data;
				break;
			case MACROMGR_ANALOG_MOVE:
				work->temp->common.analogX = MACROMGR_GET_ANALOG_X( data );
				work->temp->common.analogY = MACROMGR_GET_ANALOG_Y( data );
				break;
			case MACROMGR_RAPIDFIRE_START:
				if( work->temp->trace.rfUid ){
					mfRapidfireAutoRapid(
						work->temp->trace.rfUid,
						data,
						MACROMGR_GET_RAPIDPDELAY( sub ),
						MACROMGR_GET_RAPIDRDELAY( sub )
					);
				}
				break;
			case MACROMGR_RAPIDFIRE_STOP:
				if( work->temp->trace.rfUid ) mfRapidfireClear( work->temp->trace.rfUid, data );
				break;
			default:
				continue;
		}
		
		if( work->temp->trace.rfUid ) mfRapidfireExec( work->temp->trace.rfUid, MF_UPDATE, pad );
		
		/* ボタンを更新 */
		pad->Buttons |= work->temp->common.lastButtons;
		
		/*
			最後にマクロで指定されたアナログスティックの座標が実座標よりも動いていれば上書き
			(デッドゾーンから出ていなければ上書きの方がいい？)
		*/
		if( abs( work->temp->common.analogX - MACROMGR_ANALOG_CENTER ) > abs( pad->Lx - MACROMGR_ANALOG_CENTER ) )
			pad->Lx = work->temp->common.analogX;
		
		if( abs( work->temp->common.analogY - MACROMGR_ANALOG_CENTER ) > abs( pad->Ly - MACROMGR_ANALOG_CENTER ) )
			pad->Ly = work->temp->common.analogY;
		
		/* コマンド実行を一つ完了したので抜ける */
		break;
	} while( ( work->temp->cmd = macromgrNext( work->temp->cmd ) ) );
	
	/* コマンドを次に進める */
	if( work->temp->cmd && action != MACROMGR_DELAY ){
		work->temp->cmd = macromgrNext( work->temp->cmd );
		sceRtcGetCurrentTick( &(work->temp->common.rtc) );
	}
	
	/* 全コマンド実行終了したかどうか */
	if( work->temp->cmd ){
		return true;
	} else if( ! work->temp->trace.loopRest || ( work->temp->trace.loopRest > 1 ) ){
		if( work->temp->trace.loopRest ) work->temp->trace.loopRest--;
		work->temp->common.lastButtons = 0;
		work->temp->common.analogX     = MACROMGR_ANALOG_CENTER;
		work->temp->common.analogY     = MACROMGR_ANALOG_CENTER;
		work->temp->cmd = macromgrSeek( work->actionData->macro, 0, MACROMGR_SEEK_SET, NULL );
		sceRtcGetCurrentTick( &(work->temp->common.rtc) );
		return true;
	} else{
		return false;
	}
}

static void macro_mfengine_warning( void )
{
	mfMenuError( CG_OK, MF_STR_MACRO_ENGINE_IS_OFF );
	mfMenuProc( NULL );
}
