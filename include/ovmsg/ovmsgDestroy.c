/* ovmsgDestroy.c */

#include "ovmsg_types.h"

void ovmsgDestroy( OvmsgUID uid )
{
	memoryFree( (void *)uid );
}
