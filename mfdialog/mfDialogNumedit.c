/* mfDialogNumedit.c */

#include "mfdialog_types.h"

static int  mfdialog_numedit_draw( void );
static bool mfdialog_numedit_result( void );

int mfDialogNumedit( const char *title, const char *unit, void *num, uint32_t max )
{
	CdialogNumeditData *data;
	int ret;
	
	if( __mfdialog__gDialogType != MFDIALOG_NONE ){
		return CG_ERROR_ALREADY_INITIALIZED;
	} else if( CG_IS_ERROR( ( ret = cdialogNumeditInit( NULL ) ) ) ){
		return ret;
	}
	
	dbgprint( "Init dialog: numedit" );
	
	data = cdialogNumeditGetData();
	if( title ) strutilCopy( data->title, title, CDIALOG_NUMEDIT_TITLE_LENGTH );
	if( unit  ) strutilCopy( data->unit,  unit,  CDIALOG_NUMEDIT_UNIT_LENGTH );
	data->num = num;
	data->max = max;
	data->options = CDIALOG_DISPLAY_CENTER;
	
	if( CG_IS_ERROR( ( ret = cdialogNumeditStart( 0, 0 ) ) ) ){
		cdialogNumeditShutdownStart();
		cdialogNumeditDestroy();
		return ret;
	}
	
	mfMenuDisable( MF_MENU_EXIT );
	__mfdialog__gDialogType = MFDIALOG_NUMEDIT;
	__mfdialog__gDrawFunc   = mfdialog_numedit_draw;
	__mfdialog__gResultFunc = mfdialog_numedit_result;
	
	return ret;
}

static int mfdialog_numedit_draw( void )
{
	return __mfdialog_helper_draw( cdialogNumeditGetStatus, cdialogNumeditUpdate );
}

static bool mfdialog_numedit_result( void )
{
	return __mfdialog_helper_result( cdialogNumeditShutdownStart, cdialogNumeditGetResult, cdialogNumeditDestroy );
}
