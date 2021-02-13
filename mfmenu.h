/*=========================================================

	mfmenu.h

	MacroFire ���j���[

=========================================================*/
#ifndef MFMENU_H
#define MFMENU_H

#include "macrofire.h"
#include "mfdialog.h"

/*=========================================================
	�}�N��
=========================================================*/
#define MF_MENU_INFOTEXT_LENGTH 256

#define mfMenuInitTable( menu )      mfMenuInitTables( menu, 1 )
#define mfMenuDrawTable( menu, opt ) mfMenuDrawTables( menu, 1, opt )

/*=========================================================
	�^�錾
=========================================================*/
typedef enum {
	MF_MENU_NO_OPTIONS = 0,
	
	MF_MENU_DISPLAY_ONLY = 0x00000001,
	MF_MENU_EXIT         = 0x00000002,
	
	MF_MENU_SCREEN_UPDATE = 0x00010000,
	MF_MENU_FORCED_QUIT   = 0x00020000
} MfMenuFlags;

typedef enum {
	MF_MENU_INFOTEXT_NO_OPTIONS       = 0,
	MF_MENU_INFOTEXT_SET_LOWER_LINE   = 0x00000001,
	MF_MENU_INFOTEXT_SET_MIDDLE_LINE  = 0x00000002,
	MF_MENU_INFOTEXT_ERROR            = 0x00000004,
	MF_MENU_INFOTEXT_COMMON_CTRL      = 0x00000010,
	MF_MENU_INFOTEXT_MULTICOLUMN_CTRL = 0x00000020,
	MF_MENU_INFOTEXT_MOVABLEPAGE_CTRL = 0x00000040,
} MfMenuInfoTextOptions;

/*=========================================================
	�v���g�^�C�v
=========================================================*/
bool mfMenuInit( void );
void mfMenuDestroy( void );
void mfMenuMain( SceCtrlData *pad, MfHprmKey *hk );
void mfMenuIniLoad( InimgrUID ini, char *buf, size_t len );
void mfMenuIniSave( InimgrUID ini, char *buf, size_t len );

/*-----------------------------------------------
	mfMenuAddSysThreadId
	
	�V�X�e���X���b�h�Ƃ��ăX���b�hUID��ǉ�����B
	�ǉ������X���b�h�̓V�X�e���X���b�h�ƌ��Ȃ���A���j���[�\�����ɒ�~����Ȃ��B
-----------------------------------------------*/
bool mfMenuAddSysThreadId( SceUID thid );

/*-----------------------------------------------
	mfMenuMemoryAlloc
	mfMenuMemoryCalloc
	mfMenuMemoryFree
-----------------------------------------------*/
void *mfMenuMemoryAlloc( size_t size );
void *mfMenuMemoryCalloc( size_t size );
int  mfMenuMemoryFree( void *addr );

/*-----------------------------------------------
	mfMenuRegisterTempMemory
	
	�e�v���V�[�W�����I�������Ƃ��Ɏ����I�ɉ������铮�I��������o�^����B
	�o�^���郁������mfMenuMemoryAlloc�Ŋm�ۂ����A�h���X�łȂ���΂Ȃ�Ȃ��B
	
	�o�^�����ƁAreturn MF_MENU_BACK��mfMenuBack()�Ń��j���[�𔲂����ہA�����I�ɉ�������B
-----------------------------------------------*/
bool mfMenuRegisterTempMemory( void *proc, unsigned int count, ... );

/*-----------------------------------------------
	mfMenuError
	
	�G���[��\������B
-----------------------------------------------*/
void mfMenuError( int err, const char *msg );

/*-----------------------------------------------
	mfMenuProc
	
	���Ɏ��s����v���V�[�W����ݒ肷��B
-----------------------------------------------*/
void mfMenuProc( MfMenuProc proc );

/*-----------------------------------------------
	mfMenuBack
	
	���݂̃v���V�[�W�����I�����āA���̑O�Ɏ��s���Ă����v���V�[�W���֖߂�B
-----------------------------------------------*/
void mfMenuBack( void );

/*-----------------------------------------------
	mfMenuSetExtra
	
	���݂̃v���V�[�W���Ɠ����Ɏ��s�����ǉ��v���V�[�W����ݒ肷��B
-----------------------------------------------*/
int mfMenuSetExtra( MfMenuExtraProc extra, void *arg );

/*-----------------------------------------------
	mfMenuExitExtra
	
	�ǉ��v���V�[�W�����I������B
-----------------------------------------------*/
int mfMenuExitExtra( void );

/*-----------------------------------------------
	mfMenuSendSignal
	
	���݂̃v���V�[�W���փV�O�i���𑗂�B
-----------------------------------------------*/
void mfMenuSendSignal( MfSignalMessage sig );

void mfMenuEnable( unsigned int flags );
void mfMenuDisable( unsigned int flags );

char *mfMenuButtonsSymbolList( PadutilButtons buttons, char *str, size_t len );

void mfMenuInitTables( MfMenuTable menu[], unsigned int menucnt );
void mfMenuTermTables( MfMenuTable menu[], unsigned int menucnt );
int mfMenuDrawTables( MfMenuTable menu[], unsigned int cnt, unsigned int opt );
MfMenuTable *mfMenuCreateTables( unsigned int tables, ... );
void mfMenuDestroyTables( MfMenuTable *table );

void mfMenuSetTableLabel( MfMenuTable *menu, unsigned int tableid, char *label );
void mfMenuSetTablePosition( MfMenuTable *menu, unsigned int tableid, int x, int y );
void mfMenuSetColumnWidth( MfMenuTable *menu, unsigned int tableid, unsigned int col, unsigned int width );
void mfMenuSetTableEntry( MfMenuTable *menu, unsigned int tableid, unsigned int row, unsigned int col, char *label, MfControl ctrl, void *var, void *arg );
void mfMenuActivateTableEntry( MfMenuTable *menu, unsigned int tableid, unsigned int row, unsigned int col );
void mfMenuDeactivateTableEntry( MfMenuTable *menu, unsigned int tableid, unsigned int row, unsigned int col );

void mfMenuSetInfoText( unsigned int options, char *format, ... );

unsigned int mfMenuAcceptButton( void );
unsigned int mfMenuCancelButton( void );
char *mfMenuAcceptSymbol( void );
char *mfMenuCancelSymbol( void );

SceCtrlData *mfMenuGetCurrentPadData( void );
unsigned int mfMenuGetCurrentButtons( void );
unsigned char mfMenuGetCurrentAnalogStickX( void );
unsigned char mfMenuGetCurrentAnalogStickY( void );
u32 mfMenuGetCurrentHprmKey( void );
void mfMenuResetKeyRepeat( void );

unsigned int mfMenuScroll( int selected, unsigned int viewlines, unsigned int maxlines );

#define mfMenuActivateTable( m, t )   mfMenuActivateTableEntry( m, t, 0, 0 );
#define mfMenuDeactivateTable( m, t ) mfMenuDeactivateTableEntry( m, t, 0, 0 );

#define mfMenuQuit()            mfMenuEnable( MF_MENU_FORCED_QUIT )
#define mfMenuUpdate()          mfMenuEnable( MF_MENU_SCREEN_UPDATE )
#define mfMenuIsEnabled( f, t ) ( ( ( ( f ) & ( t ) ) == ( t ) ? true : false ) )

#endif
