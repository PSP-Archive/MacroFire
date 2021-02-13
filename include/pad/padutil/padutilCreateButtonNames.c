/* padutilCreateButtonNames.c */

#include "padutil_types.h"

static PadutilButtonName *st_names;
static unsigned int      st_names_refcount;

PadutilButtonName *padutilCreateButtonNames( void )
{
	if( st_names ){
		st_names_refcount++;
		return st_names;
	}
	
	st_names = (PadutilButtonName *)memoryExalloc( "PadutilButtonNames", MEMORY_USER, 0, sizeof( PadutilButtonName ) * 33, PSP_SMEM_High, NULL );
	if( ! st_names ) return NULL;
	
	st_names[0].button = padutilSetPad( PSP_CTRL_SELECT );
	st_names[0].name   = "SELECT";
	st_names[1].button = padutilSetPad( PSP_CTRL_START );
	st_names[1].name   = "START";
	
	st_names[2].button = padutilSetPad( PSP_CTRL_UP );
	st_names[2].name   = "Up";
	st_names[3].button = padutilSetPad( PSP_CTRL_RIGHT );
	st_names[3].name   = "Right";
	st_names[4].button = padutilSetPad( PSP_CTRL_DOWN );
	st_names[4].name   = "Down";
	st_names[5].button = padutilSetPad( PSP_CTRL_LEFT );
	st_names[5].name   = "Left";
	
	st_names[6].button = padutilSetPad( PADUTIL_CTRL_ANALOG_UP );
	st_names[6].name   = "AnalogUp";
	st_names[7].button = padutilSetPad( PADUTIL_CTRL_ANALOG_RIGHT );
	st_names[7].name   = "AnalogRight";
	st_names[8].button = padutilSetPad( PADUTIL_CTRL_ANALOG_DOWN );
	st_names[8].name   = "AnalogDown";
	st_names[9].button = padutilSetPad( PADUTIL_CTRL_ANALOG_LEFT );
	st_names[9].name   = "AnalogLeft";
	
	st_names[10].button = padutilSetPad( PSP_CTRL_LTRIGGER );
	st_names[10].name   = "LTrigger";
	st_names[11].button = padutilSetPad( PSP_CTRL_RTRIGGER );
	st_names[11].name   = "RTrigger";
	
	st_names[12].button = padutilSetPad( PSP_CTRL_TRIANGLE );
	st_names[12].name   = "Triangle";
	st_names[13].button = padutilSetPad( PSP_CTRL_CIRCLE );
	st_names[13].name   = "Circle";
	st_names[14].button = padutilSetPad( PSP_CTRL_CROSS );
	st_names[14].name   = "Cross";
	st_names[15].button = padutilSetPad( PSP_CTRL_SQUARE );
	st_names[15].name   = "Square";
	
	st_names[16].button = padutilSetPad( PSP_CTRL_HOME );
	st_names[16].name   = "HOME";
	st_names[17].button = padutilSetPad( PSP_CTRL_HOLD );
	st_names[17].name   = "HOLD";
	st_names[18].button = padutilSetPad( PSP_CTRL_NOTE );
	st_names[18].name   = "NOTE";
	st_names[19].button = padutilSetPad( PSP_CTRL_SCREEN );
	st_names[19].name   = "SCREEN";
	
	st_names[20].button = padutilSetPad( PSP_CTRL_VOLUP );
	st_names[20].name   = "VolUp";
	st_names[21].button = padutilSetPad( PSP_CTRL_VOLDOWN );
	st_names[21].name   = "VolDown";
	
	st_names[22].button = padutilSetPad( PSP_CTRL_WLAN_UP );
	st_names[22].name   = "WLAN";
	st_names[23].button = padutilSetPad( PSP_CTRL_REMOTE );
	st_names[23].name   = "REMOTE";
	st_names[24].button = padutilSetPad( PSP_CTRL_DISC );
	st_names[24].name   = "DISC";
	st_names[25].button = padutilSetPad( PSP_CTRL_MS );
	st_names[25].name   = "MS";
	
	st_names[26].button = padutilSetHprm( PSP_HPRM_PLAYPAUSE );
	st_names[26].name   = "RPlayPause";
	st_names[27].button = padutilSetHprm( PSP_HPRM_FORWARD );
	st_names[27].name   = "RForward";
	st_names[28].button = padutilSetHprm( PSP_HPRM_BACK );
	st_names[28].name   = "RBack";
	st_names[29].button = padutilSetHprm( PSP_HPRM_VOL_UP );
	st_names[29].name   = "RVolUp";
	st_names[30].button = padutilSetHprm( PSP_HPRM_VOL_DOWN );
	st_names[30].name   = "RVolDown";
	st_names[31].button = padutilSetHprm( PSP_HPRM_HOLD );
	st_names[31].name   = "RHOLD";
	
	st_names[32].button = 0;
	st_names[32].name   = NULL;
	
	st_names_refcount++;
	return st_names;
}

void padutilDestroyButtonNames( void )
{
	if( st_names_refcount ){
		st_names_refcount--;
		if( ! st_names_refcount ){
			memoryFree( st_names );
			st_names = NULL;
		}
	}
}
