/* mfCtrlSetLabel.c */

#include "mfctrl_types.h"

void mfCtrlSetLabel( void *arg, char *format, ... )
{
	va_list ap;
	
	va_start( ap, format );
	vsnprintf( (char *)arg, MF_CTRL_BUFFER_LENGTH, format, ap );
	va_end( ap );
}
