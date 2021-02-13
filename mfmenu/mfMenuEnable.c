/* mfMenuEnable.c */

#include "mfmenu_types.h"

void mfMenuEnable( unsigned int flags )
{
	__mfmenu__gParams->Flags |= flags;
}
