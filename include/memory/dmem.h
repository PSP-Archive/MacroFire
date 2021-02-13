/*=========================================================
	
	dmem.h
	
	���I�������Ǘ��B
	�q�[�v�𐔎��Ɍq���ŁA��������������������̃������𓮓I�Ɋm�ۂ���B
	���������Ă���s����I�[�o�[�w�b�h���傫���̂ŁA
	�T�C�Y���������Ă���ꍇ�́Aheap.h��memory.h�Œ��ڊm�ۂ�������悢�B
	
	�L�яk�݂��郊���N���X�g�ȂǂŗL���B
	
	*���������f�Љ������ۂ̑΍􂪂܂��Ȃ�*
	
=========================================================*/
#ifndef DMEM_H
#define DMEM_H

#ifdef HEAP_KERNEL
#include "memory/heap_kernel.h"
#else
#include "memory/heap.h"
#endif

/*=========================================================
	�}�N��
=========================================================*/
#define DMEM_LOW  HEAP_LOW
#define DMEM_HIGH HEAP_HIGH

#ifdef __cplusplus
extern "C" {
#endif

/*=========================================================
	�^�錾
=========================================================*/
typedef uintptr_t DmemUID;

/*=========================================================
	�֐�
=========================================================*/

/*-----------------------------------------------
	dmemNew
	
	���I������UID�𐶐�����B
	�p�[�e�B�V����ID�́A�ŏ��ɍ쐬����q�[�v�G���A�̈ʒu�B
	
	@param: size_t minblock
		�q�[�v1�u���b�N�̍ŏ��T�C�Y�B
		�q�[�v������Ȃ��Ȃ�ƁA���̃T�C�Y�̐V�����q�[�v���쐬��������Ȃ��B
		
	@param: MemoryPartition partid
		�q�[�v�����쐬���郁�����p�[�e�B�V�����B
	
	@param: int type
		���蓖�ĕ����B
		DMEM_HIGH��DMEM_LOW���w��B
	
	@return: DmemUID
		���I������UID�B
		0�Ŏ��s�B
-----------------------------------------------*/
DmemUID dmemNew( size_t minblock, MemoryPartition partid, int type );

/*-----------------------------------------------
	dmemAlloc
	
	���I������UID���烁�����̈���m�ۂ���B
	
	@param: DmemUID
		dmemNew()�œ���UID�B
	
	@param: size_t size
		�m�ۂ���T�C�Y�B
	
	@return: void*
		�������̈�ւ̃|�C���^�B
	
-----------------------------------------------*/
void *dmemAlloc( DmemUID uid, size_t size );
void *dmemCalloc( DmemUID uid, size_t nelem, size_t size );
int dmemFree( DmemUID uid, void *ptr );
void dmemDestroy( DmemUID uid );

#ifdef __cplusplus
}
#endif

#endif
