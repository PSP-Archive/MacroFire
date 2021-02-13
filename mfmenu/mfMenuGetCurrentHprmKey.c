/* mfMenuGetCurrentHprmKey.c */

#include "mfmenu_types.h"

MfHprmKey mfMenuGetCurrentHprmKey( void )
{
	return *(__mfmenu__gParams->Ctrl.HprmKey);
}
