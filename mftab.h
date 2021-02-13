/*=========================================================

	mftab.h

	MacroFireファンクションテーブル

=========================================================*/
#ifndef MFTAB_H
#define MFTAB_H

#ifdef MF_FIRST_INCLUDE
#define GLOBAL
/* 機能を実装したヘッダファイル */
#include "functions/remap.h"
#include "functions/rapidfire.h"
#include "functions/macro.h"
#else
#define GLOBAL extern
#endif

/*=========================================================
	ファンクションエントリ
=========================================================*/
typedef struct {
	char  *name;
	MfProc proc;
} MfFuncEntry;

/*=========================================================
	ファンクションテーブル
=========================================================*/
GLOBAL MfFuncEntry gMftab[]
#ifdef MF_FIRST_INCLUDE
= {
	{ MF_STR_FUNCTION_REMAP,     remapProc },
	{ MF_STR_FUNCTION_RAPIDFIRE, rapidfireProc },
	{ MF_STR_FUNCTION_MACRO,     macroProc }
}
#endif
;

/*=======================================================*/

GLOBAL unsigned int gMftabEntryCount
#ifdef MF_FIRST_INCLUDE
= sizeof( gMftab ) / sizeof( MfFuncEntry )
#endif
;

#endif
