/*=========================================================

	sysclib.h
	
	SysclibForKernelに存在するのに
	
		USE_KERNEL_LIBC = 1
	
	を指定すると見つからないプロトタイプの補完。
	力業でgccの警告対策。
	
=========================================================*/
#ifndef PSP_SYSCLIB_H
#define PSP_SYSCLIB_H

#include <pspkerneltypes.h>

char *strtok_r( char *str, const char *delim, char **save_ptr );

/* 機能していない */
int truncate( const char *path, SceOff length );

#endif
