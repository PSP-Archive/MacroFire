/* padutilCreateButtonSymbols.c */

#include "padutil_types.h"
#include "graphic/pb.h"

static PadutilButtonName *st_symbols;
static unsigned int      st_symbols_refcount;

PadutilButtonName *padutilCreateButtonSymbols( void )
{
	if( st_symbols ){
		st_symbols_refcount++;
		return st_symbols;
	}
	
	st_symbols = (PadutilButtonName *)memoryExalloc( "PadutilButtonSymbols", MEMORY_USER, 0, sizeof( PadutilButtonName ) * 33, PSP_SMEM_High, NULL );
	if( ! st_symbols ) return NULL;
	
	st_symbols[0].button = padutilSetPad( PSP_CTRL_SELECT );
	st_symbols[0].name   = "SELECT";
	st_symbols[1].button = padutilSetPad( PSP_CTRL_START );
	st_symbols[1].name   = "START";
	
	st_symbols[2].button = padutilSetPad( PSP_CTRL_UP );
	st_symbols[2].name   = PB_SYM_PSP_UP;
	st_symbols[3].button = padutilSetPad( PSP_CTRL_RIGHT );
	st_symbols[3].name   = PB_SYM_PSP_RIGHT;
	st_symbols[4].button = padutilSetPad( PSP_CTRL_DOWN );
	st_symbols[4].name   = PB_SYM_PSP_DOWN;
	st_symbols[5].button = padutilSetPad( PSP_CTRL_LEFT );
	st_symbols[5].name   = PB_SYM_PSP_LEFT;
	
	st_symbols[6].button = padutilSetPad( PADUTIL_CTRL_ANALOG_UP );
	st_symbols[6].name   = "A"PB_SYM_PSP_UP;
	st_symbols[7].button = padutilSetPad( PADUTIL_CTRL_ANALOG_RIGHT );
	st_symbols[7].name   = "A"PB_SYM_PSP_RIGHT;
	st_symbols[8].button = padutilSetPad( PADUTIL_CTRL_ANALOG_DOWN );
	st_symbols[8].name   = "A"PB_SYM_PSP_DOWN;
	st_symbols[9].button = padutilSetPad( PADUTIL_CTRL_ANALOG_LEFT );
	st_symbols[9].name   = "A"PB_SYM_PSP_LEFT;
	
	st_symbols[10].button = padutilSetPad( PSP_CTRL_LTRIGGER );
	st_symbols[10].name   = "L";
	st_symbols[11].button = padutilSetPad( PSP_CTRL_RTRIGGER );
	st_symbols[11].name   = "R";
	
	st_symbols[12].button = padutilSetPad( PSP_CTRL_TRIANGLE );
	st_symbols[12].name   = PB_SYM_PSP_TRIANGLE;
	st_symbols[13].button = padutilSetPad( PSP_CTRL_CIRCLE );
	st_symbols[13].name   = PB_SYM_PSP_CIRCLE;
	st_symbols[14].button = padutilSetPad( PSP_CTRL_CROSS );
	st_symbols[14].name   = PB_SYM_PSP_CROSS;
	st_symbols[15].button = padutilSetPad( PSP_CTRL_SQUARE );
	st_symbols[15].name   = PB_SYM_PSP_SQUARE;
	
	st_symbols[16].button = padutilSetPad( PSP_CTRL_HOME );
	st_symbols[16].name   = "HOME";
	st_symbols[17].button = padutilSetPad( PSP_CTRL_HOLD );
	st_symbols[17].name   = "HOLD";
	st_symbols[18].button = padutilSetPad( PSP_CTRL_NOTE );
	st_symbols[18].name   = PB_SYM_PSP_NOTE;
	st_symbols[19].button = padutilSetPad( PSP_CTRL_SCREEN );
	st_symbols[19].name   = "SCREEN";
	
	st_symbols[20].button = padutilSetPad( PSP_CTRL_VOLUP );
	st_symbols[20].name   = "VOL+";
	st_symbols[21].button = padutilSetPad( PSP_CTRL_VOLDOWN );
	st_symbols[21].name   = "VOL-";
	
	st_symbols[22].button = padutilSetPad( PSP_CTRL_WLAN_UP );
	st_symbols[22].name   = "WLAN";
	st_symbols[23].button = padutilSetPad( PSP_CTRL_REMOTE );
	st_symbols[23].name   = "REMOTE";
	st_symbols[24].button = padutilSetPad( PSP_CTRL_DISC );
	st_symbols[24].name   = "DISC";
	st_symbols[25].button = padutilSetPad( PSP_CTRL_MS );
	st_symbols[25].name   = "MS";
	
	st_symbols[26].button = padutilSetHprm( PSP_HPRM_PLAYPAUSE );
	st_symbols[26].name   = "RPLAY";
	st_symbols[27].button = padutilSetHprm( PSP_HPRM_FORWARD );
	st_symbols[27].name   = "RNEXT";
	st_symbols[28].button = padutilSetHprm( PSP_HPRM_BACK );
	st_symbols[28].name   = "RPREV";
	st_symbols[29].button = padutilSetHprm( PSP_HPRM_VOL_UP );
	st_symbols[29].name   = "RVOL+";
	st_symbols[30].button = padutilSetHprm( PSP_HPRM_VOL_DOWN );
	st_symbols[30].name   = "RVOL-";
	st_symbols[31].button = padutilSetHprm( PSP_HPRM_HOLD );
	st_symbols[31].name   = "RHOLD";
	
	st_symbols[32].button = 0;
	st_symbols[32].name   = NULL;
	
	st_symbols_refcount++;
	return st_symbols;
}

void padutilDestroyButtonSymbols( void )
{
	if( st_symbols_refcount ){
		st_symbols_refcount--;
		if( ! st_symbols_refcount ){
			memoryFree( st_symbols );
			st_symbols = NULL;
		}
	}
}
