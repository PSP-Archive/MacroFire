/*=========================================================

	heap.h

	�q�[�v�Ƃ��Ďg�p���郁�����̈���쐬����B
	�傫���̕������Ă�����̂𕡐��m�ۂ���ꍇ�ɗL���B
	
	�Ƃ͂����A�܂Ƃ��Ȏ����ɂ��悤�Ƃ���ƌ��\��ς����Ȃ̂ŁA�Ƃ肠����������ۂ��B
	���̂Ƃ��둬�x�ʂ͖����B

=========================================================*/
#ifndef HEAP_H
#define HEAP_H

#include <pspkernel.h>
#include <pspsysmem_kernel.h>
#include "memory/memory.h"
#include "cgerrno.h"

/*=========================================================
	�}�N��
=========================================================*/
#define HEAP_LOW  1
#define HEAP_HIGH 2

#define HEAP_HEADER_SIZE sizeof( struct heap_allocated_memblock_header )

#ifdef __cplusplus
extern "C" {
#endif

/*=========================================================
	�^�錾
=========================================================*/
typedef uintptr_t HeapUID;

struct heap_allocated_memblock_header {
	SceSize size;                                /* �g�p���Ă��郁�����T�C�Y(�w�b�_���܂�) */
	struct heap_allocated_memblock_header *next; /* ���̎g�p���������u���b�N�A�h���X */
};

/*=========================================================
	�֐�
=========================================================*/

/*-----------------------------------------------
	heapExCreate
	
	�q�[�v�Ƃ��Ďg�p���郁�����̈���m�ۂ���B
	�w�b�_��K�v�Ƃ��邽�߁A�m�ۂ����T�C�Y�͎w�肵���T�C�Y���w�b�_�T�C�Y(HEAP_HEADER_SIZE)�����傫���Ȃ�B
	
	@param: const char *name
		�������u���b�N���B
	
	@param: MemoryPartition partition
		���������m�ۂ���p�[�e�B�V�����B
	
	@param: SceSize size
		�q�[�v�Ƃ��Ďg�p����T�C�Y�B
	
	@return: HeapUID
		�q�[�v���g�����߂�UID�B
-----------------------------------------------*/
HeapUID heapExCreate( const char *name, MemoryPartition partition, SceSize size, int type );

/*-----------------------------------------------
	heapCreate
	
	�q�[�v�Ƃ��Ďg�p���郁�����̈���m�ۂ���B
	���̊֐��͓�����heapCreateEx()���ĂԁB
	�m�ۂ��郁�����p�[�e�B�V������MEMORY_USER�ŌŒ�B
	
	@param: SceSize size
		�q�[�v�Ƃ��Ďg�p����T�C�Y�B
	
	@return: HeapUID
		�q�[�v���g�����߂�UID�B
-----------------------------------------------*/
HeapUID heapCreate( SceSize size );

/*-----------------------------------------------
	heapDestroy
	
	�m�ۂ����q�[�v���������B
	�q�[�v�̈�S�̂�������邽�߁AheapAlloc()�Ŋ��蓖�Ă��Ă��郁���������݂��Ă��A
	���������킹�Ă��ׂĉ�������B
	��������s������́A���̃q�[�v���������A�h���X�ɑ΂��ăA�N�Z�X���Ă͂����Ȃ��B
	
	@param: HeapUID uid
		heapCreate/heapCreateEx�̕Ԃ�l�B
-----------------------------------------------*/
void heapDestroy( HeapUID uid );

/*-----------------------------------------------
	heapAlloc
	
	�q�[�v����K�v�ȃT�C�Y�̃X�g���[�W���擾����B
	�������A�q�[�v����m�ۂ���T�C�Y�́A�w�肵���T�C�Y�Ƀw�b�_�T�C�Y(HEAP_HEADER_SIZE)���������T�C�Y�ɂȂ�B
	
	@param: HeapUID uid
		heapCreate/heapCreateEx�̕Ԃ�l�B
	
	@param: SceSize size
		���蓖�Ă�T�C�Y�B
	
	@return: void *
		�m�ۂ����������u���b�N���w���|�C���^�B
-----------------------------------------------*/
void *heapAlloc( HeapUID uid, SceSize size );

/*-----------------------------------------------
	heapCalloc
	
	�q�[�v����K�v�ȃT�C�Y�̃X�g���[�W���擾����B
	�������A�q�[�v����m�ۂ���T�C�Y�́A�w�肵���T�C�Y�Ƀw�b�_�T�C�Y(HEAP_HEADER_SIZE)���������T�C�Y�ɂȂ�B
	heapAlloc()�Ɠ������Ƃ����邪�A�m�ی�ɑΏۃ������u���b�N���[���N���A����_���قȂ�B
	
	@param: HeapUID uid
		heapCreate/heapCreateEx�̕Ԃ�l�B
	
	@param: SceSize size
		���蓖�Ă�T�C�Y�B
	
	@return: void *
		�m�ۂ����������u���b�N���w���|�C���^�B
-----------------------------------------------*/
void *heapCalloc( HeapUID uid, SceSize size );

/*-----------------------------------------------
	heapFree
	
	heapAlloc()/heapCalloc()�Ŋm�ۂ����������u���b�N��������A�ė��p�\�ɂ���B
	������ꂽ�������u���b�N�͍Ăуq�[�v�Ƃ��Ďg�����Ƃ��o����B
	
	�q�[�v���̂��̂��K�v�����Ȃ����ꍇ��heapDestroy()���Ăׂ΂悢���A
	���̏ꍇ�A�m�ۂ��Ă��郁�����u���b�N��������悤�Ƃ���heapFree()���ĂԕK�v�͂Ȃ��B
	heapDestroy()�����s�����ƁA���̃q�[�v����m�ۂ��Ă��郁�����u���b�N�͑S�ĉ�������B
-----------------------------------------------*/
int heapFree( HeapUID uid, void *heap );

/*-----------------------------------------------
	heapTotalFreeSize
	
	heapCreate()/heapCreateEx()�Ŋm�ۂ����q�[�v�̈�ɁA
	���v���o�C�g�̋󂫗e�ʂ����݂��邩��Ԃ��B
	����1000�Ƃ������l���Ԃ��Ă��Ă��A�����1000�o�C�g�̘A�������e�ʂł���Ƃ͌���Ȃ��B
	500��500�̋󂫗e�ʂ��c���Ă���̂�������Ȃ����A100�̋󂫗e�ʂ�10�c���Ă���̂�������Ȃ��B
	
	
	@param: HeapUID uid
		heapCreate/heapCreateEx�̕Ԃ�l�B
	
	@return: SceSize
		�󂫗e�ʂ̍��v�B
-----------------------------------------------*/
SceSize heapTotalFreeSize( HeapUID uid );

#ifdef __cplusplus
}
#endif

#endif
