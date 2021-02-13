/* mfDialogSosk.c */

#include "mfdialog_types.h"

static int  mfdialog_sosk_draw( void );
static bool mfdialog_sosk_result( void );

int mfDialogSosk( const char *title, char *text, size_t length, unsigned int availkb )
{
	CdialogSoskData *data;
	int ret;
	
	if( __mfdialog__gDialogType != MFDIALOG_NONE ){
		return CG_ERROR_ALREADY_INITIALIZED;
	} else if( CG_IS_ERROR( ( ret = cdialogSoskInit( NULL ) ) ) ){
		return ret;
	}
	
	dbgprint( "Init dialog: sosk" );
	
	data = cdialogSoskGetData();
	if( title ) strutilCopy( data->title, title, CDIALOG_SOSK_TITLE_LENGTH );
	data->text    = text;
	data->textMax = length;
	data->types   = availkb;
	data->options = CDIALOG_DISPLAY_CENTER;
	
	if( CG_IS_ERROR( ( ret = cdialogSoskStart( 0, 0 ) ) ) ){
		cdialogSoskShutdownStart();
		cdialogSoskDestroy();
		return ret;
	}
	
	mfMenuDisable( MF_MENU_EXIT );
	__mfdialog__gDialogType = MFDIALOG_SOSK;
	__mfdialog__gDrawFunc   = mfdialog_sosk_draw;
	__mfdialog__gResultFunc = mfdialog_sosk_result;
	
	return ret;
}

static int mfdialog_sosk_draw( void )
{
	return __mfdialog_helper_draw( cdialogSoskGetStatus, cdialogSoskUpdate );
}

static bool mfdialog_sosk_result( void )
{
	return __mfdialog_helper_result( cdialogSoskShutdownStart, cdialogSoskGetResult, cdialogSoskDestroy );
}
