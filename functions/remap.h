/*=========================================================

	remap.h

	ボタン再割り当て。

=========================================================*/
#ifndef FUNCTION_REMAP_H
#define FUNCTION_REMAP_H

#include "../macrofire.h"
#include "memory/dmem.h"

/*=========================================================
	マクロ
=========================================================*/
#define REMAP_LINES_PER_PAGE 18

#define REMAP_INI_SIGNATURE "MACROFIRE-REMAP"
#define REMAP_INI_VERSION   2

#define REMAP_SECTION_DATA "Remap"

#define REMAP_COLUMN_LIST 0
#define REMAP_COLUMN_MENU 1

/*=========================================================
	関数
=========================================================*/
void *remapProc( MfMessage message );
void remapIniLoad( InimgrUID ini, char *buf, size_t len );
void remapIniCreate( InimgrUID ini, char *buf, size_t len );
void remapMain( MfHookAction action, SceCtrlData *pad, MfHprmKey *hk );
int  remapMenu( MfMenuMessage message );


#endif
