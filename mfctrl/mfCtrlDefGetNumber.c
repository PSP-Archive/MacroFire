/* mfCtrlDefGetNumber.c */

#include "mfctrl_types.h"

static unsigned int mf_ctrl_var_get_num( void *num, unsigned int max );
static void mf_ctrl_var_set_num( void *num, unsigned int max, unsigned int value );

int mfCtrlDefGetNumber( MfMessage message, const char *label, void *var, void *pref, void *ex )
{
	unsigned int value = mf_ctrl_var_get_num( var, ((MfCtrlDefGetNumberPref *)pref)->max );
	int ret = CG_ERROR_OK;
	
	switch( mfMenuMaskMainMessage( message ) ){
		case MF_CM_INIT:
		case MF_CM_TERM:
			break;
		case MF_CM_LABEL:
			if( pref && ((MfCtrlDefGetNumberPref *)pref)->unit ){
				mfCtrlSetLabel( ex, "%s: %d %s", label, value, ((MfCtrlDefGetNumberPref *)pref)->unit );
			} else{
				mfCtrlSetLabel( ex, "%s: %d", label, value );
			}
			break;
		case MF_CM_INFO:
			mfMenuSetInfoText( *((unsigned int *)ex) | MF_MENU_INFOTEXT_COMMON_CTRL | MF_MENU_INFOTEXT_SET_LOWER_LINE, "(%s)%s, (%s)%s, (%s)%s", PB_SYM_PSP_SQUARE, MF_STR_CTRL_DECREASE, mfMenuAcceptSymbol(), MF_STR_CTRL_INCREASE, PB_SYM_PSP_TRIANGLE, MF_STR_CTRL_EDIT );
			break;
		case MF_CM_PROC:
			if( mfMenuIsPressed( PSP_CTRL_SQUARE ) ){
				if( pref && ((MfCtrlDefGetNumberPref *)pref)->width ){
					if( value >= ((MfCtrlDefGetNumberPref *)pref)->width ) value -= ((MfCtrlDefGetNumberPref *)pref)->width;
				} else if( value > ((MfCtrlDefGetNumberPref *)pref)->min ){
					value--;
				}
			} else if( mfMenuIsPressed( mfMenuAcceptButton() ) ){
				if( pref && ((MfCtrlDefGetNumberPref *)pref)->width ){
					if( ((MfCtrlDefGetNumberPref *)pref)->max - value >= ((MfCtrlDefGetNumberPref *)pref)->width ) value += ((MfCtrlDefGetNumberPref *)pref)->width;
				} else if( value < ((MfCtrlDefGetNumberPref *)pref)->max ){
					value++;
				}
			} else if( mfMenuIsPressed( PSP_CTRL_TRIANGLE ) ){
				ret = mfDialogNumedit( label, ((MfCtrlDefGetNumberPref *)pref)->unit, var, ((MfCtrlDefGetNumberPref *)pref)->max );
			}
			break;
	}
	
	mf_ctrl_var_set_num( var, ((MfCtrlDefGetNumberPref *)pref)->max, value );
	
	if( CG_IS_ERROR( ret ) ) mfMenuError( ret, NULL );
	return CG_ERROR_OK;
}

static unsigned int mf_ctrl_var_get_num( void *num, unsigned int max )
{
	if( max <= UINT8_MAX ){
		return *((uint8_t *)num);
	} else if( max <= UINT16_MAX ){
		return *((uint16_t *)num);
	} else{
		return *((uint32_t *)num);
	}
}

static void mf_ctrl_var_set_num( void *num, unsigned int max, unsigned int value )
{
	if( max <= UINT8_MAX ){
		*((uint8_t *)num)  = value;
	} else if( max <= UINT16_MAX ){
		*((uint16_t *)num) = value;
	} else{
		*((uint32_t *)num) = value;
	}
}
