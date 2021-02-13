/*=========================================================

	heap_kernel.h

	�q�[�v�Ƃ��Ďg�p���郁�����̈���쐬����B
	�傫���̕������Ă�����̂𕡐��m�ۂ���ꍇ�ɗL���B
	
	�J�[�l�����[�h��p�B
	SCE��API���g�p���Ċm�ہB

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
#define HEAP_HEADER_SIZE 8 // �q�[�v���烁������Alloc����ꍇ�Ɏw�肵���T�C�Y��肱�̃T�C�Y���傫���m�ۂ����B�����Ǘ��p�̈�B

#ifdef __cplusplus
extern "C" {
#endif

/*=========================================================
	�^�錾
=========================================================*/
typedef SceUID HeapUID;

/*=========================================================
	�֐�
=========================================================*/
/*-----------------------------------------------
	heapExCreate
	
	�q�[�v�Ƃ��Ďg�p���郁�����̈���m�ہB
	�Ǘ��̈��K�v�Ƃ��邽�߁A�m�ۂ���T�C�Y�́A�w�肳�ꂽ�T�C�Y���__HEAP_MGMTAREA_SIZE�����傫���Ȃ�B
	
	@param: const char *name
		�q�[�v�G���A�̖��O�B
	
	@param: MemoryPartition partition
		�m�ۂ��郁�����p�[�e�B�V����
	
	@param: SceSize size
		�m�ۂ���T�C�Y�B
	
	@param: int type
		�m�ۂ���ʒu�B
		HEAP_LOW : �󂫃������̂����A�A�h���X�̏�����������m�ۂ���B
		HEAP_HIGH: �󂫃������̂����A�A�h���X�̑傫��������m�ۂ���B
	
	@return HeapUID
		�q�[�v���g�����߂̃n���h���B
	
-----------------------------------------------*/
HeapUID heapExCreate( const char *name, MemoryPartition partition, SceSize size, int type );

/*-----------------------------------------------
	heapCreate
	
	�q�[�v�Ƃ��Ďg�p���郁�����̈���m�ہB
	�Ǘ��̈��K�v�Ƃ��邽�߁A�m�ۂ���T�C�Y�́A�w�肳�ꂽ�T�C�Y���__HEAP_MGMTAREA_SIZE�����傫���Ȃ�B
	
	@param: SceSize size
	�m�ۂ���T�C�Y�B
	
	@return HeapUID
		�q�[�v���g�����߂̃n���h���B
	
-----------------------------------------------*/
HeapUID heapCreate( SceSize size );

/*-----------------------------------------------
	heapDestroy
	
	�m�ۂ����q�[�v���������B
	�q�[�v�̈�S�̂�������邽�߁AheapAlloc()�Ŋ��蓖�Ă��Ă��郁���������݂��Ă��A
	���������킹�Ă��ׂĉ�������B
	��������s������́A���̃q�[�v����m�ۂ����A�h���X�ɑ΂��ăA�N�Z�X���Ă͂����Ȃ��B
	
	@param: HeapUID uid
		heapCreate/heapCreateEx�̕Ԃ�l�B
-----------------------------------------------*/
void heapDestroy( HeapUID uid );

/*-----------------------------------------------
	heapAlloc
	
	�q�[�v����K�v�ȃT�C�Y�̗e�ʂ��擾����B
	�������A�q�[�v����m�ۂ���T�C�Y�́A�w�肵���T�C�Y�Ƀw�b�_�T�C�Y(HEAP_HEADER_SIZE)���������T�C�Y�ɂȂ�B
	
	@param: HeapUID uid
		heapCreate/heapExCreate�̕Ԃ�l�B
	
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
	
	heapAlloc()�Ŋm�ۂ����������u���b�N��������A�ė��p�\�ɂ���B
	������ꂽ�������u���b�N�͍Ăуq�[�v�Ƃ��Ďg�����Ƃ��o����B
	
	�q�[�v���̂��̂��K�v�����Ȃ����ꍇ��heapDestroy()���Ăׂ΂悢���A
	���̏ꍇ�A�m�ۂ��Ă��郁�����u���b�N��������悤�Ƃ���heapFree()���ĂԕK�v�͂Ȃ��B
	heapDestroy()�����s�����ƁA���̃q�[�v����m�ۂ��Ă��郁�����u���b�N�͑S�ĉ�������B
	
	
-----------------------------------------------*/
int heapFree( HeapUID uid, void *ptr );

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
