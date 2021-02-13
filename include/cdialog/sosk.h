/*=========================================================

	cdialog/sosk.h

	ä»à’OSKÅB

=========================================================*/
#ifndef CDIALOG_SOSK_H
#define CDIALOG_SOSK_H

#include "dev.h"
#include "message.h"

/*=========================================================
	É}ÉNÉç
=========================================================*/
#define CDIALOG_SOSK_TITLE_LENGTH     32
#define CDIALOG_SOSK_INPUT_AREA_WIDTH 32

#define CDIALOG_SOSK_INPUTTYPE_ASCII (\
	CDIALOG_SOSK_INPUTTYPE_DIGIT     |\
	CDIALOG_SOSK_INPUTTYPE_LOWERCASE |\
	CDIALOG_SOSK_INPUTTYPE_UPPERCASE |\
	CDIALOG_SOSK_INPUTTYPE_SYMBOL \
)

#define CDIALOG_SOSK_INPUTTYPE_FILENAME (\
	CDIALOG_SOSK_INPUTTYPE_DIGIT     |\
	CDIALOG_SOSK_INPUTTYPE_LOWERCASE |\
	CDIALOG_SOSK_INPUTTYPE_UPPERCASE |\
	CDIALOG_SOSK_INPUTTYPE_FILENAME_SYMBOL \
)

#ifdef __cplusplus
extern "C" {
#endif

/*=========================================================
	å^êÈåæ
=========================================================*/
typedef enum {
	CDIALOG_SOSK_DENY_WHITESPACE = 0x00000001,
} CdialogSoskOptions;

typedef enum {
	CDIALOG_SOSK_INPUTTYPE_LOWERCASE       = 0x00000001,
	CDIALOG_SOSK_INPUTTYPE_UPPERCASE       = 0x00000002,
	CDIALOG_SOSK_INPUTTYPE_DIGIT           = 0x00000004,
	CDIALOG_SOSK_INPUTTYPE_SYMBOL          = 0x00000008,
	CDIALOG_SOSK_INPUTTYPE_FILENAME_SYMBOL = 0x00000010,
} CdialogSoskInputType;

typedef struct {
	char *codes;
	int width;
	int letterSpace;
	int lineHeight;
} CdialogSoskCharTable;

typedef struct {
	char         title[CDIALOG_SOSK_TITLE_LENGTH];
	unsigned int options;
	char         *text;
	size_t       textMax;
	unsigned int types;
} CdialogSoskData;

struct cdialog_sosk_work_text {
	char *buf;
	int  length;
	int  offset;
	int  cursorPos;
};

struct cdialog_sosk_work_chtab {
	CdialogSoskInputType type;
	CdialogSoskCharTable *data;
	int                  count;
	int                  pos;
	int                  lastPos;
};

struct cdialog_sosk_work {
	struct cdialog_sosk_work_text  text;
	struct cdialog_sosk_work_chtab chtab;
};

typedef struct {
	bool destroySelf;
	struct cdialog_dev_base_params base;
	CdialogSoskData data;
	struct cdialog_sosk_work work;
	bool edited;
	bool showMessage;
	bool cancel;
} CdialogSoskParams;

/*=========================================================
	ä÷êî
=========================================================*/
int cdialogSoskInit( CdialogSoskParams *params );
CdialogSoskData *cdialogSoskGetData( void );
CdialogStatus cdialogSoskGetStatus( void );
CdialogResult cdialogSoskGetResult( void );
int cdialogSoskStart( unsigned int x, unsigned int y );
int cdialogSoskStartNoLock( unsigned int x, unsigned int y );
int cdialogSoskUpdate( void );
int cdialogSoskShutdownStartNoLock( void );
int cdialogSoskShutdownStart( void );
void cdialogSoskDestroy( void );

#ifdef __cplusplus
}
#endif

#endif
