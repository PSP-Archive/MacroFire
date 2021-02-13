/*=========================================================

	sysclib.h
	
	SysclibForKernel�ɑ��݂���̂�
	
		USE_KERNEL_LIBC = 1
	
	���w�肷��ƌ�����Ȃ��v���g�^�C�v�̕⊮�B
	�͋Ƃ�gcc�̌x���΍�B
	
=========================================================*/
#ifndef PSP_SYSCLIB_H
#define PSP_SYSCLIB_H

#include <pspkerneltypes.h>

char *strtok_r( char *str, const char *delim, char **save_ptr );

/* �@�\���Ă��Ȃ� */
int truncate( const char *path, SceOff length );

#endif
