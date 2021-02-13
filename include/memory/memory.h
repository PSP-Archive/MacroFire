/*=========================================================

	memory.h

	malloc()/calloc()/realloc()/memalign()/free()���ǂ��B
	
	EBOOT.PBP���쐬����ꍇ�͑f����PSPSDK��malloc����g�����B

=========================================================*/
#ifndef MEMORY_H
#define MEMORY_H

#include <pspkernel.h>

/*=========================================================
	�}�N��
=========================================================*/
#define MEMORY_PAGE_SIZE 256

/* �ǂ�������q�؂����A�������A���C���𒲐�����}�N�� */
#define MEMORY_ALIGN_ADDR( align, addr ) ( ( (uintptr_t)( addr ) + ( align ) - 1 ) & ( ~( ( align ) - 1 ) ) )

#ifdef __cplusplus
extern "C" {
#endif

/*=========================================================
	�^�錾
=========================================================*/
typedef enum {
	MEMORY_KERN_HI = 1,
	MEMORY_USER,
	MEMORY_KERN_HI_MIRROR,
	MEMORY_KERN_LO,
	MEMORY_VOLATILE,
	MEMORY_USER_MIRROR
} MemoryPartition;

/*=========================================================
	�v���g�^�C�v
=========================================================*/
/*-----------------------------------------------
	memoryAlloc
	
	malloc()���ǂ��B
	�����������蓖�Ă�B
-----------------------------------------------*/
#define memoryAlloc( size ) memoryExalloc( "user_space", MEMORY_USER, 0, size, PSP_SMEM_Low, NULL )

/*-----------------------------------------------
	memoryCalloc
	
	calloc()���ǂ��B
	�����������蓖�āA�̈���[���N���A����B
-----------------------------------------------*/
void *memoryCalloc( SceSize nelem, SceSize size );

/*-----------------------------------------------
	memoryRealloc
	
	realloc()���ǂ��B
	���蓖�Ă��������̈�𑝌�������B
	
	���T�C�Y�ɐ��������ꍇ�A���̗̈�͎����I�ɉ�������B
	���s�����ꍇ�́A�Ԃ�l��NULL���Ԃ���̂́A�ˑR�Ƃ��Č��̗̈悪�L���ł��邽�߃��������[�N�ɒ��ӁB
	�ꍇ�ɂ����memoryReallocf()���g���̂�������������Ȃ��B
	
	�|�C���^�A�h���X�͂قƂ�ǂ̏ꍇ�A���̃A�h���X�Ƃ͈Ⴄ�A�h���X��Ԃ��B
	��O�Ƃ��āA�������u���b�N�T�C�Y(256�o�C�g)�ȉ��̑����ŁA�Ȃ����A�ύX��̃T�C�Y���A
	�������u���b�N���E���܂����Ȃ��ꍇ�̓A�h���X�͕ύX����Ȃ��B
-----------------------------------------------*/
void *memoryRealloc( const char *name, void *src_memblock, SceSize new_size );

/*-----------------------------------------------
	memoryReallocf
	
	FreeBSD�Ŏ�������Ă���reallocf()���ǂ��B
	����́A�v�����ꂽ�T�C�Y�փ��T�C�Y�o���Ȃ������ꍇ���A
	���̗̈�������I�ɉ������_�������āAmemoryRealloc()�Ɠ����B
-----------------------------------------------*/
void *memoryReallocf( const char *name, void *src_memblock, SceSize new_size );

/*-----------------------------------------------
	memoryAlign
	
	memalign()���ǂ��B
	���������m�ۂ������ƁA
	�w�肳�ꂽ�A���C�����g�ɍ��킹���|�C���^�A�h���X��Ԃ��B
-----------------------------------------------*/
#define memoryAlign( align, size ) memoryExalloc( "user_aligned_space", MEMORY_USER, align, size, PSP_SMEM_Low, NULL )

/*-----------------------------------------------
	memoryUsableSize
	
	���ۂɊm�ۂ��ꂽ�̈�̃T�C�Y���擾����B
	�v�������������T�C�Y���傫���A�Ȃ��������Ƃ��߂��������y�[�W�T�C�Y�̔{���ɂȂ�B
-----------------------------------------------*/
SceSize memoryUsableSize( const void *memblock );

/*-----------------------------------------------
	memoryExalloc
	
	���ۂɃ������̊��蓖�Ă��s���֐��B
	memoryAlloc()/memoryCalloc()/memoryRealloc()/memoryReallocf()/memoryAlign()��
	�ŏI�I�ɂ��̊֐����Ăяo���B
	
	���������ɁAsceKernelAllocPartitionMemory()���g���Ď��ۂɃ��������m�ۂ���B
	�m�ۂ����T�C�Y�́A�w�肳�ꂽ�T�C�Y�ɉ����āA
	�������Ǘ������L�^���邽�߂ɁA8�o�C�g�����m�ۂ����B
	�������A�m�ۂ����T�C�Y�͕K���y�[�W(256�o�C�g)��؂�ƂȂ�B
	
	partition�́A���̊֐��𒼐ڌĂ�Ń����������蓖�Ă�ꍇ�ɂ̂ݕύX�\�ŁA
	memoryAlloc()�ȂǂŊm�ۂ���ꍇ�́A�K��MEMORY_USER���g�p�����B
	MEMORY_VOLATILE�́A����Ŋm�ۂ�����scePowerVolatileMemLock()�Ƃ�����API���g���ׂ��B
	
	align�ŃA���C�����g���w�肷�邱�Ƃ��ł���B
	�A���C�����g��0���w�肵���ꍇ�́A���̃��C�u�����͓��ɒ������s��Ȃ��B
	
	type�͈ȉ���3���w��\�B
		PSP_SMEM_Low
		PSP_SMEM_High
		PSP_SMEM_Addr
	PSP_SMEM_Low�̓������A�h���X�̏����������烁�������m�ۂ���悤���݁A
	�t��PSP_SMEM_High�̓������A�h���X�̑傫�������烁�������m�ۂ���悤�Ɏ��݂�B
	
	�Ō��PSP_SMEM_Addr�́A�悭�킩��Ȃ�����6������void *addr�Ɏw�肷��A�h���X����m�ۂ�����ۂ��B
	�����炭�A���̃A�h���X�����Ɏg�p����Ă���ꍇ�̓G���[��Ԃ��B
-----------------------------------------------*/
void *memoryExalloc( const char *name, MemoryPartition partition, unsigned int align, SceSize size, int type, void *addr );

/*-----------------------------------------------
	memoryFree
	
	free()���ǂ��B
	�m�ۂ��ꂽ���������������B
-----------------------------------------------*/
int memoryFree( void *memblock );

#ifdef __cplusplus
}
#endif

#endif
