/* mfMenuWait.c */

#include "mfmenu_types.h"

void mfMenuWait( unsigned int microsec )
{
	__mfmenu__gParams->WaitMicroSecForUpdate = microsec;
}
