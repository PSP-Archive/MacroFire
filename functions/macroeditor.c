/*=========================================================

	macroeditor.c

	マクロエディタ。

=========================================================*/
#include "macroeditor.h"

/*=========================================================
	マクロ
=========================================================*/
#define MACROEDITOR_SIG_UPDATE_CMD MF_SM_1

#define MACROEDITOR_TEMP_ANALOGMOVE_X 0
#define MACROEDITOR_TEMP_ANALOGMOVE_Y 1

#define MACROEDITOR_TEMP_RAPIDFIRESTART_PD  0
#define MACROEDITOR_TEMP_RAPIDFIRESTART_RD  1

/*=========================================================
	型宣言
=========================================================*/
struct macroeditor_command_data {
	MacromgrCommand *cmd;
	MacromgrAction  action;
	uint64_t        data, sub;
	
	int temp;
};

struct macroeditor_params {
	MacromgrUID macro;
	
	unsigned int numberOfCmd;
	unsigned int selectedPos;
	
	struct macroeditor_command_data selected;
	
	struct {
		MfMenuTable *tables;
		unsigned int count;
	} menu;
	
	struct {
		MfCtrlDefGetNumberPref  delay;
		MfCtrlDefGetNumberPref  coord;
		MfCtrlDefGetNumberPref  rapiddelay;
		MfCtrlDefGetButtonsPref buttons;
	} dialogPref;
};

/*=========================================================
	ローカル関数
=========================================================*/
static int macroeditor_ctrl_edit_analog_x( MfMessage message, const char *label, void *var, void *pref, void *ex );
static int macroeditor_ctrl_edit_analog_y( MfMessage message, const char *label, void *var, void *pref, void *ex );
static int macroeditor_ctrl_edit_rapid_pd( MfMessage message, const char *label, void *var, void *pref, void *ex );
static int macroeditor_ctrl_edit_rapid_rd( MfMessage message, const char *label, void *var, void *pref, void *ex );
static int macroeditor_ctrl_set_buttons( MfMessage message, const char *label, void *var, void *pref, void *ex );
static int macroeditor_ctrl_set_type( MfMessage message, const char *label, void *var, void *pref, void *ex );

/*=========================================================
	ローカル変数
=========================================================*/
static struct macroeditor_params *st_params;

/*=========================================================
	関数
=========================================================*/
bool macroeditorInit( MacromgrUID uid )
{
	MacromgrCommand *cmd;
	
	if( ! uid ) return false;
	
	st_params = mfMenuMemoryAlloc( sizeof( struct macroeditor_params ) );
	if( ! st_params ) return false;
	
	st_params->macro       = uid;
	st_params->numberOfCmd = 0;
	st_params->selectedPos = 0;
	if( ( cmd = macromgrSeek( st_params->macro, 0, MACROMGR_SEEK_SET, NULL ) ) ){
		while( ( cmd = macromgrNext( cmd ) ) ){
			st_params->numberOfCmd++;
		}
	} else{
		// マクロコマンドが1つもない
		macroeditorTerm();
		return false;
	}
	
	/* 選択中のマクロ情報 */
	st_params->selected.cmd    = NULL;
	st_params->selected.action = MACROMGR_DELAY;
	st_params->selected.data   = 0;
	st_params->selected.sub    = 0;
	st_params->selected.temp   = 0;
	
	/* メニュー設定 */
	st_params->menu.tables = NULL;
	st_params->menu.count  = 0;
	
	/* ダイアログ設定 */
	st_params->dialogPref.delay.unit  = "ms";
	st_params->dialogPref.delay.max   = 999999999;
	st_params->dialogPref.delay.min   = 0;
	st_params->dialogPref.delay.width = 1;
	
	st_params->dialogPref.coord.unit  = "";
	st_params->dialogPref.coord.max   = 255;
	st_params->dialogPref.coord.min   = 0;
	st_params->dialogPref.coord.width = 1;
	
	st_params->dialogPref.rapiddelay.unit  = "ms";
	st_params->dialogPref.rapiddelay.max   = 999;
	st_params->dialogPref.rapiddelay.min   = 0;
	st_params->dialogPref.rapiddelay.width = 17;
	
	st_params->dialogPref.buttons.availButtons = MF_TARGET_BUTTONS;
	
	return true;
}

void macroeditorTerm( void )
{
	if( ! st_params ) return;
	
	memoryFree( st_params );
	st_params = NULL;
}

#define PRINT_CMD( xoff, yoff, color, str )       pbPrint( pbOffsetChar( 3 + xoff ), pbOffsetLine( 4 + yoff ), color, MF_COLOR_TEXT_BG, str )
#define PRINTF_CMD( xoff, yoff, color, str, ... ) pbPrintf( pbOffsetChar( 3 + xoff ), pbOffsetLine( 4 + yoff ), color, MF_COLOR_TEXT_BG, str, __VA_ARGS__ )
bool macroeditorMain( MfMessage message ) /* メニューメッセージが来る */
{
	MacromgrCommand *cmd;
	MacromgrAction  action;
	uint64_t data, sub;
	
	unsigned int line_number = 0;
	unsigned int line_coord = 0;
	unsigned int rest_lines, offset;
	
	unsigned int text_color, guide_color;
	
	bool nowedit = message & MF_MM_WAIT || st_params->menu.tables ? true : false;
	
	/* マクロコマンドの更新シグナルかメニュー復帰があれば更新 */
	if( message & ( MF_MM_CONTINUE | MACROEDITOR_SIG_UPDATE_CMD ) ) macromgrSetCommand( st_params->selected.cmd, st_params->selected.action, st_params->selected.data, st_params->selected.sub );
	
	rest_lines  = mfMenuScroll( st_params->selectedPos, MACROEDITOR_LINES_PER_PAGE, st_params->numberOfCmd );
	line_number = rest_lines + 1;
	
	cmd = macromgrSeek( st_params->macro, 0, MACROMGR_SEEK_SET, NULL );
	while( rest_lines-- ) cmd = macromgrNext( cmd );
	
	for( rest_lines = MACROEDITOR_LINES_PER_PAGE; cmd && rest_lines; cmd = macromgrNext( cmd ), line_number++, line_coord++, rest_lines-- ){
		char buttons[64];
		
		macromgrGetCommand( cmd, &action, &data, &sub );
		
		if( line_number - 1 == st_params->selectedPos ){
			text_color = MF_COLOR_TEXT_FC;
			if( ! nowedit ){
				st_params->selected.cmd    = cmd;
				st_params->selected.action = action;
				st_params->selected.data   = data;
				st_params->selected.sub    = sub;
			}
		} else{
			text_color = MF_COLOR_TEXT_FG;
		}
		guide_color = ( 0x00ffffff & text_color ) | 0x44000000;
		offset      = PRINTF_CMD( 0, line_coord, text_color, "%4d: ", line_number );
		
		/*
			Delay ------------>
			Buttons press ---->
			Buttons release -->
			Buttons change --->
			Analog move(x,y) ->
			Rapidfire start -->
			Rapidfire stop --->
		*/
		switch( action ){
			case MACROMGR_DELAY:
				offset += PRINT_CMD( offset, line_coord, text_color, "Delay" );
				offset += PRINT_CMD( offset, line_coord, guide_color, " ------------> " );
				/* USE_KERNEL_LIBC だと %llu がなぜか常に0 */
				PRINTF_CMD( offset, line_coord, text_color, "%u %s", (unsigned int)data, st_params->dialogPref.delay.unit );
				break;
			case MACROMGR_BUTTONS_PRESS:
				offset += PRINT_CMD( offset, line_coord, text_color, "Buttons press" );
				offset += PRINT_CMD( offset, line_coord, guide_color, " ----> " );
				PRINTF_CMD( offset, line_coord, text_color, "%s", mfMenuButtonsSymbolList( data, buttons, sizeof( buttons ) ) );
				break;
			case MACROMGR_BUTTONS_RELEASE:
				offset += PRINT_CMD( offset, line_coord, text_color, "Buttons release" );
				offset += PRINT_CMD( offset, line_coord, guide_color, " --> " );
				PRINTF_CMD( offset, line_coord, text_color, "%s", mfMenuButtonsSymbolList( data, buttons, sizeof( buttons ) ) );
				break;
			case MACROMGR_BUTTONS_CHANGE:
				offset += PRINT_CMD( offset, line_coord, text_color, "Buttons change" );
				offset += PRINT_CMD( offset, line_coord, guide_color, " ---> " );
				PRINTF_CMD( offset, line_coord, text_color, "%s", mfMenuButtonsSymbolList( data, buttons, sizeof( buttons ) ) );
				break;
			case MACROMGR_ANALOG_MOVE:
				offset += PRINT_CMD( offset, line_coord, text_color, "Analog move(x,y)" );
				offset += PRINT_CMD( offset, line_coord, guide_color, " -> " );
				PRINTF_CMD( offset, line_coord, text_color, "%d, %d", (int)MACROMGR_GET_ANALOG_X( data ), (int)MACROMGR_GET_ANALOG_Y( data ) );
				break;
			case MACROMGR_RAPIDFIRE_START:
				offset += PRINT_CMD( offset, line_coord, text_color, "Rapidfire start" );
				offset += PRINT_CMD( offset, line_coord, guide_color, " --> " );
				PRINTF_CMD( offset, line_coord, text_color, "PD=%u/RD=%u %s", (int)MACROMGR_GET_RAPIDPDELAY( sub ), (int)MACROMGR_GET_RAPIDRDELAY( sub ), mfMenuButtonsSymbolList( data, buttons, sizeof( buttons ) ) );
				break;
			case MACROMGR_RAPIDFIRE_STOP:
				offset += PRINT_CMD( offset, line_coord, text_color, "Rapidfire stop" );
				offset += PRINT_CMD( offset, line_coord, guide_color, " --> " );
				PRINTF_CMD( offset, line_coord, text_color, "%s", mfMenuButtonsSymbolList( data, buttons, sizeof( buttons ) ) );
				break;
		}
	}
	
	if( nowedit ){
		if( st_params->menu.tables ){
			if( mfMenuDrawTable( st_params->menu.tables, MF_MENU_NO_OPTIONS ) != MF_MENU_OK ){
				mfMenuDestroyTables( st_params->menu.tables );
				st_params->menu.tables = NULL;
			} else{
				return true;
			}
		}
	} else{
		mfMenuSetInfoText( MF_MENU_INFOTEXT_COMMON_CTRL | MF_MENU_INFOTEXT_MOVABLEPAGE_CTRL | MF_MENU_INFOTEXT_SET_LOWER_LINE, "(%s)%s, (%s)%s, (%s)%s (L)%s, (R)%s", mfMenuAcceptSymbol(), MF_STR_MACROEDITOR_CTRL_EDIT, PB_SYM_PSP_TRIANGLE, MF_STR_MACROEDITOR_CTRL_CHANGE, PB_SYM_PSP_SQUARE, MF_STR_MACROEDITOR_CTRL_DELETE, MF_STR_MACROEDITOR_CTRL_INS_BEFORE, MF_STR_MACROEDITOR_CTRL_INS_AFTER );
		if( mfMenuIsPressed( mfMenuCancelButton() ) ){
			return false;
		} else if( mfMenuIsPressed( mfMenuAcceptButton() ) ) {
			switch( st_params->selected.action ){
				case MACROMGR_DELAY:
					mfDialogNumedit( "Delay", st_params->dialogPref.delay.unit,  &(st_params->selected.data), st_params->dialogPref.delay.max );
					break;
				case MACROMGR_BUTTONS_PRESS:
					mfDialogDetectbuttons( "Buttons press", st_params->dialogPref.buttons.availButtons, (PadutilButtons *)&(st_params->selected.data) );
					break;
				case MACROMGR_BUTTONS_RELEASE:
					mfDialogDetectbuttons( "Buttons release", st_params->dialogPref.buttons.availButtons, (PadutilButtons *)&(st_params->selected.data) );
					break;
				case MACROMGR_BUTTONS_CHANGE:
					mfDialogDetectbuttons( "Buttons change", st_params->dialogPref.buttons.availButtons, (PadutilButtons *)&(st_params->selected.data) );
					break;
				case MACROMGR_ANALOG_MOVE:
					st_params->menu.tables = mfMenuCreateTables( 1, 2, 1 );
					mfMenuSetTablePosition( st_params->menu.tables, 1, pbOffsetChar( 40 ), pbOffsetLine( 26 ) );
					mfMenuSetTableLabel( st_params->menu.tables, 1, "Analog move" );
					mfMenuSetTableEntry( st_params->menu.tables, 1, 1, 1, MF_STR_MACROEDITOR_CMD_ANALOG_MOVE_EDIT_X, macroeditor_ctrl_edit_analog_x, &(st_params->selected), &(st_params->dialogPref.coord) );
					mfMenuSetTableEntry( st_params->menu.tables, 1, 2, 1, MF_STR_MACROEDITOR_CMD_ANALOG_MOVE_EDIT_Y, macroeditor_ctrl_edit_analog_y, &(st_params->selected), &(st_params->dialogPref.coord) );
					mfMenuInitTable( st_params->menu.tables );
					break;
				case MACROMGR_RAPIDFIRE_START:
					st_params->menu.tables = mfMenuCreateTables( 1, 3, 1 );
					mfMenuSetTablePosition( st_params->menu.tables, 1, pbOffsetChar( 40 ), pbOffsetLine( 25 ) );
					mfMenuSetTableLabel( st_params->menu.tables, 1, "Rapidfire start" );
					mfMenuSetTableEntry( st_params->menu.tables, 1, 1, 1, MF_STR_MACROEDITOR_CMD_RAPIDFIRE_START_EDIT_BUTTON, macroeditor_ctrl_set_buttons, &(st_params->selected.data), &(st_params->dialogPref.buttons) );
					mfMenuSetTableEntry( st_params->menu.tables, 1, 2, 1, MF_STR_MACROEDITOR_CMD_RAPIDFIRE_START_EDIT_PD,     macroeditor_ctrl_edit_rapid_pd, &(st_params->selected), &(st_params->dialogPref.rapiddelay) );
					mfMenuSetTableEntry( st_params->menu.tables, 1, 3, 1, MF_STR_MACROEDITOR_CMD_RAPIDFIRE_START_EDIT_RD,     macroeditor_ctrl_edit_rapid_rd, &(st_params->selected), &(st_params->dialogPref.rapiddelay) );
					mfMenuInitTable( st_params->menu.tables );
					break;
				case MACROMGR_RAPIDFIRE_STOP:
					mfDialogDetectbuttons( "Rapidfire stop", st_params->dialogPref.buttons.availButtons, (PadutilButtons *)&(st_params->selected.data) );
					break;
			}
		} else if( mfMenuIsPressed( PSP_CTRL_SQUARE ) ){
			if( ! st_params->numberOfCmd ){
				macromgrSetDelay( st_params->selected.cmd, 0 );
			} else if( macromgrRemove( st_params->macro, st_params->selected.cmd ) ){
				st_params->numberOfCmd--;
				if( st_params->numberOfCmd < st_params->selectedPos ){
					st_params->selectedPos = st_params->numberOfCmd;
				}
			}
		} else if( mfMenuIsPressed( PSP_CTRL_TRIANGLE ) ){
			st_params->menu.tables = mfMenuCreateTables( 1, 7, 1 );
			mfMenuSetTablePosition( st_params->menu.tables, 1, pbOffsetChar( 40 ), pbOffsetLine( 21 ) );
			mfMenuSetTableLabel( st_params->menu.tables, 1, MF_STR_MACROEDITOR_CTRL_CHANGE );
			mfMenuSetTableEntry( st_params->menu.tables, 1, 1, 1, "Delay",           macroeditor_ctrl_set_type, &(st_params->selected), (void *)MACROMGR_DELAY );
			mfMenuSetTableEntry( st_params->menu.tables, 1, 2, 1, "Buttons press",   macroeditor_ctrl_set_type, &(st_params->selected), (void *)MACROMGR_BUTTONS_PRESS );
			mfMenuSetTableEntry( st_params->menu.tables, 1, 3, 1, "Buttons release", macroeditor_ctrl_set_type, &(st_params->selected), (void *)MACROMGR_BUTTONS_RELEASE );
			mfMenuSetTableEntry( st_params->menu.tables, 1, 4, 1, "Buttons change",  macroeditor_ctrl_set_type, &(st_params->selected), (void *)MACROMGR_BUTTONS_CHANGE );
			mfMenuSetTableEntry( st_params->menu.tables, 1, 5, 1, "Analog move",     macroeditor_ctrl_set_type, &(st_params->selected), (void *)MACROMGR_ANALOG_MOVE );
			mfMenuSetTableEntry( st_params->menu.tables, 1, 6, 1, "Rapidfire start", macroeditor_ctrl_set_type, &(st_params->selected), (void *)MACROMGR_RAPIDFIRE_START );
			mfMenuSetTableEntry( st_params->menu.tables, 1, 7, 1, "Rapidfire stop",  macroeditor_ctrl_set_type, &(st_params->selected), (void *)MACROMGR_RAPIDFIRE_STOP );
			mfMenuInitTable( st_params->menu.tables );
		} else if( mfMenuIsPressed( PSP_CTRL_LTRIGGER ) ){
			if( macromgrInsertBefore( st_params->macro, st_params->selected.cmd ) ){
				st_params->numberOfCmd++;
			}
		} else if( mfMenuIsPressed( PSP_CTRL_RTRIGGER ) ){
			if( macromgrInsertAfter( st_params->macro, st_params->selected.cmd ) ){
				st_params->numberOfCmd++;
				st_params->selectedPos++;
			}
		} else if( mfMenuIsPressed( PSP_CTRL_UP ) ){
			if( ! st_params->selectedPos ){
				st_params->selectedPos = st_params->numberOfCmd;
			} else{
				st_params->selectedPos--;
			}
		} else if( mfMenuIsPressed( PSP_CTRL_DOWN ) ){
			if( st_params->selectedPos >= st_params->numberOfCmd ){
				st_params->selectedPos = 0;
			} else{
				st_params->selectedPos++;
			}
		} else if( mfMenuIsPressed( PSP_CTRL_LEFT ) ){
			if( st_params->selectedPos  <= ( MACROEDITOR_LINES_PER_PAGE >> 1 ) ){
				st_params->selectedPos = 0;
			} else{
				st_params->selectedPos -= ( MACROEDITOR_LINES_PER_PAGE >> 1 );
			}
		} else if( mfMenuIsPressed( PSP_CTRL_RIGHT ) ){
			if( st_params->numberOfCmd - st_params->selectedPos <= ( MACROEDITOR_LINES_PER_PAGE >> 1 ) ){
				st_params->selectedPos = st_params->numberOfCmd;
			} else{
				st_params->selectedPos += ( MACROEDITOR_LINES_PER_PAGE >> 1 );
			}
		}
	}
	return true;
}

/*-----------------------------------------------
	コントロール
-----------------------------------------------*/
static int macroeditor_ctrl_edit_analog_x( MfMessage message, const char *label, void *var, void *pref, void *ex )
{
	if( message & MF_CM_INFO ){
		mfMenuSetInfoText( MF_MENU_INFOTEXT_COMMON_CTRL | MF_MENU_INFOTEXT_SET_LOWER_LINE, "(%s)%s", mfMenuAcceptSymbol(), MF_STR_MACROEDITOR_CTRL_EDIT );
	} else if( message & MF_CM_PROC ){
		 if( mfMenuIsPressed( mfMenuAcceptButton() ) ){
			MfCtrlDefGetNumberPref *numpref = pref;
			((struct macroeditor_command_data *)var)->temp = MACROMGR_GET_ANALOG_X( ((struct macroeditor_command_data *)var)->data );
			mfDialogNumedit( label, numpref->unit, &(((struct macroeditor_command_data *)var)->temp), numpref->max );
		}
	} else if( message & MF_CM_CONTINUE ){
		dbgprintf( "%X: %d, %d", message, ((struct macroeditor_command_data *)var)->temp, MACROMGR_GET_ANALOG_Y( ((struct macroeditor_command_data *)var)->data ) );
		((struct macroeditor_command_data *)var)->data = MACROMGR_SET_ANALOG_XY( ((struct macroeditor_command_data *)var)->temp, MACROMGR_GET_ANALOG_Y( ((struct macroeditor_command_data *)var)->data ) );
		mfMenuSendSignal( MACROEDITOR_SIG_UPDATE_CMD );
		mfMenuBack();
	}
	return CG_OK;
}

static int macroeditor_ctrl_edit_analog_y( MfMessage message, const char *label, void *var, void *pref, void *ex )
{
	if( message & MF_CM_INFO ){
		mfMenuSetInfoText( MF_MENU_INFOTEXT_COMMON_CTRL | MF_MENU_INFOTEXT_SET_LOWER_LINE, "(%s)%s", mfMenuAcceptSymbol(), MF_STR_MACROEDITOR_CTRL_EDIT );
	} else if( message & MF_CM_PROC ){
		 if( mfMenuIsPressed( mfMenuAcceptButton() ) ){
			MfCtrlDefGetNumberPref *numpref = pref;
			((struct macroeditor_command_data *)var)->temp = MACROMGR_GET_ANALOG_Y( ((struct macroeditor_command_data *)var)->data );
			mfDialogNumedit( label, numpref->unit, &(((struct macroeditor_command_data *)var)->temp), numpref->max );
		}
	} else if( message & MF_CM_CONTINUE ){
		((struct macroeditor_command_data *)var)->data = MACROMGR_SET_ANALOG_XY( MACROMGR_GET_ANALOG_X( ((struct macroeditor_command_data *)var)->data ), ((struct macroeditor_command_data *)var)->temp );
		mfMenuSendSignal( MACROEDITOR_SIG_UPDATE_CMD );
		mfMenuBack();
	}
	return CG_OK;
}


static int macroeditor_ctrl_edit_rapid_pd( MfMessage message, const char *label, void *var, void *pref, void *ex )
{
	if( message & MF_CM_INFO ){
		mfMenuSetInfoText( MF_MENU_INFOTEXT_COMMON_CTRL | MF_MENU_INFOTEXT_SET_LOWER_LINE, "(%s)%s", mfMenuAcceptSymbol(), MF_STR_MACROEDITOR_CTRL_EDIT );
	} else if( message & MF_CM_PROC ){
		if( mfMenuIsPressed( mfMenuAcceptButton() ) ){
			MfCtrlDefGetNumberPref *numpref = pref;
			((struct macroeditor_command_data *)var)->temp = MACROMGR_GET_RAPIDPDELAY( ((struct macroeditor_command_data *)var)->sub );
			mfDialogNumedit( label, numpref->unit, &(((struct macroeditor_command_data *)var)->temp), numpref->max );
		}
	} else if( message & MF_CM_CONTINUE ){
		((struct macroeditor_command_data *)var)->sub = MACROMGR_SET_RAPIDDELAY( ((struct macroeditor_command_data *)var)->temp, MACROMGR_GET_RAPIDRDELAY( ((struct macroeditor_command_data *)var)->sub ) );
		mfMenuSendSignal( MACROEDITOR_SIG_UPDATE_CMD );
		mfMenuBack();
	}
	return CG_OK;
}


static int macroeditor_ctrl_edit_rapid_rd( MfMessage message, const char *label, void *var, void *pref, void *ex )
{
	if( message & MF_CM_INFO ){
		mfMenuSetInfoText( MF_MENU_INFOTEXT_COMMON_CTRL | MF_MENU_INFOTEXT_SET_LOWER_LINE, "(%s)%s", mfMenuAcceptSymbol(), MF_STR_MACROEDITOR_CTRL_EDIT );
	} else if( message & MF_CM_PROC ){
		if( mfMenuIsPressed( mfMenuAcceptButton() ) ){
			MfCtrlDefGetNumberPref *numpref = pref;
			((struct macroeditor_command_data *)var)->temp = MACROMGR_GET_RAPIDRDELAY( ((struct macroeditor_command_data *)var)->sub );
			mfDialogNumedit( label, numpref->unit, &(((struct macroeditor_command_data *)var)->temp), numpref->max );
		}
	} else if( message & MF_CM_CONTINUE ){
		((struct macroeditor_command_data *)var)->sub = MACROMGR_SET_RAPIDDELAY( MACROMGR_GET_RAPIDPDELAY( ((struct macroeditor_command_data *)var)->sub ), ((struct macroeditor_command_data *)var)->temp );
		mfMenuSendSignal( MACROEDITOR_SIG_UPDATE_CMD );
		mfMenuBack();
	}
	return CG_OK;
}

static int macroeditor_ctrl_set_buttons( MfMessage message, const char *label, void *var, void *pref, void *ex )
{
	if( message & MF_CM_CONTINUE ){
		mfMenuSendSignal( MACROEDITOR_SIG_UPDATE_CMD );
		mfMenuBack();
		return CG_OK;
	} else{
		return mfCtrlDefGetButtons( message, label, var, pref, ex );
	}
}

static int macroeditor_ctrl_set_type( MfMessage message, const char *label, void *var, void *pref, void *ex )
{
	if( message & MF_CM_INFO ){
		mfMenuSetInfoText( MF_MENU_INFOTEXT_COMMON_CTRL | MF_MENU_INFOTEXT_SET_LOWER_LINE, "(%s)%s", mfMenuAcceptSymbol(), MF_STR_MACROEDITOR_CTRL_CHANGE );
	} else if( message & MF_CM_PROC ){
		if( mfMenuIsPressed( mfMenuAcceptButton() ) ){
			if( ((struct macroeditor_command_data *)var)->action != (MacromgrAction)pref ){
				((struct macroeditor_command_data *)var)->action = (MacromgrAction)pref;
				switch( ((struct macroeditor_command_data *)var)->action ){
					case MACROMGR_ANALOG_MOVE:
						((struct macroeditor_command_data *)var)->data = MACROMGR_ANALOG_NEUTRAL;
						((struct macroeditor_command_data *)var)->sub  = 0;
						break;
					default:
						((struct macroeditor_command_data *)var)->data = 0;
						((struct macroeditor_command_data *)var)->sub  = 0;
				}
				mfMenuSendSignal( MACROEDITOR_SIG_UPDATE_CMD );
			}
			mfMenuBack();
		}
	}
	return CG_OK;
}
