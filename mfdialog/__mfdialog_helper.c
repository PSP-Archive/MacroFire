/* mfDialogCurrentType.c */

#include "mfdialog_types.h"

int __mfdialog_helper_draw( CdialogStatus ( *statf )( void ), int ( *updatef )( void ) )
{
	CdialogStatus status;
	int ret;
	
	if( CG_IS_ERROR( ( ret = updatef() ) ) ) return ret;
	
	status = statf();
	if( status == CDIALOG_SHUTDOWN ){
		return MFDIALOG_DRAW_FINISH;
	} else{
		return MFDIALOG_DRAW_CONTINUE;
	}
}

bool __mfdialog_helper_result( int ( *shutdownf )( void ), CdialogResult ( *resultf )( void ), void ( *destroyf )( void ) )
{
	CdialogResult result;
	
	shutdownf();
	result = resultf();
	
	destroyf();
	
	mfMenuEnable( MF_MENU_EXIT );
	__mfdialog__gDialogType = MFDIALOG_NONE;
	__mfdialog__gDrawFunc   = NULL;
	__mfdialog__gResultFunc = NULL;
	
	
	__mfdialog__gLastResult = result == CDIALOG_ACCEPT ? true : false;
	
	return __mfdialog__gLastResult;
}
