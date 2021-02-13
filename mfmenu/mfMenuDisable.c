/* mfMenuDisable.c */

#include "mfmenu_types.h"

void mfMenuDisable( unsigned int flags )
{
	__mfmenu__gParams->Flags &= ~flags;
}
