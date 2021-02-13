/*=========================================================

	mfdialog.h

	ダイアログラッパー

=========================================================*/
#ifndef MFDIALOG_H
#define MFDIALOG_H

#include "macrofire.h"
#include "cdialog.h"

/*=========================================================
	型宣言
=========================================================*/
typedef enum {
	MFDIALOG_NONE = 0,
	MFDIALOG_MESSAGE,
	MFDIALOG_SOSK,
	MFDIALOG_NUMEDIT,
	MFDIALOG_GETFILENAME,
	MFDIALOG_DETECTBUTTONS
} MfDialogType;

typedef enum {
	MFDIALOG_DRAW_FINISH = 0,
	MFDIALOG_DRAW_CONTINUE
} MfDialogDrawStatus;

/*=========================================================
	関数
=========================================================*/
void mfDialogInit( PadutilRemap *remap );
void mfDialogFinish( void );
MfDialogType mfDialogCurrentType( void );
bool mfDialogLastResult( void );
int mfDialogMessage( const char *title, const char *message, unsigned int error, bool yesno );
int mfDialogSosk( const char *title, char *text, size_t length, unsigned int availkb );
int mfDialogNumedit( const char *title, const char *unit, void *num, uint32_t max );
int mfDialogGetfilename( const char *title, const char *initdir, const char *initname, char *path, size_t pathmax, unsigned int options );
int mfDialogDetectbuttons( const char *title, PadutilButtons availbtns, PadutilButtons *buttons );

int mfDialogDraw( void );
bool mfDialogResult( void );

#endif
