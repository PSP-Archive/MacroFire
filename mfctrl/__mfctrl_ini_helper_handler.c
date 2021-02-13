/* __mfctrl_ini_helper.c */

#include "mfctrl_types.h"
#include "mfctrl_ini_helper.h"

void __mf_ctrl_ini_helper_handler( MfMessage message, void *arg )
{
	struct __mf_ctrl_ini_helper_params *params = arg;
	
	if( message & MF_MM_INIT ){
		unsigned int options;
		
		if( params->action == MF_CTRL_INI_SAVE ){
			options = CDIALOG_GETFILENAME_SAVE | CDIALOG_GETFILENAME_OVERWRITEPROMPT;
		} else{
			options = CDIALOG_GETFILENAME_OPEN | CDIALOG_GETFILENAME_FILEMUSTEXIST;
		}
		
		if( CG_IS_ERROR( mfDialogGetfilename( params->pref->title, params->pref->initDir, params->pref->initFilename, params->path, params->pref->pathMax, options ) ) ){
			mfMenuSendSignal( MF_CTRL_DEF_INI_SIG_FINISH );
		}
	} else if( message & MF_MM_PROC ){
		 if( message & MF_CTRL_DEF_INI_SIG_ACCEPT ){
			int ret = ( params->proc )( params->action, params->path );
			if( ret != CG_ERROR_OK ) mfMenuError( ret, NULL );
			mfMenuSendSignal( MF_CTRL_DEF_INI_SIG_FINISH );
		} else if( message & MF_CTRL_DEF_INI_SIG_FINISH ){
			mfMenuMemoryFree( params->path );
			mfMenuMemoryFree( params );
			mfMenuExitExtra();
		} else if( message & MF_DM_FINISH ){
			if( ! mfDialogLastResult() ){
				mfMenuSendSignal( MF_CTRL_DEF_INI_SIG_FINISH );
			} else{
				mfMenuSetInfoText( MF_MENU_INFOTEXT_SET_MIDDLE_LINE, params->action == MF_CTRL_INI_SAVE ? MF_STR_SAVING : MF_STR_LOADING, params->path );
				mfMenuSendSignal( MF_CTRL_DEF_INI_SIG_ACCEPT );
			}
		}
	}
}

