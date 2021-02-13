/* mfMenuInit.c */

#define MFMENU_INIT
#include "mfmenu_types.h"
#undef MFMENU_INIT

bool mfMenuInit( void )
{
	if( sceKernelGetThreadmanIdList( SCE_KERNEL_TMID_Thread, __mfmenu__gSysThreads.List, MF_MENU_MAX_NUMBER_OF_THREADS, &(__mfmenu__gSysThreads.Count) ) < 0 ){
		dbgprint( "Failed to get a first thread-id list" );
		return false;
	}
	sceCtrlSetSamplingMode( PSP_CTRL_MODE_ANALOG );
	
	/* Ý’è‰Šú‰» */
	__mfmenu__gPref.LowMemoryMode = false;
	__mfmenu__gPref.Remap         = NULL;
	
	return true;
}
