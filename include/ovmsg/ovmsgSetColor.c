/* ovmsgSetColor.c */

#include "ovmsg_types.h"

void ovmsgSetColor( OvmsgUID uid, unsigned int fg, unsigned int bg )
{
	((struct ovmsg_params *)uid)->fgcolor = fg;
	((struct ovmsg_params *)uid)->bgcolor = bg;
}
