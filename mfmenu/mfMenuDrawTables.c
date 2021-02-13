/* mfMenuDrawTables.c */

#include "mfmenu_types.h"

static bool mf_menu_move_table( enum MfMenuMoveDirection dir, MfMenuTable *menu, unsigned int menucnt, struct MfMenuPosition *pos );
static bool mf_menu_nearest_table( unsigned int pri, unsigned int sec, unsigned int *save_pri, unsigned int *save_sec );
static bool mf_menu_farest_table( unsigned int pri, unsigned int sec, unsigned int *save_pri, unsigned int *save_sec );
static bool mf_menu_select_table_horizontal( MfMenuTable *menu, unsigned int targ_tid, struct MfMenuPosition *pos, bool reverse );
static bool mf_menu_select_table_vertical( MfMenuTable *menu, unsigned int targ_tid, struct MfMenuPosition *pos, bool reverse );
static unsigned int mf_label_width( char *str );

int mfMenuDrawTables( MfMenuTable menu[], unsigned int cnt, unsigned int opt )
{
	/* 選択可能な項目(コントロールが設定されていてActivateされている)が一つもないと無限ループ */
	int ret = CG_OK;
	
	unsigned int i, row, col;
	unsigned int col_width, width, col_offset, row_offset;
	
	struct MfMenuPosition dupe_pos;
	
	bool refind = true;
	
	/* ラベルを描画 */
	for( i = 0; i < cnt; i++ ){
		if( menu[i].label ){
			col_offset = pbOffsetChar( 2 );
			row_offset = pbOffsetLine( 1 ) + 2;
			pbPrint( menu[i].x, menu[i].y,  MF_COLOR_TEXT_LABEL, MF_COLOR_TEXT_BG, menu[i].label );
			pbLine( menu[i].x + pbMeasureString( menu[i].label ) + ( pbOffsetChar( 1 ) >> 1 ), menu[i].y + ( pbOffsetLine( 1 ) >>1 ), SCR_WIDTH - pbOffsetChar( 6 ), menu[i].y + ( pbOffsetLine( 1 ) >>1 ), MF_COLOR_TEXT_LABEL );
		} else{
			col_offset = 0;
			row_offset = 0;
		}
		
		for( col = 0, col_width = 0; col < menu[i].cols; col++ ){
			if( col && menu[i].colsWidth && menu[i].colsWidth[col - 1] > 0 ){
				col_width =  menu[i].colsWidth[col - 1];
			} else if( col_width ){
				col_width += pbOffsetChar( 2 );
			}
			col_offset += col_width;
			for( row = 0; row < menu[i].rows; row++ ){
				char labelbuf[MF_CTRL_BUFFER_LENGTH] = "\0";
				if( menu[i].entry[row][col].ctrl ){
					( menu[i].entry[row][col].ctrl )(
						MF_CM_LABEL | __mfmenu__gParams->Message.Sub,
						menu[i].entry[row][col].label,
						menu[i].entry[row][col].var,
						menu[i].entry[row][col].arg,
						labelbuf
					);
				}
				if( labelbuf[0] == '\0' ) strutilCopy( labelbuf, menu[i].entry[row][col].label, sizeof( labelbuf ) );
				
				if( labelbuf[0] != '\0' ){
					pbPrint(
						menu[i].x + col_offset,
						menu[i].y + row_offset + pbOffsetLine( row ),
						(
							i == __mfmenu__gParams->Menu.CurTable && row == __mfmenu__gParams->Menu.CurRow && col == __mfmenu__gParams->Menu.CurCol ? MF_COLOR_TEXT_FC :
							menu[i].entry[row][col].active ? MF_COLOR_TEXT_FG :
							                                 MF_COLOR_TEXT_INACT
						),
						MF_COLOR_TEXT_BG,
						labelbuf
					);
					
					width = mf_label_width( labelbuf );
					if( col_width < width ) col_width = width;
				}
			}
		}
	}
	
	if( ! ( opt & MF_MENU_DISPLAY_ONLY ) ){
		MfCtrlMessage ctrlmsg;
		
		if( __mfmenu__gParams->Extra.Proc ){
			/* 拡張関数が設定されていれば実行 */
			int extraret = ( __mfmenu__gParams->Extra.Proc )( MF_MM_PROC | __mfmenu__gParams->Message.Sub, __mfmenu__gParams->Extra.Argp );
			if( CG_IS_ERROR( extraret ) ){
				mfMenuExitExtra();
				mfMenuError( extraret, NULL );
			}
			ctrlmsg = ( __mfmenu__gParams->Extra.Proc ? MF_CM_WAIT : MF_CM_CONTINUE );
		} else if( mfMenuMaskDialogMessage( __mfmenu__gParams->Message.Sub ) ){
			ctrlmsg = ( ! ( __mfmenu__gParams->Message.Sub & MF_DM_FINISH ) ? MF_CM_WAIT : MF_CM_CONTINUE );
		} else{
			ctrlmsg = MF_CM_PROC;
		}
		
		if( menu[__mfmenu__gParams->Menu.CurTable].entry[__mfmenu__gParams->Menu.CurRow][__mfmenu__gParams->Menu.CurCol].active ){
			if( menu[__mfmenu__gParams->Menu.CurTable].entry[__mfmenu__gParams->Menu.CurRow][__mfmenu__gParams->Menu.CurCol].ctrl ){
				ret = ( menu[__mfmenu__gParams->Menu.CurTable].entry[__mfmenu__gParams->Menu.CurRow][__mfmenu__gParams->Menu.CurCol].ctrl )(
					ctrlmsg | __mfmenu__gParams->Message.Sub,
					menu[__mfmenu__gParams->Menu.CurTable].entry[__mfmenu__gParams->Menu.CurRow][__mfmenu__gParams->Menu.CurCol].label,
					menu[__mfmenu__gParams->Menu.CurTable].entry[__mfmenu__gParams->Menu.CurRow][__mfmenu__gParams->Menu.CurCol].var,
					menu[__mfmenu__gParams->Menu.CurTable].entry[__mfmenu__gParams->Menu.CurRow][__mfmenu__gParams->Menu.CurCol].arg,
					NULL
				);
			}
			if( __mfmenu__gParams->Message.Sub & ( MF_DM_START | MF_DM_UPDATE ) ) return ret;
		}
	} else{
		return ret;
	}
	
	/* 拡張関数が設定されていればここまで */
	if( __mfmenu__gParams->Extra.Proc ) return ret;
	
	/* BACKの検出 */
	if( mfMenuIsPressed( mfMenuCancelButton() ) || ( __mfmenu__gParams->Message.Main & MF_MENU_IM_BACK ) ) return MF_MENU_BACK;
	
	/* 情報バーのデータを取得 */
	if( __mfmenu__gParams->Info.Text[0] == '\0' ){
		unsigned int opt = menu[__mfmenu__gParams->Menu.CurTable].cols == 1 ? 0 : MF_MENU_INFOTEXT_MULTICOLUMN_CTRL;
		
		if( menu[__mfmenu__gParams->Menu.CurTable].entry[__mfmenu__gParams->Menu.CurRow][__mfmenu__gParams->Menu.CurCol].active ){
			( menu[__mfmenu__gParams->Menu.CurTable].entry[__mfmenu__gParams->Menu.CurRow][__mfmenu__gParams->Menu.CurCol].ctrl )(
				MF_CM_INFO | __mfmenu__gParams->Message.Sub,
				menu[__mfmenu__gParams->Menu.CurTable].entry[__mfmenu__gParams->Menu.CurRow][__mfmenu__gParams->Menu.CurCol].label,
				menu[__mfmenu__gParams->Menu.CurTable].entry[__mfmenu__gParams->Menu.CurRow][__mfmenu__gParams->Menu.CurCol].var,
				menu[__mfmenu__gParams->Menu.CurTable].entry[__mfmenu__gParams->Menu.CurRow][__mfmenu__gParams->Menu.CurCol].arg,
				&opt
			);
		}
		if( __mfmenu__gParams->Info.Text[0] == '\0' ) mfMenuSetInfoText( MF_MENU_INFOTEXT_COMMON_CTRL, "" );
	}
	
	/* 現在の位置をバックアップ */
	dupe_pos = __mfmenu__gParams->Menu;
	
	/* 操作 */
	while( refind ){
		refind = false;
		
		if( mfMenuIsPressed( PSP_CTRL_UP ) ){
			if( __mfmenu__gParams->Menu.CurRow ){
				__mfmenu__gParams->Menu.CurRow--;
			} else if( ! mf_menu_move_table( MF_MENU_MOVE_UP, menu, cnt, &(__mfmenu__gParams->Menu) ) ){
				refind = true;
				break;
			}
		} else if( mfMenuIsPressed( PSP_CTRL_DOWN ) ){
			if( __mfmenu__gParams->Menu.CurRow < menu[__mfmenu__gParams->Menu.CurTable].rows - 1 ){
				__mfmenu__gParams->Menu.CurRow++;
			} else if( ! mf_menu_move_table( MF_MENU_MOVE_DOWN, menu, cnt, &(__mfmenu__gParams->Menu) ) ){
				refind = true;
				break;
			}
		} else if( mfMenuIsPressed( PSP_CTRL_LEFT ) ){
			if( __mfmenu__gParams->Menu.CurCol ){
				__mfmenu__gParams->Menu.CurCol--;
			} else if( ! mf_menu_move_table( MF_MENU_MOVE_LEFT, menu, cnt, &(__mfmenu__gParams->Menu) ) ){
				refind = true;
				break;
			}
		} else if( mfMenuIsPressed( PSP_CTRL_RIGHT ) ){
			if( __mfmenu__gParams->Menu.CurCol < menu[__mfmenu__gParams->Menu.CurTable].cols - 1 ){
				__mfmenu__gParams->Menu.CurCol++;
			} else if( ! mf_menu_move_table( MF_MENU_MOVE_RIGHT, menu, cnt, &(__mfmenu__gParams->Menu) ) ){
				refind = true;
				break;
			}
		}
		
		if( ! menu[__mfmenu__gParams->Menu.CurTable].entry[__mfmenu__gParams->Menu.CurRow][__mfmenu__gParams->Menu.CurCol].active ){
			int pos_pos, pos_neg;
			if( mfMenuIsAnyPressed( PSP_CTRL_UP | PSP_CTRL_DOWN ) ){
				refind = true;
				pos_pos = __mfmenu__gParams->Menu.CurCol + 1;
				pos_neg = __mfmenu__gParams->Menu.CurCol - 1;
				while( refind && ( pos_pos >= 0 || pos_neg >= 0 ) ){
					if( pos_pos >= menu[__mfmenu__gParams->Menu.CurTable].cols ) pos_pos = -1;
					
					if( pos_neg >= 0 ){
						if( menu[__mfmenu__gParams->Menu.CurTable].entry[__mfmenu__gParams->Menu.CurRow][pos_neg].active ){
							__mfmenu__gParams->Menu.CurCol = pos_neg;
							refind = false;
						}
						pos_neg--;
					}else if( pos_pos >= 0 ){
						if( menu[__mfmenu__gParams->Menu.CurTable].entry[__mfmenu__gParams->Menu.CurRow][pos_pos].active ){
							__mfmenu__gParams->Menu.CurCol = pos_pos;
							refind = false;
						}
						pos_pos++;
					}
				}
			} else if( mfMenuIsAnyPressed( PSP_CTRL_RIGHT | PSP_CTRL_LEFT ) ){
				refind = true;
				pos_pos = __mfmenu__gParams->Menu.CurRow + 1;
				pos_neg = __mfmenu__gParams->Menu.CurRow - 1;
				while( refind && ( pos_pos >= 0 || pos_neg >= 0 ) ){
					if( pos_pos >= menu[__mfmenu__gParams->Menu.CurTable].rows ) pos_pos = -1;
					
					if( pos_neg >= 0 ){
						if( menu[__mfmenu__gParams->Menu.CurTable].entry[pos_neg][__mfmenu__gParams->Menu.CurCol].active ){
							__mfmenu__gParams->Menu.CurRow = pos_neg;
							refind = false;
						}
						pos_neg--;
					}else if( pos_pos >= 0 ){
						if( menu[__mfmenu__gParams->Menu.CurTable].entry[pos_pos][__mfmenu__gParams->Menu.CurCol].active ){
							__mfmenu__gParams->Menu.CurRow = pos_pos;
							refind = false;
						}
						pos_pos++;
					}
				}
			}
		}
	}
	
	if( refind ) __mfmenu__gParams->Menu = dupe_pos;
	
	return ret;
}

static bool mf_menu_move_table( enum MfMenuMoveDirection dir, MfMenuTable *menu, unsigned int menucnt, struct MfMenuPosition *pos )
{
	unsigned int i;
	unsigned int target_table = pos->CurTable;
	unsigned int   save_diff_x  = ~0;
	unsigned int   save_diff_y  = ~0;
	
	/* 入力方向にもっとも近いテーブルを探す */
	dbgprint( "Out of menu-range. find a next table..." );
	switch( dir ){
		case MF_MENU_MOVE_UP:
			for( i = 0; i < menucnt; i++ ){
				if( menu[pos->CurTable].y <= menu[i].y ) continue;
				if( mf_menu_nearest_table( abs( menu[pos->CurTable].y - menu[i].y ), abs( menu[pos->CurTable].x - menu[i].x ), &save_diff_y, &save_diff_x ) ){
					target_table = i;
				}
			}
			if( target_table != pos->CurTable ){
				return mf_menu_select_table_vertical( menu, target_table, pos, false );
			} else{
				break;
			}
		case MF_MENU_MOVE_RIGHT:
			for( i = 0; i < menucnt; i++ ){
				if( menu[pos->CurTable].x >= menu[i].x ) continue;
				if( mf_menu_nearest_table( abs( menu[pos->CurTable].x - menu[i].x ), abs( menu[pos->CurTable].y - menu[i].y ), &save_diff_x, &save_diff_y ) ){
					target_table = i;
				}
			}
			if( target_table != pos->CurTable ){
				return mf_menu_select_table_horizontal( menu, target_table, pos, false );
			} else{
				break;
			}
		case MF_MENU_MOVE_DOWN:
			for( i = 0; i < menucnt; i++ ){
				if( menu[pos->CurTable].y >= menu[i].y ) continue;
				if( mf_menu_nearest_table( abs( menu[pos->CurTable].y - menu[i].y ), abs( menu[pos->CurTable].x - menu[i].x ), &save_diff_y, &save_diff_x ) ){
					target_table = i;
				}
			}
			if( target_table != pos->CurTable ){
				return mf_menu_select_table_vertical( menu, target_table, pos, false );
			} else{
				break;
			}
		case MF_MENU_MOVE_LEFT:
			for( i = 0; i < menucnt; i++ ){
				if( menu[pos->CurTable].x <= menu[i].x ) continue;
				if( mf_menu_nearest_table( abs( menu[pos->CurTable].x - menu[i].x ), abs( menu[pos->CurTable].y - menu[i].y ), &save_diff_x, &save_diff_y ) ){
					target_table = i;
				}
			}
			if( target_table != pos->CurTable ){
				return mf_menu_select_table_horizontal( menu, target_table, pos, false );
			} else{
				break;
			}
	}
	
	/* 見つからなければ、入力方向と逆方向のもっとも遠いテーブルを探す */
	dbgprint( "Next table not found. position wrap." );
	save_diff_x  = ~0;
	save_diff_y  = ~0;
	switch( dir ){
		case MF_MENU_MOVE_UP:
			for( i = 0; i < menucnt; i++ ){
				if( menu[pos->CurTable].y >= menu[i].y ) continue;
				if( mf_menu_farest_table( abs( menu[pos->CurTable].y - menu[i].y ), abs( menu[pos->CurTable].x - menu[i].x ), &save_diff_y, &save_diff_x ) ){
					target_table = i;
				}
			}
			return mf_menu_select_table_vertical( menu, target_table, pos, true );
		case MF_MENU_MOVE_RIGHT:
			for( i = 0; i < menucnt; i++ ){
				if( menu[pos->CurTable].x <= menu[i].x ) continue;
				if( mf_menu_farest_table( abs( menu[pos->CurTable].x - menu[i].x ), abs( menu[pos->CurTable].y - menu[i].y ), &save_diff_x, &save_diff_y ) ){
					target_table = i;
				}
			}
			return mf_menu_select_table_horizontal( menu, target_table, pos, true );
		case MF_MENU_MOVE_DOWN:
			for( i = 0; i < menucnt; i++ ){
				if( menu[pos->CurTable].y <= menu[i].y ) continue;
				if( mf_menu_farest_table( abs( menu[pos->CurTable].y - menu[i].y ), abs( menu[pos->CurTable].x - menu[i].x ), &save_diff_y, &save_diff_x ) ){
					target_table = i;
				}
			}
			return mf_menu_select_table_vertical( menu, target_table, pos, true );
		case MF_MENU_MOVE_LEFT:
			for( i = 0; i < menucnt; i++ ){
				if( menu[pos->CurTable].x >= menu[i].x ) continue;
				if( mf_menu_farest_table( abs( menu[pos->CurTable].x - menu[i].x ), abs( menu[pos->CurTable].y - menu[i].y ), &save_diff_x, &save_diff_y ) ){
					target_table = i;
				}
			}
			return mf_menu_select_table_horizontal( menu, target_table, pos, true );
	}
	
	
	return false;
}

static bool mf_menu_nearest_table( unsigned int pri, unsigned int sec, unsigned int *save_pri, unsigned int *save_sec )
{
	/*
		*save_pri, *save_sec は、全ビット1(~0)の場合のみ、特別に未定義値として扱う。
		従って、*save_pri, *save_sec に渡す変数はかならず ~0 で初期化した状態で渡す。
	*/
	
	if( *save_pri == ~0 || pri < *save_pri ){
		*save_pri = pri;
		return true;
	} else if( *save_sec == sec ){
		if( ! *save_sec || sec < *save_sec ){
			*save_sec = sec;
			return true;
		}
	}
	return false;
}

static bool mf_menu_farest_table( unsigned int pri, unsigned int sec, unsigned int *save_pri, unsigned int *save_sec )
{
	/*
		*save_pri, *save_sec は、全ビット1(~0)の場合のみ、特別に未定義値として扱う。
		従って、*save_pri, *save_sec に渡す変数はかならず ~0 で初期化した状態で渡す。
	*/
	
	if( *save_pri == ~0 || pri > *save_pri ){
		*save_pri = pri;
		return true;
	} else if( *save_sec == sec ){
		if( ! *save_sec || sec > *save_sec ){
			*save_sec = sec;
			return true;
		}
	}
	return false;
}

static bool mf_menu_select_table_horizontal( MfMenuTable *menu, unsigned int targ_tid, struct MfMenuPosition *pos, bool reverse )
{
	if( pos->CurTable == targ_tid ){
		dbgprintf( "Selected same table id: Col %d, %d", pos->CurCol, menu[targ_tid].cols );
		pos->CurCol = pos->CurCol ? 0 : menu[targ_tid].cols - 1;
	} else{
		unsigned int i, targ_row = 0, targ_col = 0;
		unsigned int current_y = menu[pos->CurTable].y + pbOffsetLine( pos->CurRow );
		unsigned int diff_y, last_diff_y = ~0;
		
		for( i = 0; i < menu[targ_tid].rows; i++ ){
			diff_y = abs( current_y - ( menu[targ_tid].y + pbOffsetLine( i ) ) );
			if( last_diff_y == ~0 || last_diff_y > diff_y ){
				last_diff_y = diff_y;
				targ_row = i;
			}
		}
		
		if( menu[pos->CurTable].x > menu[targ_tid].x ){
			targ_col = reverse ? 0 : menu[targ_tid].cols - 1;
		} else{
			targ_col = reverse ? menu[targ_tid].cols - 1 : 0;
		}
		
		pos->CurTable = targ_tid;;
		pos->CurRow   = targ_row;
		pos->CurCol   = targ_col;
	}
	
	return true;
}

static bool mf_menu_select_table_vertical( MfMenuTable *menu, unsigned int targ_tid, struct MfMenuPosition *pos, bool reverse )
{
	if( pos->CurTable == targ_tid ){
		dbgprintf( "Selected same table id: Row %d, %d", pos->CurRow, menu[targ_tid].rows );
		pos->CurRow = pos->CurRow ? 0 : menu[targ_tid].rows - 1;
	} else{
		unsigned int i, targ_row = 0, targ_col = 0;
		unsigned int current_x = menu[pos->CurTable].x;
		unsigned int comp_x, diff_x, last_diff_x = ~0;
		
		if( menu[pos->CurTable].colsWidth ){
			for( i = 0; i < pos->CurCol; i++ ){
				current_x += menu[pos->CurTable].colsWidth[i];
			}
		}
		
		comp_x = menu[targ_tid].x;
		for( i = 0; i < menu[targ_tid].cols; i++ ){
			diff_x = abs( current_x - comp_x );
			if( last_diff_x == ~0 || last_diff_x > diff_x ){
				last_diff_x = diff_x;
				targ_col = i;
			}
			if( menu[targ_tid].colsWidth ) comp_x += menu[targ_tid].colsWidth[i];
		}
		
		if( menu[pos->CurTable].y > menu[targ_tid].y ){
			targ_row = reverse ? 0 : menu[targ_tid].rows - 1;
		} else{
			targ_row = reverse ? menu[targ_tid].rows - 1 : 0;
		}
		
		pos->CurTable = targ_tid;
		pos->CurRow   = targ_row;
		pos->CurCol   = targ_col;
	}
	return true;
}

static unsigned int mf_label_width( char *str )
{
	unsigned int max_width = 0;
	unsigned int cur_width;
	char *lnstart, *lnend;
	
	if( ! str ) return 0;
	
	lnstart = str;
	while( ( lnend = strchr( str, '\n' ) ) ){
		cur_width = lnend - lnstart;
		if( cur_width > max_width ) max_width = cur_width;
		
		lnstart = lnend++;
	}
	
	return pbOffsetChar( max_width ? max_width : strlen( str ) );
}
