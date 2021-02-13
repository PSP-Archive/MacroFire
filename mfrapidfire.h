/*=========================================================

	mfrapidfire.h
	場所: ./mfrapidfire

	連射機能。

=========================================================*/
#ifndef MFRAPIDFIRE_H
#define MFRAPIDFIRE_H

#include "macrofire.h"
#include <psprtc.h>

/*=========================================================
	マクロ
=========================================================*/
#define MF_RAPIDFIRE_NUMBER_OF_AVAIL_BUTTONS 12

#define MF_RAPIDFIRE_DEFAULT_PRESS_DELAY   17
#define MF_RAPIDFIRE_DEFAULT_RELEASE_DELAY 17

#define MF_RAPIDFIRE_NAME_NORMAL    "Normal"
#define MF_RAPIDFIRE_NAME_RAPID     "Rapid"
#define MF_RAPIDFIRE_NAME_HOLD      "Hold"
#define MF_RAPIDFIRE_NAME_AUTORAPID "Auto-Rapid"
#define MF_RAPIDFIRE_NAME_AUTOHOLD  "Auto-Hold"

/*=========================================================
	型宣言
=========================================================*/
typedef intptr_t MfRapidfireUID;

typedef enum {
	MF_RAPIDFIRE_MODE_NORMAL = 0,
	MF_RAPIDFIRE_MODE_RAPID,
	MF_RAPIDFIRE_MODE_AUTORAPID,
	MF_RAPIDFIRE_MODE_HOLD,
	MF_RAPIDFIRE_MODE_AUTOHOLD
} MfRapidfireMode;

typedef struct {
	unsigned int    button;       /* 対象ボタン */
	MfRapidfireMode mode;         /* モード */
	unsigned int    pressDelay;   /* 連射時のディレイ */
	unsigned int    releaseDelay; /* 連射時のディレイ */
	unsigned int    nextDelay;    /* 次のボタンアクションまでの待ち時間 (内部で使用) */
	unsigned int    bitFlags;     /* モードに応じて必要なフラグがセットされる (内部で使用) */
} MfRapidfireData;

typedef struct {
	uint64_t lastTick;
	MfRapidfireData pref[MF_RAPIDFIRE_NUMBER_OF_AVAIL_BUTTONS];
} MfRapidfireParams;

/*=========================================================
	関数
=========================================================*/
MfRapidfireUID mfRapidfireNew( void );
void mfRapidfireDestroy( MfRapidfireUID uid );
MfRapidfireUID mfRapidfireBind( MfRapidfireParams *params );
void mfRapidfireSetMode( MfRapidfireUID uid, unsigned int buttons, MfRapidfireMode mode, unsigned int pdelay, unsigned rdelay );
bool mfRapidfireGetEntry( MfRapidfireUID uid, MfRapidfireData *data, enum PspCtrlButtons button );
bool mfRapidfireReadEntry( MfRapidfireUID uid, MfRapidfireData *data, unsigned int *save );
void mfRapidfireExec( MfRapidfireUID uid, MfHookAction action, SceCtrlData *pad );
MfRapidfireMode mfRapidfireGetModeByName( const char *name );
const char *mfRapidfireGetNameByMode( MfRapidfireMode mode );

/* mfRapidfireSetMode()のマクロ */
#define mfRapidfireRapid( uid, buttons, pd, rd )     mfRapidfireSetMode( uid, buttons, MF_RAPIDFIRE_MODE_RAPID, pd, rd )
#define mfRapidfireAutoRapid( uid, buttons, pd, rd ) mfRapidfireSetMode( uid, buttons, MF_RAPIDFIRE_MODE_AUTORAPID, pd, rd )
#define mfRapidfireHold( uid, buttons )              mfRapidfireSetMode( uid, buttons, MF_RAPIDFIRE_MODE_HOLD, 0, 0 )
#define mfRapidfireAutoHold( uid, buttons )          mfRapidfireSetMode( uid, buttons, MF_RAPIDFIRE_MODE_AUTOHOLD, 0, 0 )
#define mfRapidfireClear( uid, buttons )             mfRapidfireSetMode( uid, buttons, MF_RAPIDFIRE_MODE_NORMAL, 0, 0 )

#endif
