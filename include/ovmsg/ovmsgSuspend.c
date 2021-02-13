/* ovmsgSuspend.c */

#include "ovmsg_types.h"

void ovmsgSuspend( OvmsgUID uid )
{
	sceKernelSetEventFlag( ((struct ovmsg_params *)uid)->workEvId, OVMSG_EVENT_SUSPEND );
}
