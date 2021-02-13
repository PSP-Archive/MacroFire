/* mfCtrlDefBool.c */

#include "mfctrl_types.h"

int mfCtrlDefBool( MfMessage message, const char *label, void *var, void *unused, void *ex )
{
	switch( mfMenuMaskMainMessage( message ) ){
		case MF_CM_INIT:
		case MF_CM_TERM:
			break;
		case MF_CM_LABEL:
			mfCtrlSetLabel( ex, "%s: %s", label, *((bool *)var) ? MF_STR_CTRL_ON : MF_STR_CTRL_OFF );
			break;
		case MF_CM_INFO:
			mfMenuSetInfoText( *((unsigned int *)ex) | MF_MENU_INFOTEXT_COMMON_CTRL | MF_MENU_INFOTEXT_SET_LOWER_LINE, "(%s/%s)%s", PB_SYM_PSP_SQUARE, mfMenuAcceptSymbol(), MF_STR_CTRL_TOGGLE_SWITCH );
			break;
		case MF_CM_PROC:
			if( mfMenuIsAnyPressed( mfMenuAcceptButton() | PSP_CTRL_SQUARE ) ) *((bool *)var) = *((bool *)var) ? false : true;
			break;
	}
	return CG_ERROR_OK;
}
