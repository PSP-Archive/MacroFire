/* mfMenuSendSignal.c */

#include "mfmenu_types.h"

void mfMenuSendSignal( MfSignalMessage sig )
{
	__mfmenu__gParams->Message.Signal = sig;
}
