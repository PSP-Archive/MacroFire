/* hook_types.h                   */
/* �����Ŏg�p�B�O����Q�Ƃ��Ȃ��B */

#include "../hook.h"

#include <pspkernel.h>
#include <psputilsforkernel.h>
#include <stdbool.h>
#include <string.h>

/* PSPLink����q�� */
struct hook_syscall_table_header
{
	void *next; /* ���̃e�[�u��(���ĉ�?)�ւ̃|�C���^ */
	unsigned int offset;                    /* offset to substract from syscall code (substract���ĉ�?) */
	unsigned int num;                       /* �e�[�u���̑S�G���g���� */
	unsigned int size;                      /* �e�[�u���S�̂̃T�C�Y(�o�C�g�P��) */
};
