/*=========================================================

	mfanalogstick.c

	アナログスティックの調整。

=========================================================*/
#ifndef MFANALOGSTICK_H
#define MFANALOGSTICK_H

#include "macrofire.h"

/*=========================================================
	関数
=========================================================*/
void mfAnalogStickInit( void );
void mfAnalogStickIniLoad( InimgrUID ini, char *buf, size_t len );
void mfAnalogStickIniSave( InimgrUID ini, char *buf, size_t len );
bool mfAnalogStickIsEnabled( void );
void mfAnalogStickAdjust( SceCtrlData *pad );
int  mfAnalogStickMenu( MfMessage message );
const PadutilAnalogStick *mfAnalogStickGetContext( void );

#endif
