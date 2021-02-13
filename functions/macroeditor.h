/*=========================================================

	macroeditor.h

	マクロエディタ。

=========================================================*/
#ifndef FUNCTION_MACROEDITOR_H
#define FUNCTION_MACROEDITOR_H

#include "macromgr.h"

/*=========================================================
	マクロ
=========================================================*/
#define MACROEDITOR_LINES_PER_PAGE 26

/*=========================================================
	関数
=========================================================*/
bool macroeditorInit( MacromgrUID uid );
void macroeditorTerm( void );
bool macroeditorMain( MfMessage message );


#endif