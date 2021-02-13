/*=========================================================

	cdialog/numedit.h

	数値編集。
	
	printf系の関数が64bit幅の整数を正常に書式化できないようなので、
	とりあえず32bit幅まで。
	
	以下のコードでバスエラー
	---------------------------------
	char buf[20];
	unsigned long long val64 = ~0; //uint64_tと宣言しても同じ。
	sprintf( buf, "%019llu", val64 );
	---------------------------------

=========================================================*/
#ifndef CDIALOG_NUMEDIT_H
#define CDIALOG_NUMEDIT_H

#include "dev.h"
#include "message.h"
#include <math.h>

/*=========================================================
	マクロ
=========================================================*/
#define CDIALOG_NUMEDIT_TITLE_LENGTH              64
#define CDIALOG_NUMEDIT_UNIT_LENGTH               32
#define CDIALOG_NUMEDIT_MAX_NUMBER_OF_DIGITS      10 /* uint32_t */

#ifdef __cplusplus
extern "C" {
#endif

/*=========================================================
	型宣言
=========================================================*/
typedef struct {
	char         title[CDIALOG_NUMEDIT_TITLE_LENGTH];
	unsigned int options;
	char         unit[CDIALOG_NUMEDIT_UNIT_LENGTH];
	uint32_t     max;
	void         *num;
} CdialogNumeditData;

struct cdialog_numedit_work {
	unsigned int maxdigits;
	unsigned int pos;
	char *numbers;
	char buf[CDIALOG_NUMEDIT_MAX_NUMBER_OF_DIGITS + 1];
};

typedef struct {
	bool destroySelf;
	struct cdialog_dev_base_params base;
	CdialogNumeditData data;
	struct cdialog_numedit_work work;
	bool showMessage;
} CdialogNumeditParams;

/*=========================================================
	関数
=========================================================*/
int cdialogNumeditInit( CdialogNumeditParams *params );
CdialogNumeditData *cdialogNumeditGetData( void );
CdialogStatus cdialogNumeditGetStatus( void );
CdialogResult cdialogNumeditGetResult( void );
int cdialogNumeditStart( unsigned int x, unsigned int y );
int cdialogNumeditStartNoLock( unsigned int x, unsigned int y );
int cdialogNumeditUpdate( void );
int cdialogNumeditShutdownStartNoLock( void );
int cdialogNumeditShutdownStart( void );
void cdialogNumeditDestroy( void );

#ifdef __cplusplus
}
#endif

#endif
