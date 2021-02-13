/*=========================================================

	mfanalogstick.c

	アナログスティックの調整。
	
	他の連射やマクロ機能のような構造になっているが、
	ここの関数は、メニューから直接呼び出しているため変更する場合は注意する。

=========================================================*/
#include "mfanalogstick.h"

/*=========================================================
	ローカル関数
=========================================================*/

/*=========================================================
	ローカル変数
=========================================================*/
static bool st_enable_movement;
static PadutilAnalogStick st_analogstick;

/*=========================================================
	関数
=========================================================*/
void mfAnalogStickInit( void )
{
	st_enable_movement         = true;
	st_analogstick.deadzone    = 40;
	st_analogstick.sensitivity = 100;
	st_analogstick.originX     = PADUTIL_CENTER_X;
	st_analogstick.originY     = PADUTIL_CENTER_Y;
}

void mfAnalogStickIniLoad( InimgrUID ini, char *buf, size_t len )
{
	int deadzone, sensitivity, originx, originy;
	
	st_enable_movement = true;
	
	mfIniGetBool( ini, "AnalogStickMovement", &st_enable_movement );
	if( CG_IS_OK( mfIniGetInt(  ini, "AnalogStickDeadzone",    &deadzone )    ) ) st_analogstick.deadzone    = deadzone;
	if( CG_IS_OK( mfIniGetInt(  ini, "AnalogStickSensitivity", &sensitivity ) ) ) st_analogstick.sensitivity = sensitivity;
	if( CG_IS_OK( mfIniGetInt(  ini, "AnalogStickOriginX",     &originx )     ) ) st_analogstick.originX     = originx;
	if( CG_IS_OK( mfIniGetInt(  ini, "AnalogStickOriginY",     &originy )     ) ) st_analogstick.originY     = originy;
}

void mfAnalogStickIniSave( InimgrUID ini, char *buf, size_t len )
{
	inimgrSetBool( ini, MF_INI_SECTION_DEFAULT, "AnalogStickMovement",    true );
	inimgrSetInt(  ini, MF_INI_SECTION_DEFAULT, "AnalogStickDeadzone",    40 );
	inimgrSetInt(  ini, MF_INI_SECTION_DEFAULT, "AnalogStickSensitivity", 100 );
	inimgrSetInt(  ini, MF_INI_SECTION_DEFAULT, "AnalogStickOriginX",     PADUTIL_CENTER_X );
	inimgrSetInt(  ini, MF_INI_SECTION_DEFAULT, "AnalogStickOriginY",     PADUTIL_CENTER_Y );
}

void mfAnalogStickAdjust( SceCtrlData *pad )
{
	if( st_enable_movement ){
		padutilAdjustAnalogStick( &st_analogstick, pad );
	} else{
		pad->Lx = PADUTIL_CENTER_X;
		pad->Ly = PADUTIL_CENTER_Y;
	}
}

int mfAnalogStickMenu( MfMenuMessage message )
{
	static MfMenuTable   *menu;
	SceCtrlData *pad, pad_dupe;
	pad = mfMenuGetCurrentPadData();
	
	switch( message ){
		case MF_MM_INIT:
			dbgprint( "Creating analogstick menu" );
			menu = mfMenuCreateTables(
				1,
				7, 1
			);
			
			if( menu ){
				MfCtrlDefGetNumberPref *pref_deadzone    = mfMenuMemoryCalloc( sizeof( MfCtrlDefGetNumberPref ) );
				MfCtrlDefGetNumberPref *pref_sensitivity = mfMenuMemoryCalloc( sizeof( MfCtrlDefGetNumberPref ) );
				MfCtrlDefGetNumberPref *pref_origin      = mfMenuMemoryCalloc( sizeof( MfCtrlDefGetNumberPref ) );
				
				if(
					! pref_deadzone || ! pref_sensitivity || ! pref_origin ||
					! mfMenuRegisterTempMemory( mfAnalogStickMenu, 4, menu, pref_deadzone, pref_sensitivity, pref_origin )
				){
					mfMenuDestroyTables( menu );
					if( pref_deadzone    ) mfMenuMemoryFree( pref_deadzone );
					if( pref_sensitivity ) mfMenuMemoryFree( pref_sensitivity );
					if( pref_origin      ) mfMenuMemoryFree( pref_origin );
					break;
				}
				
				pref_deadzone->unit  = "(0-182)";
				pref_deadzone->max   = 182;
				
				pref_sensitivity->unit = "% (0-400)";
				pref_sensitivity->max  = 400;
				
				pref_origin->unit = "(0-255)";
				pref_origin->max  = 255;
				
				mfMenuSetTablePosition( menu, 1, pbOffsetChar( 5 ), pbOffsetLine( 6 ) );
				mfMenuSetTableEntry( menu, 1, 1, 1, MF_STR_ASA_MOVEMENT, mfCtrlDefBool, &st_enable_movement, NULL );
				mfMenuSetTableEntry( menu, 1, 2, 1, "", NULL, NULL, NULL );
				mfMenuSetTableEntry( menu, 1, 3, 1, MF_STR_ASA_DEADZONE, mfCtrlDefGetNumber, &(st_analogstick.deadzone), pref_deadzone );
				mfMenuSetTableEntry( menu, 1, 4, 1, MF_STR_ASA_SENSITIVITY, mfCtrlDefGetNumber, &(st_analogstick.sensitivity), pref_sensitivity );
				mfMenuSetTableEntry( menu, 1, 5, 1, "", NULL, NULL, NULL );
				mfMenuSetTableEntry( menu, 1, 6, 1, MF_STR_ASA_ORIGIN_X, mfCtrlDefGetNumber, &(st_analogstick.originX), pref_origin );
				mfMenuSetTableEntry( menu, 1, 7, 1, MF_STR_ASA_ORIGIN_Y, mfCtrlDefGetNumber, &(st_analogstick.originY), pref_origin );
				mfMenuInitTable( menu );
				break;
			}
			return CG_ERROR_NOT_ENOUGH_MEMORY;
		case MF_MM_TERM:
			break;
		default:
			pad_dupe = *pad;
			mfAnalogStickAdjust( &pad_dupe );
			
			pbPrint( pbOffsetChar( 3 ), pbOffsetLine(  4 ), MF_COLOR_TEXT_FG, MF_COLOR_TEXT_BG, MF_STR_ASA_DESC );
			
			pbLineRectRel( pbOffsetChar( 60 ) - ( 128 >> 1 ) - 1, pbOffsetLine( 18 ) - ( 128 >> 1 ) - 1, ( 256 >> 1 ) + 2, ( 256 >> 1 ) + 2, MF_COLOR_TEXT_FG );
			pbLineCircle( pbOffsetChar( 60 ) + ( ( st_analogstick.originX - PADUTIL_CENTER_X ) >> 1 ), pbOffsetLine( 18 ) + ( ( st_analogstick.originY - PADUTIL_CENTER_Y ) >> 1 ), st_analogstick.deadzone >> 1, MF_COLOR_TEXT_FC );
			
			/* 調整前座標の位置を示す */
			pbLine(
				pbOffsetChar( 60 ) + ( ( pad->Lx - PADUTIL_CENTER_X  ) >> 1 ),
				pbOffsetLine( 18 ) - ( 127 >> 1 ) - 1,
				pbOffsetChar( 60 ) + ( ( pad->Lx - PADUTIL_CENTER_X  ) >> 1 ),
				pbOffsetLine( 18 ) + ( 127 >> 1 ) + 1,
				( MF_COLOR_TEXT_FC & 0x00ffffff ) | 0x66000000
			);
			pbLine(
				pbOffsetChar( 60 ) - ( 127 >> 1 ) - 1,
				pbOffsetLine( 18 ) + ( ( pad->Ly - PADUTIL_CENTER_Y ) >> 1 ),
				pbOffsetChar( 60 ) + ( 127 >> 1 ) + 1,
				pbOffsetLine( 18 ) + ( ( pad->Ly - PADUTIL_CENTER_Y ) >> 1 ),
				( MF_COLOR_TEXT_FC & 0x00ffffff ) | 0x66000000
			);
			pbPoint( pbOffsetChar( 60 ) + ( ( pad->Lx - PADUTIL_CENTER_X  ) >> 1 ), pbOffsetLine( 18 ) + ( ( pad->Ly - PADUTIL_CENTER_Y ) >> 1 ), MF_COLOR_TEXT_FC );
			
			/* 調整後座標の位置を示す */
			pbLine(
				pbOffsetChar( 60 ) + ( ( pad_dupe.Lx - PADUTIL_CENTER_X  ) >> 1 ),
				pbOffsetLine( 18 ) - ( 127 >> 1 ) - 1,
				pbOffsetChar( 60 ) + ( ( pad_dupe.Lx - PADUTIL_CENTER_X  ) >> 1 ),
				pbOffsetLine( 18 ) + ( 127 >> 1 ) + 1,
				( MF_COLOR_TEXT_FG & 0x00ffffff ) | 0x66000000
			);
			pbLine(
				pbOffsetChar( 60 ) - ( 127 >> 1 ) - 1,
				pbOffsetLine( 18 ) + ( ( pad_dupe.Ly - PADUTIL_CENTER_Y ) >> 1 ),
				pbOffsetChar( 60 ) + ( 127 >> 1 ) + 1,
				pbOffsetLine( 18 ) + ( ( pad_dupe.Ly - PADUTIL_CENTER_Y ) >> 1 ),
				( MF_COLOR_TEXT_FG & 0x00ffffff ) | 0x66000000
			);
			pbPoint( pbOffsetChar( 60 ) + ( ( pad_dupe.Lx - PADUTIL_CENTER_X  ) >> 1 ), pbOffsetLine( 18 ) + ( ( pad_dupe.Ly - PADUTIL_CENTER_Y ) >> 1 ), MF_COLOR_TEXT_FG );
			
			pad_dupe.Buttons |= padutilGetAnalogStickDirection( pad_dupe.Lx, pad_dupe.Ly, 0 );
			{
				unsigned int test_cross_offset = pbMeasureString( MF_STR_ASA_TEST_DIR ) + pbOffsetChar( 1 );
				pbPrint( pbOffsetChar( 3 ), pbOffsetLine( 24 ),  MF_COLOR_TEXT_FG, MF_COLOR_TEXT_BG, MF_STR_ASA_TEST_DIR );
				pbPrint( test_cross_offset + pbOffsetChar( 5 ), pbOffsetLine( 23 ), pad_dupe.Buttons & PADUTIL_CTRL_ANALOG_UP    ? MF_COLOR_TEXT_FC : MF_COLOR_TEXT_FG, MF_COLOR_TEXT_BG, PB_SYM_PSP_UP );
				pbPrint( test_cross_offset + pbOffsetChar( 7 ), pbOffsetLine( 24 ), pad_dupe.Buttons & PADUTIL_CTRL_ANALOG_RIGHT ? MF_COLOR_TEXT_FC : MF_COLOR_TEXT_FG, MF_COLOR_TEXT_BG, PB_SYM_PSP_RIGHT );
				pbPrint( test_cross_offset + pbOffsetChar( 5 ), pbOffsetLine( 25 ), pad_dupe.Buttons & PADUTIL_CTRL_ANALOG_DOWN  ? MF_COLOR_TEXT_FC : MF_COLOR_TEXT_FG, MF_COLOR_TEXT_BG, PB_SYM_PSP_DOWN );
				pbPrint( test_cross_offset + pbOffsetChar( 3 ), pbOffsetLine( 24 ), pad_dupe.Buttons & PADUTIL_CTRL_ANALOG_LEFT  ? MF_COLOR_TEXT_FC : MF_COLOR_TEXT_FG, MF_COLOR_TEXT_BG, PB_SYM_PSP_LEFT );
			}
			pbPrint( pbOffsetChar( 3 ), pbOffsetLine( 27 ),  MF_COLOR_TEXT_FG, MF_COLOR_TEXT_BG, MF_STR_ASA_TEST_MOVE );
			pbPrintf( pbOffsetChar( 5 ), pbOffsetLine( 28 ), MF_COLOR_TEXT_FC, MF_COLOR_TEXT_BG, "%s (X,Y) = ( %03d , %03d )", MF_STR_ASA_TEST_MOVE_RAW, pad->Lx, pad->Ly );
			pbPrintf( pbOffsetChar( 5 ), pbOffsetLine( 29 ), MF_COLOR_TEXT_FG, MF_COLOR_TEXT_BG, "%s (X,Y) = ( %03d , %03d )", MF_STR_ASA_TEST_MOVE_ADJ, pad_dupe.Lx, pad_dupe.Ly );
			
			return mfMenuDrawTable( menu, MF_MENU_NO_OPTIONS );
	}
	return MF_MENU_OK;
}

const PadutilAnalogStick *mfAnalogStickGetContext( void )
{
	return &st_analogstick;
}
