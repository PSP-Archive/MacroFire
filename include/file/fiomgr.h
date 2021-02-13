/*=========================================================
	
	fiomgr.h
	
	[�ύX����]
	09/07/16 ����
	
	�t�@�C��I/O�}�l�[�W���B
	
=========================================================*/
#ifndef FIOMGR_H
#define FIOMGR_H

#include <pspkernel.h>
#include <stdio.h>
#include <stdarg.h>

/*=========================================================
	�}�N��
=========================================================*/
#define FH_PATH_MAX     255

#define FH_O_RDONLY     PSP_O_RDONLY
#define FH_O_WRONLY     PSP_O_WRONLY
#define FH_O_RDWR       PSP_O_RDWR
#define FH_O_NONBLOCK   PSP_O_NBLOCK
#define FH_O_APPEND     PSP_O_APPEND
#define FH_O_CREAT      PSP_O_CREAT
#define FH_O_TRUNC      PSP_O_TRUNC
#define FH_O_EXCL       PSP_O_EXCL
#define FH_O_NOWAIT     PSP_O_NOWAIT
#define FH_O_LARGEFILE  0x80000000
#define FH_O_ALLOC_HIGH 0x40000000

#define FH_SEEK_SET     PSP_SEEK_SET
#define FH_SEEK_END     PSP_SEEK_END
#define FH_SEEK_CUR     PSP_SEEK_CUR

#ifdef __cplusplus
extern "C" {
#endif

/*=========================================================
	�^�錾
=========================================================*/
typedef intptr_t FiomgrHandle;

/*=========================================================
	�֐�
=========================================================*/
/*-----------------------------------------------
	fiomgrOpen
	
	�t�@�C�����J���ăt�@�C��IO�n���h���𓾂�B
	
	@param: const char *path
		�t�@�C���p�X�B
	
	@param: int flags
		�I�[�v���t���O�B
		PSPSDK�Œ�`����Ă���PSP_O_*�ł��A���̃w�b�_�t�@�C���Œ�`����Ă���FH_O_*�ł��ǂ���ł��悢�B
		32bit�ȏ�̃T�C�Y�̃t�@�C�����J���Ƃ��́AFH_O_LARGEFILE���w�肷��B
	
	@param: int perm
		�t�@�C���̃p�[�~�b�V�����B
	
	@return: FiomgrHandle
		! 0: �L���ȃt�@�C��I/O�n���h��
		= 0: �G���[ (�������s�����s���ȃt�@�C���p�X)
-----------------------------------------------*/
FiomgrHandle fiomgrOpen( const char *path, int flags, int perm );

/*-----------------------------------------------
	fiomgrClose
	
	�t�@�C�������B
	
	@param: FiomgrHandle fh
		�L���ȃt�@�C��I/O�n���h���B
	
	@return: int
		< 0: �G���[
-----------------------------------------------*/
int fiomgrClose( FiomgrHandle fh );

/*-----------------------------------------------
	fiomgrRead
	
	�t�@�C������w��o�C�g���ǂݏo���B
	�I�[�����͕t������Ȃ��B
	
	@param: FiomgrHandle fh
		�L���ȃt�@�C��I/O�n���h���B
	
	@param: void *data
		�ǂݏo�����f�[�^�̕ۑ���|�C���^�B
	
	@param: size_t length
		�ǂݏo���o�C�g���B
	
	@return: int
		�ǂݏo�����o�C�g���B
-----------------------------------------------*/
int fiomgrRead( FiomgrHandle fh, void *data, size_t length );

/*-----------------------------------------------
	fiomgrReadln
	
	�t�@�C������1�s�ǂݏo���B
	�W�����C�u������fgets()�̂悤�Ȃ��́B
	�������A�I�[�̉��s�����͍폜�����B
	1�s���ۑ���o�b�t�@�̃T�C�Y�𒴂��Ă���ꍇ�́A�r���܂ŃR�s�[����B
	������̏ꍇ���K���I�[����'\0'�͊܂܂��B
	
	�Ԃ�l�ɓǂݏo�����o�C�g����Ԃ����A�ꍇ�ɂ���Ă�maxlength�𒴂��邱�Ƃ�����B
	����́A���s�R�[�h��CRLF��2�o�C�g�ŕ\������Ă���ꍇ�ŁA
	���̃��C�u������CRLF��1�����Ƃ��ď������邽�߁A
	�ۑ���o�b�t�@�Ɋi�[�����o�C�g������1�o�C�g�����ǂݏo���\��������B
	�ۑ���o�b�t�@�𒴂��ĕ������R�s�[���Ă���킯�ł͂Ȃ��̂ŁA���ɋC�ɂ���K�v�͂Ȃ��B
	
	@param: FiomgrHandle fh
		�L���ȃt�@�C��I/O�n���h���B
	
	@param: char *str
		�ǂݏo����������̕ۑ���|�C���^�B
	
	@param: size_t maxlength
		�ۑ���o�b�t�@�̃T�C�Y�B
	
-----------------------------------------------*/
int fiomgrReadln( FiomgrHandle fh, char *str, size_t maxlength );

SceOff fiomgrTell( FiomgrHandle fh );
SceOff fiomgrSeek( FiomgrHandle fh, SceOff offset, int whence );
int fiomgrWrite( FiomgrHandle fh, void *data, size_t length );
int fiomgrWriteln( FiomgrHandle fh, void *data, size_t length );
int fiomgrWritef( FiomgrHandle fh, char *buf, size_t buflen, const char *format, ... );
int fiomgrWritefln( FiomgrHandle fh, char *buf, size_t buflen, const char *format, ... );

/*-----------------------------------------------
	fiomgrTruncate
	
	�Ώۂ̃t�@�C���̎w�肵���o�C�g���֐L���A�܂��͐؂�l�߂�B
	ftruncate()�Ɠ����悤�Ȍ��ʂ������炷�B
	
	�������Aftruncate()�ƌ���I�ɈႤ�̂́A
	��x�J���Ă���t�@�C������Ă���truncate()�������R�[�����Ă�����x�J������
	�Ƃ������������_�B
	
	PSP�ł�ftruncate()�����̋@�\���������Ă��Ȃ�����(���Ԃ�)�B
	
	@param: FiomgrHandle fh
		�L���ȃt�@�C��I/O�n���h���B
	
	@param: SceOff length
		�o�C�g���B
	
	@return: int
		= 0: ����
		< 0: �G���[
-----------------------------------------------*/
int fiomgrTruncate( FiomgrHandle fh, SceOff length );

#ifdef __cplusplus
}
#endif

#endif
