/*=========================================================

	cdialog/dev.h

	共通ダイアログ開発用。

=========================================================*/
#ifndef CDIALOG_DEV_H
#define CDIALOG_DEV_H

#include <pspkernel.h>
#include <psphprm.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "util/strutil.h"
#include "pad/padctrl.h"
#include "pad/padutil.h"
#include "graphic/pb.h"

#ifdef PB_SJIS_SUPPORT
#include "lang_jp.h"
#else
#include "lang_en.h"
#endif

#define CDIALOG_FUNCTION
#include "cdialog.h"
#undef CDIALOG_FUNCTION

/*=========================================================
	マクロ
=========================================================*/
#define CDIALOG_VOID_ABORT( err, syserr ) { cdialogDevSetLastError( err, syserr ); return }
#define CDIALOG_ABORT( ret, err, syserr ) { cdialogDevSetLastError( err, syserr ); return ret }
#define CDIALOG_GET_BTNCODE( btn ) ( ( btn ) & -( btn ) )

#ifdef __cplusplus
extern "C" {
#endif

/*=========================================================
	型宣言
=========================================================*/
struct cdialog_dev_color {
	unsigned int fg;
	unsigned int bg;
	unsigned int fcfg;
	unsigned int fcbg;
	unsigned int border;
	unsigned int title;
	unsigned int help;
	unsigned int extra;
};

typedef struct cdialog_dev_help {
	unsigned int x, y;
	const char *str;
} CdialogDevHelp;

struct cdialog_dev_base_params {
	CdialogStatus status;
	CdialogResult result;
	unsigned int x, y;
	unsigned int width, height;
	struct cdialog_dev_color *color;
	PadctrlUID paduid;
	PadutilAnalogStick analogStick;
	PadutilRemap *remap;
	CdialogDevHelp *help;
};

/*=========================================================
	関数定義
=========================================================*/
bool cdialogDevLock( void );
bool cdialogDevUnlock( void );
unsigned int cdialogDevGetEnvvar( void );
unsigned int cdialogDevAcceptButton( void );
unsigned int cdialogDevCancelButton( void );
char *cdialogDevAcceptSymbol( void );
char *cdialogDevCancelSymbol( void );
void cdialogDevInitBaseParams( struct cdialog_dev_base_params *params );
int cdialogDevPrepareToStart( struct cdialog_dev_base_params *params, unsigned int options );
void cdialogDevPrepareToFinish( struct cdialog_dev_base_params *params );
void cdialogDevSetAnalogStickAdjust( struct cdialog_dev_base_params *params, PadutilCoord x, PadutilCoord y, PadutilCoord deadzone, PadutilSensitivity sens );
void cdialogDevSetRepeatButtons( struct cdialog_dev_base_params *params, unsigned int buttons );
PadutilButtons cdialogDevReadCtrlBuffer( struct cdialog_dev_base_params *params, SceCtrlData *pad, u32 *hprmkey );
void cdialogDevHelp( struct cdialog_dev_help *help, unsigned int x, unsigned int y, const char *str );
void cdialogDevSetHelp( struct cdialog_dev_help *help, size_t count );
int cdialogDevDrawHelp( unsigned int x, unsigned int y, struct cdialog_dev_color *colors );

#ifdef __cplusplus
}
#endif

#endif
