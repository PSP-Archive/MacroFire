/* ovmsgShutdown.c */

#include "ovmsg_types.h"

void ovmsgShutdown( OvmsgUID uid )
{
	sceKernelSetEventFlag( ((struct ovmsg_params *)uid)->workEvId, OVMSG_EVENT_SHUTDOWN );
	sceKernelWaitEventFlag( ((struct ovmsg_params *)uid)->ctrlEvId, OVMSG_EVENT_OK, PSP_EVENT_WAITOR | PSP_EVENT_WAITCLEAR, NULL, 0 );
}
