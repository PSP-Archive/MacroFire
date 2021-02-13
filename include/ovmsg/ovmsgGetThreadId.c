/* ovmsgGetThreadId.c */

#include "ovmsg_types.h"

SceUID ovmsgGetThreadId( OvmsgUID uid )
{
	return ((struct ovmsg_params *)uid)->selfThreadId;
}
