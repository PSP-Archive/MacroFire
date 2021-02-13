/* ovmsgVprintf.c */

#include "ovmsg_types.h"

int ovmsgVprintf( OvmsgUID uid, const char *format, va_list ap )
{
	int ret;
	
	sceKernelSetEventFlag( ((struct ovmsg_params *)uid)->workEvId, OVMSG_EVENT_PRINT );
	sceKernelWaitEventFlag( ((struct ovmsg_params *)uid)->ctrlEvId, OVMSG_EVENT_OK, PSP_EVENT_WAITOR | PSP_EVENT_WAITCLEAR, NULL, 0 );
	ret = vsnprintf( ((struct ovmsg_params *)uid)->message, OVMSG_MESSAGE_MAX_LENGTH, format, ap );
	sceKernelSetEventFlag( ((struct ovmsg_params *)uid)->workEvId, OVMSG_EVENT_OK );
	
	return ret;
}
