/* padctrlPeekBuffer.c */

#include "padctrl_types.h"

unsigned int padctrlPeekBuffer( PadctrlUID uid, SceCtrlData *pad, int count )
{
	unsigned int realbuttons;
	
	sceCtrlPeekBufferPositive( pad, count );
	realbuttons = pad->Buttons;
	
	__padctrl_proc( uid, pad, count );
	return realbuttons;
}
