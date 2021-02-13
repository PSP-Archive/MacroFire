/* mfMenuSetExtra.c */

#include "mfmenu_types.h"

int mfMenuSetExtra( MfMenuExtraProc extra, void *argp )
{
	int ret;
	
	if( __mfmenu__gParams->Extra.Proc ) return CG_ERROR_ALREADY_INITIALIZED;
	
	if( ( ret = ( extra )( MF_MM_INIT, argp ) ) == CG_OK ){
		__mfmenu__gParams->Extra.Proc = extra;
		__mfmenu__gParams->Extra.Argp = argp;
	}
	return ret;
}
