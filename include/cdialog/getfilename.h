/*=========================================================

	getfilename.h

	任意のファイルパスの取得。
	重い。

=========================================================*/
#ifndef CDIALOG_GETFILENAME_H
#define CDIALOG_GETFILENAME_H

#include "dev.h"
#include "message.h"
#include "sosk.h"
#include "file/dirh.h"

/*=========================================================
	マクロ
=========================================================*/
#define CDIALOG_GETFILENAME_TITLE_LENGTH 30
#define CDIALOG_GETFILENAME_DEFAULT_PATH "ms0:"

#ifdef __cplusplus
extern "C" {
#endif

/*=========================================================
	型宣言
=========================================================*/
typedef enum {
	CDIALOG_GETFILENAME_OPEN            = 0x00000001,
	CDIALOG_GETFILENAME_SAVE            = 0x00000002,
	CDIALOG_GETFILENAME_FILEMUSTEXIST   = 0x00000004,
	CDIALOG_GETFILENAME_OVERWRITEPROMPT = 0x00000008,
	CDIALOG_GETFILENAME_CREATEPROMPT    = 0x00000010
} CdialogGetfilenameOptions;

typedef struct {
	char         title[CDIALOG_GETFILENAME_TITLE_LENGTH + 1];
	unsigned int options;
	const char   *initialDir;
	const char   *initialName;
	char         *path;
	size_t       pathMax;
} CdialogGetfilenameData;

enum cdialog_getfilename_area {
	CDIALOG_GETFILENAME_AREA_LIST,
	CDIALOG_GETFILENAME_AREA_INPUT
};

enum cdialog_getfilename_show_dialog {
	CDIALOG_GETFILENAME_DIALOG_NONE = 0,
	CDIALOG_GETFILENAME_DIALOG_HELP,
	CDIALOG_GETFILENAME_DIALOG_MESSAGE,
	CDIALOG_GETFILENAME_DIALOG_CONFIRM,
	CDIALOG_GETFILENAME_DIALOG_KEYBOARD
};

struct cdialog_getfilename_work {
	DirhUID dirh;
	enum cdialog_getfilename_area area;
	unsigned int allEntries;
	int  selectPos;
	char *filename;
};

typedef struct {
	bool destroySelf;
	struct cdialog_dev_base_params base;
	CdialogGetfilenameData data;
	struct cdialog_getfilename_work work;
	enum cdialog_getfilename_show_dialog showDialog;
	bool shutdown;
} CdialogGetfilenameParams;

/*=========================================================
	関数
=========================================================*/
int cdialogGetfilenameInit( CdialogGetfilenameParams *params );
CdialogGetfilenameData *cdialogGetfilenameGetData( void );
CdialogStatus cdialogGetfilenameGetStatus( void );
CdialogResult cdialogGetfilenameGetResult( void );
int cdialogGetfilenameStart( unsigned int x, unsigned int y );
int cdialogGetfilenameStartNoLock( unsigned int x, unsigned int y );
int cdialogGetfilenameUpdate( void );
int cdialogGetfilenameShutdownStartNoLock( void );
int cdialogGetfilenameShutdownStart( void );
void cdialogGetfilenameDestroy( void );

#ifdef __cplusplus
}
#endif

#endif
