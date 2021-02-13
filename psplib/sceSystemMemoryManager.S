	.set noreorder

#include "pspimport.s"

	IMPORT_START "SysMemUserForUser",0x40000000
	IMPORT_FUNC  "SysMemUserForUser",0xA291F107,sceKernelMaxFreeMemSize
	IMPORT_FUNC  "SysMemUserForUser",0xF919F628,sceKernelTotalFreeMemSize
