/*=========================================================
	
	cgerrno.h
	
	ClassG�G���[�R�[�h
	
=========================================================*/
#ifndef CGERRNO_H
#define CGERRNO_H

/*-----------------------------------------------
	����
-----------------------------------------------*/
#define CG_ERROR_OK 0
#define CG_OK       CG_ERROR_OK

/*-----------------------------------------------
	�G���[�}�N��
-----------------------------------------------*/
/* �������������ׂ� */
#define CG_IS_OK( err ) ( ( err ) >= CG_OK )

/* �G���[���ǂ������ׂ� */
#define CG_IS_ERROR( err ) ( ( err ) < CG_OK )

/* �G���[�R�[�h��SCE API����̃G���[���ǂ������ׂ� */
#define CG_IS_SCE_ERROR( err ) ( (( err ) & 0xF0000000 ) == 0x80000000 )

/* �G���[��CGERRNO�ł��邩���ׂ� */
#define CG_IS_CG_ERROR( err )  ( (( err ) & 0xF0000000 ) == 0xE0000000 )

/* �e�탉�C�u�������Ԃ�*UID�A�����SCE��API���Ԃ�SceUID�����L���Ȓl���ǂ������ׂ� */
#define CG_IS_VALID_UID( uid ) ( (int)( uid ) > CG_OK )

/*-----------------------------------------------
	�ėp�G���[
-----------------------------------------------*/
/* ���炩�̃G���[ */
#define CG_ERROR_UNKNOWN                 0xE001FFFF

/* �s���Ȉ��� */
#define CG_ERROR_INVALID_ARGUMENT        0xE0010001

/* ���������s�����Ă��� */
#define CG_ERROR_NOT_ENOUGH_MEMORY       0xE0010002

/* �o�b�t�@���s�����Ă��� */
#define CG_ERROR_OUT_OF_BUFFER           0xE0010003

/* ������Ă��Ȃ�������s���� */
#define CG_ERROR_OPERATION_NOT_PERMITTED 0xE0010004

/* ���݂��Ă��Ȃ� */
#define CG_ERROR_NOT_FOUND               0xE0010005

/* ���݂��Ă��� */
#define CG_ERROR_ALREADY_EXIST           0xE0010006

/* �ǂݍ��݋֎~ */
#define CG_ERROR_NOT_READABLE            0xE0010009

/* �������݋֎~ */
#define CG_ERROR_NOT_WRITABLE            0xE001000A

/* �f�B���N�g���ł��� */
#define CG_ERROR_IS_DIRECTORY            0xE001000B

/* �f�B���N�g���ł͂Ȃ� */
#define CG_ERROR_NOT_DIRECTORY           0xE001000C

/* ���Ɏ��s����Ă��� */
#define CG_ERROR_ALREADY_RUNNING         0xE001000D

/* �܂����s����Ă��Ȃ� */
#define CG_ERROR_NOT_RUNNING_YET         0xE001000E

/* ���ɏ���������Ă��� */
#define CG_ERROR_ALREADY_INITIALIZED     0xE001000F

/* �܂�����������Ă��Ȃ� */
#define CG_ERROR_NOT_INITIALIZED_YET     0xE0010010

/* ���ɊJ���Ă��� */
#define CG_ERROR_ALREADY_OPENED          0xE0010011

/* �܂��J���Ă��Ȃ� */
#define CG_ERROR_NOT_OPENED_YET          0xE0010012

/* �@�\�͖����� */
#define CG_ERROR_NOT_IMPLEMENTED_YET     0xE0010013


/*-----------------------------------------------
	SCE API �G���[
-----------------------------------------------*/
#define CG_ERROR_FAILED_TO_CREATE_SEMA 0xE0020001
#define CG_ERROR_FAILED_TO_GETSTAT     0xE0020002
#define CG_ERROR_FAILED_TO_OPEN        0xE0020004
#define CG_ERROR_FAILED_TO_READ        0xE0020005
#define CG_ERROR_FAILED_TO_WRITE       0xE0020006
#define CG_ERROR_FAILED_TO_SEEK        0xE0020007
#define CG_ERROR_FAILED_TO_DOPEN       0xE0020009
#define CG_ERROR_FAILED_TO_DREAD       0xE002000A

/*-----------------------------------------------
	Inimgr�G���[
-----------------------------------------------*/
/* �V�O�l�`������v���Ȃ� */
#define CG_ERROR_INI_SIGNATURE_MISMATCH    0xE0030001

/* �T�|�[�g���Ă��Ȃ��o�[�W���� */
#define CG_ERROR_INI_VERSION_NOT_SUPPORTED 0xE0030002
	
/* �w�肳�ꂽ�Z�N�V�����͑��݂��Ȃ� */
#define CG_ERROR_INI_SECTION_NOT_FOUND     0xE0030003

/* �w�肳�ꂽ�L�[�͑��݂��Ȃ� */
#define CG_ERROR_INI_KEY_NOT_FOUND         0xE0030004

/* �w�肳�ꂽ�L�[�̃f�[�^�́A�ǂݏo�����Ƃ����f�[�^�^�ɓK�����Ă��Ȃ� */
#define CG_ERROR_INI_INVALID_VALUE         0xE0030005


#endif
