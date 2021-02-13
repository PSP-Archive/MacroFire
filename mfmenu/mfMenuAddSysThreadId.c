/* mfMenuAddSysThreadId.c */

#include "mfmenu_types.h"

bool mfMenuAddSysThreadId( SceUID thid )
{
	if( __mfmenu__gSysThreads.Count >= MF_MENU_MAX_NUMBER_OF_THREADS ) return false;
	__mfmenu__gSysThreads.List[__mfmenu__gSysThreads.Count++] = thid;
	return true;
}
