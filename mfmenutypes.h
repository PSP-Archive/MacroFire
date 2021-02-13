/*=========================================================

	mfmenutypes.h

	メニュー用型宣言。

=========================================================*/
#ifndef MFMENUTYPES_H
#define MFMENUTYPES_H

#include "mfctrltypes.h"

/*=========================================================
	マクロ
=========================================================*/
#define mfMenuIsPressed( buttons )     mfIsPressed( mfMenuGetCurrentButtons(), ( buttons ) )
#define mfMenuIsOnlyPressed( buttons ) mfIsOnlyPressed( mfMenuGetCurrentButtons(), ( buttons ) )
#define mfMenuIsAnyPressed( buttons )  mfIsAnyPressed( mfMenuGetCurrentButtons(), ( buttons ) )

#define mfMenuMaskMainMessage( message )   ( ( message ) & 0x0000FFFF )
#define mfMenuMaskDialogMessage( message ) ( ( message ) & 0x00FF0000 )
#define mfMenuMaskSignalMessage( message ) ( ( message ) & 0xFF000000 )

#define MF_MESSAGE_DELAY 1000000
#define MF_ERROR_DELAY   3000000

/*=========================================================
	型宣言
=========================================================*/
typedef enum {
	MF_MM_INIT     = 0x00000001,
	MF_MM_PROC     = 0x00000002,
	MF_MM_TERM     = 0x00000004,
	MF_MM_WAIT     = 0x00000010,
	MF_MM_CONTINUE = 0x00000020
} MfMenuMessage;

typedef enum {
	MF_MENU_OK = CG_OK,
	MF_MENU_BACK
} MfMenuStatus;

typedef int ( *MfMenuProc )( MfMessage );
typedef int ( *MfMenuExtraProc )( MfMessage, void* );

typedef struct {
	bool      active;
	MfControl ctrl;
	char      *label;
	void      *var;
	void      *arg;
} MfMenuEntry;

typedef struct {
	char *label;
	int x, y;
	unsigned int rows, cols;
	unsigned int *colsWidth;
	MfMenuEntry  **entry;
} MfMenuTable;

#endif
