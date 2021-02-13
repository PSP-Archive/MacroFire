/* mfDialogMessage.c */

#include "mfdialog_types.h"

static int  mfdialog_message_draw( void );
static bool mfdialog_message_result( void );

int mfDialogMessage( const char *title, const char *message, unsigned int error, bool yesno )
{
	CdialogMessageData *data;
	int ret;
	
	dbgprint( "Init dialog: message" );
	
	if( __mfdialog__gDialogType != MFDIALOG_NONE ){
		return CG_ERROR_ALREADY_INITIALIZED;
	} else if( CG_IS_ERROR( ( ret = cdialogMessageInit( NULL ) ) ) ){
		return ret;
	}
	
	data = cdialogMessageGetData();
	if( title   ) strutilCopy( data->title,   title,   CDIALOG_MESSAGE_TITLE_LENGTH );
	if( message ) strutilCopy( data->message, message, CDIALOG_MESSAGE_LENGTH );
	data->options = CDIALOG_DISPLAY_CENTER;
	if( error ){
		data->options   |= CDIALOG_MESSAGE_ERROR;
		data->errorcode = error;
	}
	if( yesno ) data->options |= CDIALOG_MESSAGE_YESNO;
	
	if( CG_IS_ERROR( ( ret = cdialogMessageStart( 0, 0 ) ) ) ){
		cdialogMessageShutdownStart();
		cdialogMessageDestroy();
		return ret;
	}
	
	mfMenuDisable( MF_MENU_EXIT );
	__mfdialog__gDialogType = MFDIALOG_MESSAGE;
	__mfdialog__gDrawFunc   = mfdialog_message_draw;
	__mfdialog__gResultFunc = mfdialog_message_result;
	
	return ret;
}

static int mfdialog_message_draw( void )
{
	return __mfdialog_helper_draw( cdialogMessageGetStatus, cdialogMessageUpdate );
}

static bool mfdialog_message_result( void )
{
	return __mfdialog_helper_result( cdialogMessageShutdownStart, cdialogMessageGetResult, cdialogMessageDestroy );
}
