/* padctrlReadBuffer.c */

#include "padctrl_types.h"

unsigned int padctrlReadBuffer( PadctrlUID uid, SceCtrlData *pad, int count )
{
	unsigned int realbuttons;
	
	sceCtrlReadBufferPositive( pad, count );
	realbuttons = pad->Buttons;
	
	__padctrl_proc( uid, pad, count );
	return realbuttons;
}
