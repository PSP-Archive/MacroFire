/*=========================================================

	function/rapidfire.h

	�A�ːݒ�B

=========================================================*/
#ifndef FUNCTION_RAPIDFIRE_H
#define FUNCTION_RAPIDFIRE_H

#include "../macrofire.h"
#include "memory/heap_kernel.h"

/*=========================================================
	�}�N��
=========================================================*/
#define RAPIDFIRE_INI_SIGNATURE     "MACROFIRE-RAPIDFIRE"
#define RAPIDFIRE_INI_VERSION       4
#define RAPIDFIRE_INI_SECTION_MODE  "Mode"
#define RAPIDFIRE_INI_SECTION_DELAY "WaitControl"
#define RAPIDFIRE_INI_ENTRY_PD      "PressDelay"
#define RAPIDFIRE_INI_ENTRY_RD      "ReleaseDelay"

/*=========================================================
	�^�錾
=========================================================*/

/*=========================================================
	�֐�
=========================================================*/
void *rapidfireProc( MfMessage message );
void rapidfireIniLoad( InimgrUID ini, char *buf, size_t len );
void rapidfireIniCreate( InimgrUID ini, char *buf, size_t len );
void rapidfireMain( MfHookAction action, SceCtrlData *pad, MfHprmKey *hk );
int  rapidfireMenu( MfMessage message );

#endif
