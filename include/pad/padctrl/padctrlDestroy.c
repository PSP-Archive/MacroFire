/* padctrlDestroy.c */

#include "padctrl_types.h"

void padctrlDestroy( PadctrlUID uid )
{
	memoryFree( (void *)uid );
}
