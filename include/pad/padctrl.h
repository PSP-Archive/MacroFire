/*=========================================================

	padctrl.h

	�p�b�h�f�[�^�̎擾���ɁA�L�[���s�[�g����������B

=========================================================*/
#ifndef PADCTRL_H
#define PADCTRL_H

#include <pspkernel.h>
#include <pspctrl.h>
#include <psprtc.h>
#include <stdbool.h>
#include <stdint.h>
#include "memory/memory.h"
#include "pad/padutil.h"
#include "cgerrno.h"

/*=========================================================
	�}�N��
=========================================================*/
#define PADCTRL_REPEAT_START_DELAY 250000
#define PADCTRL_REPEAT_NEXT_DELAY  100000

#ifdef __cplusplus
extern "C" {
#endif

/*=========================================================
	�^��`
=========================================================*/
typedef intptr_t PadctrlUID;

/*=========================================================
	�֐�
=========================================================*/
/*-----------------------------------------------
	padctrlNew
	
	Padctrl�����������A�V����Padctrl�n���h���𓾂�B
-----------------------------------------------*/
PadctrlUID padctrlNew( void );

/*-----------------------------------------------
	padctrlDestroy
	
	Padctrl�n���h����j������B
-----------------------------------------------*/
void padctrlDestroy( PadctrlUID uid );

/*-----------------------------------------------
	padctrlSetRepeatButtons
	
	�L�[���s�[�g��L���ɂ���{�^����ݒ肷��B
-----------------------------------------------*/
void padctrlSetRepeatButtons( PadctrlUID uid, PadutilButtons buttons );

/*-----------------------------------------------
	padctrlReadBuffer
	
	�L�[�o�b�t�@��sceCtrlReadBufferPositive()��API���g���ēǂݏo���B
	�L�[���s�[�g�Ԋu�O�̎��̓g�O���{�^���ȊO�̃{�^�������ׂĉ�����Ă��Ȃ����Ƃ��ĕԂ����B
	
	�L�[���s�[�g���L���ɂȂ��Ă��Ȃ��{�^���́A�������ςȂ��ɂ���Ă���Ԃ͏�ɉ�����Ă��Ȃ����Ƃ��ĕԂ����B
	
	�Ԃ�l�ɃL�[���s�[�g���������Ă��Ȃ����̃{�^���R�[�h���A�G���[�R�[�h��Ԃ��B
	�������ςȂ��𔻒肷��K�v������ꍇ�́A������̃{�^���R�[�h���画�f���邱�ƁB
-----------------------------------------------*/
unsigned int padctrlReadBuffer( PadctrlUID uid, SceCtrlData *pad, int count );

/*-----------------------------------------------
	padctrlPeekBuffer
	
	padctrlReadBuffer()�Ɠ����������邪�A�L�[�o�b�t�@�ǂݎ��Ɏg��API�ɁA
	sceCtrlPeekBufferPositive()���g���_���قȂ�B
-----------------------------------------------*/
unsigned int padctrlPeekBuffer( PadctrlUID uid, SceCtrlData *pad, int count );

/*-----------------------------------------------
	padctrlResetRepeat
	
	�L�[���s�[�g��Ԃ����Z�b�g����B
-----------------------------------------------*/
void padctrlResetRepeat( PadctrlUID uid );

#ifdef __cplusplus
}
#endif

#endif
