/* mfDialogDraw.c */

#include "mfdialog_types.h"

int mfDialogDraw( void )
{
	return ( __mfdialog__gDrawFunc )();
}
