/*=========================================================
	
	ovmsg.h
	
	�I�[�o�[���C���b�Z�[�W�B
	
	ovmsgInit()��A�K�v�Ȃ�F��ʒu�̐ݒ�����āA
	�V�����X���b�h�𐶐�����B
	
=========================================================*/
#ifndef OVMSG_H
#define OVMSG_H

#include <pspkernel.h>
#include <stdarg.h>
#include <stdbool.h>

/*=========================================================
	�}�N��
=========================================================*/
#define OVMSG_MESSAGE_MAX_LENGTH 128

#ifdef __cplusplus
extern "C" {
#endif

/*=========================================================
	�^�錾
=========================================================*/
typedef intptr_t OvmsgUID;

/*=========================================================
	�֐�
=========================================================*/

/*-----------------------------------------------
	ovmsgInit
	
	ovmsg�ɕK�v�ȃ��������m�ۂ��ď������B
	
	@return: OvmsgUID
		����ovmsg�֐��ŕK�v��UID�B
-----------------------------------------------*/
OvmsgUID ovmsgInit( void );

/*-----------------------------------------------
	ovmsgStart
	
	ovmsg�X���b�h���쐬���ĊJ�n����B
	
	@param: OvmsgUID uid
		ovmsgInit()�œ���UID�B
	
	@param: const char *name
		�X���b�h�̖��O�B
	
	@param: int pri
		�X���b�h�̗D��x�B
	
	@return: int
		= CG_ERROR_OK: ����
		< 0: �G���[
-----------------------------------------------*/
int ovmsgStart( OvmsgUID uid, const char *name, int pri );

/*-----------------------------------------------
	ovmsgGetThreadId
	
	ovmsg�X���b�h��ID���擾����B
	
	@param: OvmsgUID uid
		ovmsgInit()�œ���UID�B
	
	@return: SceUID
		�X���b�hID�B
		0�̏ꍇ�̓X���b�h�͋N�����Ă��Ȃ����A���ɏI�����Ă���B
-----------------------------------------------*/
SceUID ovmsgGetThreadId( OvmsgUID uid );

/*-----------------------------------------------
	ovmsgSetColor
	
	***���̂Ƃ���A���̊֐��̓X���b�h�Z�[�t�ł͂Ȃ����߁AovmsgStart()��ɌĂ�ł͂����Ȃ�***
	
	�\�����郁�b�Z�[�W�̕����F�Ƃ��̔w�i�F���w�肷��B
	
	@param: OvmsgUID uid
		ovmsgInit()�œ���UID�B
	
	@param: unsigned int fg
		�����F�B
	
	@param: unsigned int bg
		�w�i�F�B
-----------------------------------------------*/
void ovmsgSetColor( OvmsgUID uid, unsigned int fg, unsigned int bg );

/*-----------------------------------------------
	ovmsgSetPosition

	***���̂Ƃ���A���̊֐��̓X���b�h�Z�[�t�ł͂Ȃ����߁AovmsgStart()��ɌĂ�ł͂����Ȃ�***
	
	�\�����郁�b�Z�[�W�̈ʒu���w�肷��B
	
	@param: OvmsgUID uid
		ovmsgInit()�œ���UID�B
	
	@param: unsigned int x
		�\������X���W�B
	
	@param: unsigned int y
		�\������Y���W�B
-----------------------------------------------*/
void ovmsgSetPosition( OvmsgUID uid, unsigned int x, unsigned int y );

/*-----------------------------------------------
	ovmsgVprintf
	
	�W�����C�u������vprintf�̂悤�ɁAva_list�������ɂƂ��ĕ\������B
	
	@param: OvmsgUID uid
		ovmsgInit()�œ���UID�B
	
	@param: const char *format
		�t�H�[�}�b�g������B
	
	@param: va_list ap
	
	@return: int
		�\�������������B
-----------------------------------------------*/
int ovmsgVprintf( OvmsgUID uid, const char *format, va_list ap );

/*-----------------------------------------------
	ovmsgPrintf
	
	�W�����C�u������printf�̂悤�ɕ\������B
	
	@param: OvmsgUID uid
		ovmsgInit()�œ���UID�B
	
	@param: const char *format
		�t�H�[�}�b�g������B
	
	@param: ...
		�ϐ�
	
	@return: int
		�\�������������B
-----------------------------------------------*/
int ovmsgPrintf( OvmsgUID uid, const char *format, ... );

/*-----------------------------------------------
	ovmsgSuspend
	
	�����������\�����Ă����炻��𒆒f������B
	
	@param: OvmsgUID uid
		ovmsgInit()�œ���UID�B
-----------------------------------------------*/
void ovmsgSuspend( OvmsgUID uid );

/*-----------------------------------------------
	ovmsgShutdown
	
	ovmsg�X���b�h���I��������B
	
	@param: OvmsgUID uid
		ovmsgInit()�œ���UID�B
-----------------------------------------------*/
void ovmsgShutdown( OvmsgUID uid );

/*-----------------------------------------------
	ovmsgDestroy
	
	OvmsgUID���폜����B
	���s��͊Y��OvmsgUID���g���Ă͂����Ȃ��B
	
	@param: OvmsgUID uid
		ovmsgInit()�œ���UID�B
-----------------------------------------------*/
void ovmsgDestroy( OvmsgUID uid );

#ifdef __cplusplus
}
#endif

#endif
