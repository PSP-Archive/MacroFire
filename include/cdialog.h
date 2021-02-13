/*=========================================================

	cdialog.h

	共通ダイアログ。

=========================================================*/
#ifndef CDIALOG_H
#define CDIALOG_H

#include <stdbool.h>

/*=========================================================
	マクロ
=========================================================*/
#ifdef __cplusplus
extern "C" {
#endif

/*=========================================================
	型宣言
=========================================================*/
typedef enum {
	CDIALOG_ACCEPT_CROSS = 0x00000001,
} CdialogEnvFlags;

typedef enum {
	CDIALOG_NONE = 0,
	CDIALOG_INIT,
	CDIALOG_VISIBLE,
	CDIALOG_SHUTDOWN
} CdialogStatus;

typedef enum {
	CDIALOG_UNKNOWN = 0,
	CDIALOG_CANCEL,
	CDIALOG_ACCEPT
} CdialogResult;

typedef enum {
	CDIALOG_DISPLAY_CENTER = 0x00010000,
} CdialogOptions;

/*=========================================================
	関数
=========================================================*/
bool cdialogInit( void );
void cdialogFinish( void );
void cdialogSetRemap( PadutilRemap *remap );
void cdialogClearRemap( void );
void cdialogEnable( unsigned int opt );
void cdialogDisable( unsigned int opt );
unsigned int cdialogGetLastError( void );
unsigned int cdialogGetLastSysError( void );

#ifndef CDIALOG_FUNCTION
#include "cdialog/message.h"
#include "cdialog/sosk.h"
#include "cdialog/numedit.h"
#include "cdialog/getfilename.h"
#include "cdialog/detectbuttons.h"
#endif

#ifdef __cplusplus
}
#endif

#endif
