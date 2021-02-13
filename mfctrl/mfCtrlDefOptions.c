/* mfCtrlDefOptions.c */

#include "mfctrl_types.h"

int mfCtrlDefOptions( MfMessage message, const char *label, void *var, void *items, void *ex )
{
	switch( mfMenuMaskMainMessage( message ) ){
		case MF_CM_INIT:
		case MF_CM_TERM:
			 break;
		case MF_CM_LABEL:
			if( items ){
				mfCtrlSetLabel( ex, "%s: %s", label, ((char **)items)[*((unsigned int *)var)] );
			} else{
				mfCtrlSetLabel( ex, "%s: Option%d", label, *((unsigned int *)var) );
			}
			break;
		case MF_CM_INFO:
			mfMenuSetInfoText( *((unsigned int *)ex) | MF_MENU_INFOTEXT_COMMON_CTRL | MF_MENU_INFOTEXT_SET_LOWER_LINE, "(%s/%s)%s", PB_SYM_PSP_SQUARE, mfMenuAcceptSymbol(), MF_STR_CTRL_CHANGE_OPTION );
			break;
		case MF_CM_PROC:
			if( mfMenuIsPressed( mfMenuAcceptButton() ) ){
				(*((unsigned int *)var))++;
				if( ! ((char **)items)[*((unsigned int *)var)] ) *((unsigned int *)var) = 0;
			} else if( mfMenuIsPressed( PSP_CTRL_SQUARE ) ){
				if( ! *((unsigned int *)var) ){
					while( ((char **)items)[*((unsigned int *)var) + 1] ) (*((unsigned int *)var))++;
				} else{
					(*((unsigned int *)var))--;
				}
			}
			break;
	}
	return CG_ERROR_OK;
}
