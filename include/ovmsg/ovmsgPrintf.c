/* ovmsgPrintf.c */

#include "ovmsg_types.h"

int ovmsgPrintf( OvmsgUID uid, const char *format, ... )
{
	va_list ap;
	int ret;
	
	va_start( ap, format );
	ret = ovmsgVprintf( uid, format, ap );
	va_end( ap );
	
	return ret;
}
