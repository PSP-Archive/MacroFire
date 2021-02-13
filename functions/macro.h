/*=========================================================

	macro.h

	マクロ。

=========================================================*/
#ifndef FUNCTION_MACRO_H
#define FUNCTION_MACRO_H

#include "../macrofire.h"
#include "memory/heap_kernel.h"
#include "macromgr.h"
#include "macroeditor.h"

/*=========================================================
	マクロ
=========================================================*/
#define MACRO_NAME_LENGTH   32
#define MACRO_HOTKEY_LENGTH 255

#define MACRO_INI_SIGNATURE "MACROFIRE-MACRO"
#define MACRO_SECTION_INFO  "Info"
#define MACRO_SECTION_DATA  "Macro"

#define MACRO_ENTRY_NAME   "Name"
#define MACRO_ENTRY_REPEAT "Repeat"
#define MACRO_ENTRY_HOTKEY "Hotkey"

/*=========================================================
	関数
=========================================================*/
void *macroProc( MfMessage message );
void macroIniLoad( InimgrUID ini, char *buf, size_t len );
void macroIniCreate( InimgrUID ini, char *buf, size_t len );
void macroMain( MfHookAction action, SceCtrlData *pad, MfHprmKey *hk );
int  macroMenu( MfMenuMessage message );


#endif
