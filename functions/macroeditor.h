/*=========================================================

	macroeditor.h

	�}�N���G�f�B�^�B

=========================================================*/
#ifndef FUNCTION_MACROEDITOR_H
#define FUNCTION_MACROEDITOR_H

#include "macromgr.h"

/*=========================================================
	�}�N��
=========================================================*/
#define MACROEDITOR_LINES_PER_PAGE 26

/*=========================================================
	�֐�
=========================================================*/
bool macroeditorInit( MacromgrUID uid );
void macroeditorTerm( void );
bool macroeditorMain( MfMessage message );


#endif