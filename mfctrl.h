/*=========================================================

	mfctrl.h

	組み込みコントロール。

=========================================================*/
#ifndef MFCTRL_H
#define MFCTRL_H

#include "macrofire.h"

/*=========================================================
	マクロ
=========================================================*/
#define MF_CTRL_BUFFER_LENGTH 250

/*=========================================================
	設定構造体
=========================================================*/
typedef struct {
	char *unit;
	unsigned int min, max, width;
} MfCtrlDefGetNumberPref;

typedef struct {
	PadutilButtons availButtons;
} MfCtrlDefGetButtonsPref;

typedef enum {
	MF_CTRL_INI_LOAD = 0x00000001,
	MF_CTRL_INI_SAVE = 0x00000002,
} MfCtrlDefIniAction;
	
typedef int ( *MfCtrlDefIniProc )( MfCtrlDefIniAction, const char* );

typedef struct {
	char   *title;
	char   *initDir;
	char   *initFilename;
	size_t pathMax;
} MfCtrlDefIniPref;


/*=========================================================
	関数定義
=========================================================*/
void mfCtrlSetLabel( void *arg, char *format, ... );
int mfCtrlDefBool( MfMessage message, const char *label, void *var, void *arg, void *ex );
int mfCtrlDefOptions( MfMessage message, const char *label, void *var, void *arg, void *ex );
int mfCtrlDefCallback( MfMessage message, const char *label, void *func, void *arg, void *ex );
int mfCtrlDefExtra( MfMessage message, const char *label, void *func, void *arg, void *ex );
int mfCtrlDefGetButtons( MfMessage message, const char *label, void *var, void *pref, void *ex );
int mfCtrlDefGetNumber( MfMessage message, const char *label, void *var, void *pref, void *ex );
unsigned int mfCtrlVarGetNum( void *num, unsigned int max );
void mfCtrlVarSetNum( void *num, unsigned int max, unsigned int value );
int mfCtrlDefIniLoad( MfMessage message, const char *label, void *loader, void *pref, void *ex );
int mfCtrlDefIniSave( MfMessage message, const char *label, void *saver, void *pref, void *ex );

#endif
