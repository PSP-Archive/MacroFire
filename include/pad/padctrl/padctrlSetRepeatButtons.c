/* padctrlSetRepeatButtons.c */

#include "padctrl_types.h"

void padctrlSetRepeatButtons( PadctrlUID uid, PadutilButtons buttons )
{
	((struct padctrl_params *)uid)->buttons = buttons;
}
