/* mfMenuExitExtra.c */

#include "mfmenu_types.h"

int mfMenuExitExtra( void )
{
	if( ! __mfmenu__gParams->Extra.Proc ) return CG_ERROR_NOT_INITIALIZED_YET;
	
	( __mfmenu__gParams->Extra.Proc )( MF_MM_TERM, __mfmenu__gParams->Extra.Argp );
	__mfmenu__gParams->Extra.Proc = NULL;
	__mfmenu__gParams->Extra.Argp = NULL;
	mfMenuResetKeyRepeat();
	
	return CG_ERROR_OK;
}
