/* mfdialog_types.h */

#include "../mfdialog.h"
#include "util/strutil.h"

#ifdef MFDIALOG_INIT
#define GLOBAL
#define INIT( x ) = x
#else
#define GLOBAL extern
#define INIT( x )
#endif

GLOBAL MfDialogType __mfdialog__gDialogType      INIT( MFDIALOG_NONE );
GLOBAL bool __mfdialog__gLastResult              INIT( true );
GLOBAL int  ( *__mfdialog__gDrawFunc )( void )   INIT( NULL );
GLOBAL bool ( *__mfdialog__gResultFunc )( void ) INIT( NULL );

int  __mfdialog_helper_draw( CdialogStatus ( *statf )( void ), int ( *updatef )( void ) );
bool __mfdialog_helper_result( int ( *shutdownf )( void ), CdialogResult ( *resultf )( void ), void ( *destroyf )( void ) );

#undef GLOBAL
#undef INIT