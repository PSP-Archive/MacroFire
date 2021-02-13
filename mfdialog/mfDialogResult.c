/* mfDialogResult.c */

#include "mfdialog_types.h"

bool mfDialogResult( void )
{
	return ( __mfdialog__gResultFunc )();
}
