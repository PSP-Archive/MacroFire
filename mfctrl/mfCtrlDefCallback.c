/* mfCtrlDefCallback.c */

#include "mfctrl_types.h"

int mfCtrlDefCallback( MfMessage message, const char *label, void *func, void *arg, void *ex )
{
	switch( mfMenuMaskMainMessage( message ) ){
		case MF_CM_INIT:
		case MF_CM_TERM:
		case MF_CM_LABEL:
			break;
		case MF_CM_INFO:
			mfMenuSetInfoText( *((unsigned int *)ex) | MF_MENU_INFOTEXT_COMMON_CTRL | MF_MENU_INFOTEXT_SET_LOWER_LINE, "(%s)%s", mfMenuAcceptSymbol(), MF_STR_CTRL_ENTER );
			break;
		case MF_CM_PROC:
			if( mfMenuIsPressed( mfMenuAcceptButton() ) ){
				mfMenuProc( (MfMenuProc)func );
			}
			break;
	}
	return CG_ERROR_OK;
}
