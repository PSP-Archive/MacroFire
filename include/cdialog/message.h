/*=========================================================

	cdialog/message.h

	メッセージダイアログ。

=========================================================*/
#ifndef CDIALOG_MESSAGE_H
#define CDIALOG_MESSAGE_H

#include "dev.h"

/*=========================================================
	定数
=========================================================*/
#define CDIALOG_MESSAGE_TITLE_LENGTH 32
#define CDIALOG_MESSAGE_LENGTH       512

#ifdef __cplusplus
extern "C" {
#endif

/*=========================================================
	型宣言
=========================================================*/
typedef enum {
	CDIALOG_MESSAGE_YESNO = 0x00000001,
	CDIALOG_MESSAGE_ERROR = 0x00000002
} CdialogMessageOptions;

typedef int ( *CdialogMessageCallback )( unsigned int x, unsigned int y, struct cdialog_dev_color *colors );

typedef struct {
	char title[CDIALOG_MESSAGE_TITLE_LENGTH];
	char message[CDIALOG_MESSAGE_LENGTH];
	unsigned int errorcode;
	unsigned int options;
	
	unsigned int width, height;
	CdialogMessageCallback callback;
} CdialogMessageData;

typedef struct {
	bool destroySelf;
	struct cdialog_dev_base_params base;
	CdialogMessageData data;
} CdialogMessageParams;

/*=========================================================
	関数
=========================================================*/
int cdialogMessageInit( CdialogMessageParams *params );
CdialogMessageData *cdialogMessageGetData( void );
CdialogStatus cdialogMessageGetStatus( void );
CdialogResult cdialogMessageGetResult( void );
int cdialogMessageStart( unsigned int x, unsigned int y );
int cdialogMessageStartNoLock( unsigned int x, unsigned int y );
int cdialogMessageUpdate( void );
int cdialogMessageShutdownStartNoLock( void );
int cdialogMessageShutdownStart( void );
void cdialogMessageDestroy( void );

#ifdef __cplusplus
}
#endif

#endif
