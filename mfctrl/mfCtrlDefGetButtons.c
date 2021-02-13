/* mfCtrlDefGetButtons.c */

#include "mfctrl_types.h"

int mfCtrlDefGetButtons( MfMessage message, const char *label, void *var, void *pref, void *ex )
{
	int ret = CG_ERROR_OK;
	
	switch( mfMenuMaskMainMessage( message ) ){
		case MF_CM_INIT:
		case MF_CM_TERM:
		case MF_CM_LABEL:
			break;
		case MF_CM_INFO:
			mfMenuSetInfoText( *((unsigned int *)ex) | MF_MENU_INFOTEXT_COMMON_CTRL | MF_MENU_INFOTEXT_SET_LOWER_LINE, "(%s)%s", mfMenuAcceptSymbol(), MF_STR_CTRL_ASSIGN );
			break;
		case MF_CM_PROC:
			if( mfMenuIsPressed( mfMenuAcceptButton() ) ){
				ret = mfDialogDetectbuttons( label, ((MfCtrlDefGetButtonsPref *)pref)->availButtons, (PadutilButtons *)var );
			}
			break;
	}
	
	if( CG_IS_ERROR( ret ) ) mfMenuError( ret, NULL );
	return CG_ERROR_OK;
}
