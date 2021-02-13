/* mfMenuError.c */

#include "mfmenu_types.h"

void mfMenuError( int err, const char *msg )
{
	if( ! msg && CG_IS_ERROR( err ) ){
		switch( err ){
			case CG_ERROR_INI_SIGNATURE_MISMATCH:    msg = MF_STR_ERROR_INVALID_CONF;        break;
			case CG_ERROR_INI_VERSION_NOT_SUPPORTED: msg = MF_STR_ERROR_UNSUPPORTED_VERSION; break;
			case CG_ERROR_NOT_ENOUGH_MEMORY:         msg = MF_STR_ERROR_NOT_ENOUGH_MEMORY;   break;
			default: msg = MF_STR_ERROR_COMMON;
		}
	}
	
	if( CG_IS_ERROR( mfDialogMessage( MF_STR_ERROR, msg, err, false ) ) ){
		mfMenuSetInfoText( MF_MENU_INFOTEXT_ERROR | MF_MENU_INFOTEXT_SET_MIDDLE_LINE, "%s(%X): %s", MF_STR_ERROR, err, msg );
		__mfmenu__gParams->WaitMicroSecForUpdate = MF_ERROR_DELAY;
	}
}
