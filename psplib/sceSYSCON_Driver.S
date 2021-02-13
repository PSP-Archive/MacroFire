	.set noreorder

#include "pspimport.s"

	IMPORT_START "sceSyscon_driver",0x00010000 
	IMPORT_FUNC  "sceSyscon_driver",0x5EE92F3C,sceSysconSetDebugHandlers
