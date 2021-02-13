/* mfMenuDestroy.c */

#include "mfmenu_types.h"

void mfMenuDestroy( void )
{
	if( __mfmenu__gPref.Remap ) padutilDestroyRemapArray( __mfmenu__gPref.Remap );
}
