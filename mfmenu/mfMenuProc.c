/* mfMenuProc.c */

#include "mfmenu_types.h"

void mfMenuProc( MfMenuProc proc )
{
	__mfmenu__gParams->NextProc = proc;
}
