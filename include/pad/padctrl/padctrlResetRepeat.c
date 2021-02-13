/* padctrlResetRepeat.c */

#include "padctrl_types.h"

void padctrlResetRepeat( PadctrlUID uid )
{
	SceCtrlData pad;
	sceCtrlPeekBufferPositive( &pad, 1 );
	
	((struct padctrl_params *)uid)->lastTick    = 0;
	((struct padctrl_params *)uid)->lastButtons = pad.Buttons;
	((struct padctrl_params *)uid)->prePressed  = pad.Buttons & PADUTIL_PAD_NORMAL_BUTTONS;
	((struct padctrl_params *)uid)->doRepeat    = false;

}
