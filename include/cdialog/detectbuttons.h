/*=========================================================

	detectbuttons.h

	ボタンの組み合わせ取得。

=========================================================*/
#ifndef CDIALOG_DETECTBUTTONS_H
#define CDIALOG_DETECTBUTTONS_H

#include "dev.h"
#include "message.h"
#include "pad/padutil.h"

/*=========================================================
	マクロ
=========================================================*/
#define CDIALOG_DETECTBUTTONS_TITLE_LENGTH 64
#define CDIALOG_DETECTBUTTONS_BUF_LENGTH  128

#ifdef __cplusplus
extern "C" {
#endif

/*=========================================================
	型宣言
=========================================================*/
typedef struct {
	char               title[CDIALOG_DETECTBUTTONS_TITLE_LENGTH];
	unsigned int       options;
	PadutilButtons     availButtons;
	PadutilButtons     *buttons;
} CdialogDetectbuttonsData;

struct cdialog_detectbuttons_work {
	unsigned int flags;
	PadutilButtons buttons;
	char buf[CDIALOG_DETECTBUTTONS_BUF_LENGTH];
	PadutilButtonName *buttonNames;
	
	PadutilButtonName **availButtonNames;
	unsigned char availButtonCount;
	unsigned char index;
};

typedef struct {
	bool destroySelf;
	struct cdialog_dev_base_params base;
	CdialogDetectbuttonsData data;
	struct cdialog_detectbuttons_work work;
} CdialogDetectbuttonsParams;

/*=========================================================
	関数
=========================================================*/
int cdialogDetectbuttonsInit( CdialogDetectbuttonsParams *params );
CdialogDetectbuttonsData *cdialogDetectbuttonsGetData( void );
CdialogStatus cdialogDetectbuttonsGetStatus( void );
CdialogResult cdialogDetectbuttonsGetResult( void );
int cdialogDetectbuttonsStart( unsigned int x, unsigned int y );
int cdialogDetectbuttonsStartNoLock( unsigned int x, unsigned int y );
int cdialogDetectbuttonsUpdate( void );
int cdialogDetectbuttonsShutdownStartNoLock( void );
int cdialogDetectbuttonsShutdownStart( void );
void cdialogDetectbuttonsDestroy( void );

#ifdef __cplusplus
}
#endif

#endif
