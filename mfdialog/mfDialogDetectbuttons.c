/* mfDialogDetectbuttons.c */

#include "mfdialog_types.h"

static int  mfdialog_detectbuttons_draw( void );
static bool mfdialog_detectbuttons_result( void );

int mfDialogDetectbuttons( const char *title, PadutilButtons availbtns, PadutilButtons *buttons )
{
	CdialogDetectbuttonsData *data;
	int ret;
	
	if( __mfdialog__gDialogType != MFDIALOG_NONE ){
		return CG_ERROR_ALREADY_INITIALIZED;
	} else if( CG_IS_ERROR( ( ret = cdialogDetectbuttonsInit( NULL ) ) ) ){
		return ret;
	}
	
	dbgprint( "Init dialog: detectbuttons" );
	
	data = cdialogDetectbuttonsGetData();
	if( title ) strutilCopy( data->title, title, CDIALOG_DETECTBUTTONS_TITLE_LENGTH );
	data->availButtons = availbtns;
	data->buttons      = buttons;
	data->options      = CDIALOG_DISPLAY_CENTER;
	
	if( CG_IS_ERROR( ( ret = cdialogDetectbuttonsStart( 0, 0 ) ) ) ){
		cdialogDetectbuttonsShutdownStart();
		cdialogDetectbuttonsDestroy();
		return ret;
	}
	
	mfMenuDisable( MF_MENU_EXIT );
	__mfdialog__gDialogType = MFDIALOG_DETECTBUTTONS;
	__mfdialog__gDrawFunc   = mfdialog_detectbuttons_draw;
	__mfdialog__gResultFunc = mfdialog_detectbuttons_result;
	
	return ret;
}

static int mfdialog_detectbuttons_draw( void )
{
	return __mfdialog_helper_draw( cdialogDetectbuttonsGetStatus, cdialogDetectbuttonsUpdate );
}

static bool mfdialog_detectbuttons_result( void )
{
	return __mfdialog_helper_result( cdialogDetectbuttonsShutdownStart, cdialogDetectbuttonsGetResult, cdialogDetectbuttonsDestroy );
}

