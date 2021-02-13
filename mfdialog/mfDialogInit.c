/* mfDialogInit.c */

#define MFDIALOG_INIT
#include "mfdialog_types.h"
#undef MFDIALOG_INIT

void mfDialogInit( PadutilRemap *remap )
{
	cdialogInit();
	cdialogSetRemap( remap );
}
