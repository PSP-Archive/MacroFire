/* mfCtrlDefIniSave.c */

#include "mfctrl_types.h"
#include "mfctrl_ini_helper.h"

int mfCtrlDefIniSave( MfMessage message, const char *label, void *saver, void *pref, void *ex )
{
	int ret = CG_ERROR_OK;
	
	switch( mfMenuMaskMainMessage( message ) ){
		case MF_CM_INIT:
		case MF_CM_TERM:
		case MF_CM_LABEL:
			break;
		case MF_CM_INFO:
			mfMenuSetInfoText( *((unsigned int *)ex) | MF_MENU_INFOTEXT_COMMON_CTRL | MF_MENU_INFOTEXT_SET_LOWER_LINE, "(%s)%s", mfMenuAcceptSymbol(), MF_STR_CTRL_SAVEFILE );
			break;
		case MF_CM_PROC:
			if( mfMenuIsPressed( mfMenuAcceptButton() ) ){
				struct __mf_ctrl_ini_helper_params *params = mfMenuMemoryAlloc( sizeof( struct __mf_ctrl_ini_helper_params ) );
				if( params ){
					params->proc   = saver;
					params->pref   = pref;
					params->action = MF_CTRL_INI_SAVE;
					params->path   = mfMenuMemoryAlloc( params->pref->pathMax );
					if( params->path ){
						params->path[0] = '\0';
						ret = mfMenuSetExtra( (MfMenuExtraProc)__mf_ctrl_ini_helper_handler, params );
					} else{
						mfMenuMemoryFree( params );
						ret = CG_ERROR_NOT_ENOUGH_MEMORY;
					}
				} else{
					ret = CG_ERROR_NOT_ENOUGH_MEMORY;
				}
			}
			break;
	}
	
	if( CG_IS_ERROR( ret ) ) mfMenuError( ret, NULL );
	return CG_ERROR_OK;
}
