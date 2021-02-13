/*=========================================================

	mfcommontypes.h

	MacroFire ���L�^�錾�B

=========================================================*/
#ifndef MFCOMMONTYPES_H
#define MFCOMMONTYPES_H

#include <pspkernel.h>
#include <psphprm.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdarg.h>
#include <string.h>
#include "util/strutil.h"
#include "file/inimgr.h"
#include "pad/padctrl.h"
#include "pad/padutil.h"
#include "graphic/pb.h"
#include "mfmemory.h"
#include "cgtypes.h"

/*=========================================================
	�}�N��
=========================================================*/
#define MF_SIZEOF_ARRAY( x ) sizeof( x ) / sizeof( x[0] )
#define MF_PATH_MAX CG_PATH_MAX

#define MF_TARGET_BUTTONS ( \
	PSP_CTRL_CIRCLE   | PSP_CTRL_CROSS    | PSP_CTRL_SQUARE | PSP_CTRL_TRIANGLE | \
	PSP_CTRL_UP       | PSP_CTRL_RIGHT    | PSP_CTRL_DOWN   | PSP_CTRL_LEFT     | \
	PSP_CTRL_LTRIGGER | PSP_CTRL_RTRIGGER | PSP_CTRL_SELECT | PSP_CTRL_START    | \
	PADUTIL_CTRL_ANALOG_UP   | PADUTIL_CTRL_ANALOG_RIGHT | \
	PADUTIL_CTRL_ANALOG_DOWN | PADUTIL_CTRL_ANALOG_LEFT  | \
	PSP_CTRL_HOME | PSP_CTRL_NOTE | PSP_CTRL_SCREEN | PSP_CTRL_VOLUP | PSP_CTRL_VOLDOWN \
)

#define MF_HOTKEY_BUTTONS (\
	padutilSetPad( MF_TARGET_BUTTONS ) | padutilSetHprm( PADUTIL_HPRM_NORMAL_KEYS ) \
)

#define mfIsPressed( buttons, target )     ( ( ( buttons ) & ( target ) ) == ( target ) ? true : false )
#define mfIsOnlyPressed( buttons, target ) ( ( buttons ) == ( target ) ? true : false )
#define mfIsAnyPressed( buttons, target )  ( ( buttons ) &  ( target ) ? true : false )

/*=========================================================
	�^�錾
=========================================================*/
/*-----------------------------------------------
	���s����\�����l�B
-----------------------------------------------*/
typedef enum {
	MF_WORLD_GAME = 0x00000001,
	MF_WORLD_POPS = 0x00000002,
	MF_WORLD_VSH  = 0x00000004,
} MfWorldId;

/*-----------------------------------------------
	���s���̃A�v���P�[�V�������ʔԍ��B
	
	���̒l��mfFindApp()�ɓn���ƁA���̃A�v���P�[�V�������N���������ׂ�B
	���̂Ƃ���ANetfront(�E�F�u�u���E�U)�������肵�Ă��Ȃ��B
-----------------------------------------------*/
typedef enum {
	MF_APP_WEBBROWSER = 0x00000001,
	/*
	MF_APP_MUSICPLAYER = 0x00000002,
	MF_APP_VIDEO       = 0x00000004,
	MF_APP_ONESEG      = 0x00000008,
	*/
} MfAppId;

/*-----------------------------------------------
	�p�b�h�f�[�^�̎擾���@
-----------------------------------------------*/
typedef enum {
	MF_KEEP,
	MF_UPDATE
} MfHookAction;

/*-----------------------------------------------
	HPRM�L�[�^
-----------------------------------------------*/
typedef u32 MfHprmKey;

/*-----------------------------------------------
	���b�Z�[�W
	
	���[�g���b�Z�[�W�͊e�t�@���N�V�����̋N�_�ɑ����A
	���̃��b�Z�[�W��g�ݍ��킳��đ����邱�Ƃ͂Ȃ��B
-----------------------------------------------*/
typedef unsigned int MfMessage;
typedef enum {
	MF_MS_INIT       = 0x00000001,
	MF_MS_INI_LOAD   = 0x00000002,
	MF_MS_INI_CREATE = 0x00000004,
	MF_MS_TERM       = 0x00000008,
	MF_MS_HOOK       = 0x00000010,
	MF_MS_TOGGLE     = 0x00000020,
	MF_MS_MENU       = 0x00000040
} MfRootMessage;
	
typedef enum {
	MF_DM_START  = 0x00010000,
	MF_DM_UPDATE = 0x00020000,
	MF_DM_FINISH = 0x00040000,
} MfDialogMessage;

typedef enum {
	MF_SM_1 = 0x01000000,
	MF_SM_2 = 0x02000000,
	MF_SM_3 = 0x04000000,
	MF_SM_4 = 0x08000000,
	MF_SM_5 = 0x10000000,
	MF_SM_6 = 0x20000000,
	MF_SM_7 = 0x40000000,
	MF_SM_8 = 0x80000000
} MfSignalMessage;

/*-----------------------------------------------
	�v���V�[�W��
-----------------------------------------------*/
typedef void ( *MfFuncInit   )( void );
typedef void ( *MfFuncIni    )( InimgrUID, char*, size_t );
typedef void ( *MfFuncTerm   )( void );
typedef void ( *MfFuncHook   )( MfHookAction, SceCtrlData*, MfHprmKey* );
typedef void ( *MfFuncToggle )( bool );
typedef int  ( *MfFuncMenu   )( MfMessage );
typedef void *( *MfProc      )( MfRootMessage message );

#endif
