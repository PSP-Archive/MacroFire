/* mfDialogGetfilename.c */

#include "mfdialog_types.h"

static int  mfdialog_getfilename_draw( void );
static bool mfdialog_getfilename_result( void );

int mfDialogGetfilename( const char *title, const char *initdir, const char *initname, char *path, size_t pathmax, unsigned int options )
{
	CdialogGetfilenameData *data;
	int ret;
	
	if( __mfdialog__gDialogType != MFDIALOG_NONE ){
		return CG_ERROR_ALREADY_INITIALIZED;
	} else if( CG_IS_ERROR( ( ret = cdialogGetfilenameInit( NULL ) ) ) ){
		return ret;
	}
	
	dbgprint( "Init dialog: getfilename" );
	
	data = cdialogGetfilenameGetData();
	if( title ) strutilCopy( data->title, title, CDIALOG_GETFILENAME_TITLE_LENGTH );
	data->initialDir  = initdir;
	data->initialName = initname;
	data->path        = path;
	data->pathMax     = pathmax;
	data->options     = CDIALOG_DISPLAY_CENTER | options;
	
	if( CG_IS_ERROR( ( ret = cdialogGetfilenameStart( 0, 0 ) ) ) ){
		cdialogGetfilenameShutdownStart();
		cdialogGetfilenameDestroy();
		return ret;
	}
	
	mfMenuDisable( MF_MENU_EXIT );
	__mfdialog__gDialogType = MFDIALOG_GETFILENAME;
	__mfdialog__gDrawFunc   = mfdialog_getfilename_draw;
	__mfdialog__gResultFunc = mfdialog_getfilename_result;
	
	return ret;
}

static int mfdialog_getfilename_draw( void )
{
	return __mfdialog_helper_draw( cdialogGetfilenameGetStatus, cdialogGetfilenameUpdate );
}

static bool mfdialog_getfilename_result( void )
{
	return __mfdialog_helper_result( cdialogGetfilenameShutdownStart, cdialogGetfilenameGetResult, cdialogGetfilenameDestroy );
}
