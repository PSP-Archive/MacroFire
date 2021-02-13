/* mfMenuDestroyTables.c */

#include "mfmenu_types.h"

void mfMenuDestroyTables( MfMenuTable *table )
{
	mfMenuMemoryFree( table );
}
