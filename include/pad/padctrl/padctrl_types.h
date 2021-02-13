/* padctrl_types.h                */
/* 内部で使用。外から参照しない。 */

#include "../padctrl.h"

struct padctrl_params {
	unsigned int buttons;
	uint64_t     lastTick;
	unsigned int lastButtons;
	unsigned int prePressed;
	bool         doRepeat;
};

void __padctrl_proc( PadctrlUID uid, SceCtrlData *pad, int count );
