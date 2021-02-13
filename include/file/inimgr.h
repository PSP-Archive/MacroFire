/*=========================================================
	
	inimgr.h
	
	INI�t�@�C���}�l�[�W���B
	
=========================================================*/
#ifndef INIMGR_H
#define INIMGR_H

#include <pspkernel.h>
#include <stdbool.h>

/*=========================================================
	�萔/�}�N��
=========================================================*/
#define INIMGR_ENTRY_LENGTH   255
#define INIMGR_SECTION_LENGTH 32

/*=========================================================
	�I�v�V����
=========================================================*/
#define INIMGR_CREATE ( FH_O_RDONLY )
#define INIMGR_SAVE ( FH_O_RDWR | FH_O_CREAT )

/*=========================================================
	�^�錾
=========================================================*/
typedef intptr_t InimgrUID;
typedef intptr_t InimgrContext;
typedef uint32_t InimgrVersion;

typedef enum {
	INIMGR_OP_LOAD,
	INIMGR_OP_SAVE
} InimgrOpMode;

typedef int ( *InimgrCallback )( InimgrContext, void* );

/*=========================================================
	�֐�
=========================================================*/
/*-----------------------------------------------
	inimgrNew
	
	�V����INI�}�l�[�W���𐶐�����B
	
	@return: InimgrUID
		���̊֐��Ŏg��UID�B
-----------------------------------------------*/
InimgrUID inimgrNew( void );

/*-----------------------------------------------
	inimgrDestroy
	
	INI�}�l�[�W�����폜����B
	
	@param: InimgrUID uid
		inimgrNew()�œ���UID�B
	
	@return: int
		= 0: ����
		< 0: �G���[
-----------------------------------------------*/
int inimgrDestroy( InimgrUID uid );

/*-----------------------------------------------
	inimgrMakeVersion
	
	inimgrLoad()��inimgrSave()�AinimgrMerge()�Ŏg���o�[�W�����ԍ��𐶐�����B
	����Ő�������InimgrVersion�́A�P���� == != < =< > >= �Ƃ��������Z�q�Ŕ�r�\�B
	
	
-----------------------------------------------*/
InimgrVersion inimgrMakeVersion( unsigned char major, unsigned char minor, unsigned char rev );

/*-----------------------------------------------
	inimgrRegisterCallback
	
	inimgrLoadSection()/inimgrSave()/inimgrMerge()�ŌĂяo�����R�[���o�b�N�֐��B
	�Z�N�V�������ɑ�ʂ̃f�[�^������ꍇ�ȂǂɎg�p���A�t�@�C�����璼�ڃf�[�^���Ă�Ń�������ߖ񂷂�B
	
	�R�[���o�b�N�Z�N�V�������ł����Ă� key = value �̌`���ȊO�œǂݏ������邱�Ƃ͂ł��Ȃ��B
	
	inimgrLoadSection()���ɁA�R�[���o�b�N���ݒ肳�ꂽ�Z�N�V������ǂݏo�����Ƃ���ƁA�R�[���o�b�N�����B
	inimgrSave()/inimgrMerge()�ł́A�R�[���o�b�N���ݒ肳�ꂽ�Z�N�V�����������o�����Ƃ����Ƃ��ɃR�[���o�b�N�����B
	
	�R�[���o�b�N���ꂽ�֐��́AinimgrReadEntry()/inimgrWriteEntry()���g���ăf�[�^��ǂݏo�����菑���o�����肷��B
	
	�ΏۂƂȂ�Z�N�V�������̎w��ɂ̓��C���h�J�[�h( ? * )���g�p�ł���B
	�}�b�`����Z�N�V�����ł́A�S�ăR�[���o�b�N�����B
	
	@param: InimgrUID uid
		inimgrNew()�œ���UID�B
	
	@param: const char *wildcard
		�R�[���o�b�N������Z�N�V�������B
		���C���h�J�[�h���g�p�\�B
	
	@param: InimgrCallback cb
		�R�[���o�b�N�֐��B
	
	@param: void *arg
		�R�[���o�b�N�֐��ɓn���������B
		�Z�b�g���ꂽ�l�����̂܂ܓn�����B
	
	@return: int
		= 0: ����
		< 0: �G���[
-----------------------------------------------*/
int inimgrRegisterCallback( InimgrUID uid, const char *wildcard, InimgrCallback cb, void *arg );

/*-----------------------------------------------
	inimgrLoad
	
	INI�t�@�C�������[�h���鏀�����s���B
	���̊֐���INI�t�@�C�������S�Ƀ��[�h�����킯�ł͂Ȃ��B
	����ɂ��e�Z�N�V�����̃t�@�C�����ł̃I�t�Z�b�g���擾����B
	
	���ۂɃf�[�^���g�p����ɂ́A�����inimgrLoadSection()�ŕK�v�ȃZ�N�V������ǂݏo���Ȃ���΂����Ȃ��B
	
	@param: InimgrUID uid
		inimgrNew()�œ���UID�B
	
	@param: const char *path
		INI�t�@�C���̃p�X�B
	
	@param: const char *sig
		�`�F�b�N����V�O�l�`���B
		��v���Ȃ���΁ACG_ERROR_INI_SIGNATURE_MISMATCH ��Ԃ��B
	
	@param: InimgrVersion version
		inimgrMakeVersion()�̕Ԃ�l�B
		�L���ȃo�[�W�������w�肷��B
		�w�肵���o�[�W���������AINI�t�@�C���̃o�[�W�������Â����
		CG_ERROR_INI_VERSION_NOT_SUPPORTED ��Ԃ��B
	
	@return int
		= 0: ����
		< 0: �G���[
-----------------------------------------------*/
int inimgrLoad( InimgrUID uid, const char *path, const char *sig, InimgrVersion version );

/*-----------------------------------------------
	inimgrLoadSection
	
	�w�肵���Z�N�V���������[�h���āA�ǂݏ����\�ȏ�Ԃɂ���B
	inimgrLoad()�́A�P�ɃZ�N�V�����̃I�t�Z�b�g���L�^���Ă��邾���Ȃ̂ŁA
	���̊֐��ŃG���g����ǂݏo���Ȃ���� inimgrGet* / inimgrSet* �͍s���Ȃ��B
-----------------------------------------------*/
int inimgrLoadSection( InimgrUID uid, const char *name );

/*-----------------------------------------------
	inimgrLoadAllSections
	
	inimgrLoadSection()�𑶍݂��邷�ׂẴZ�N�V�����ɑ΂��čs���B
-----------------------------------------------*/
int inimgrLoadAllSections( InimgrUID uid );

/*-----------------------------------------------
	inimgrAddSection
	
	�V�����Z�N�V�������쐬����B
	inimgrSet*�́A���݂��Ȃ��Z�N�V�����֒l���Z�b�g���邱�Ƃ͏o���Ȃ��B
	�����I�ɃZ�N�V�����͍쐬����Ȃ����߁A�����ō쐬����K�v������B
	
	���łɑ��݂��Ă���Z�N�V�������쐬���悤�Ƃ����ꍇ�͉�������������Ԃ��B
-----------------------------------------------*/
int inimgrAddSection( InimgrUID uid, const char *name );

/*-----------------------------------------------
	inimgrSave
	
	��������̃f�[�^��INI�t�@�C���Ƃ��ď����o���B
	�w�肵���p�X�ցA��������̃f�[�^��P�ɏ㏑������B
	�ǂݏo����Ȃ������Z�N�V�����͋�ɂȂ��Ă��܂��B
-----------------------------------------------*/
int inimgrSave( InimgrUID uid, const char *path, const char *sig, InimgrVersion version );

/*-----------------------------------------------
	inimgrMerge (������)
	
	��������̃f�[�^�ƁA������INI�t�@�C�����}�[�W����B
	��������̃f�[�^���D�悳���B
	
	�}�[�W����ɂ́A��x�ꎞ�t�@�C���ɏ����o���K�v�����邽�߁A�ꎞ�t�@�C���̃p�X�������ɕK�v�Ƃ���B
-----------------------------------------------*/
int inimgrMerge( InimgrUID uid, const char *path, const char *tempfile, const char *sig, InimgrVersion version );

/*-----------------------------------------------
	inimgrReadEntry
	
	�G���g������ǂݏo���B
-----------------------------------------------*/
bool inimgrReadEntry( InimgrContext context, char **key, char **value );

/*-----------------------------------------------
	inimgrWriteEntry
	
	�G���g����������o���B
-----------------------------------------------*/
bool inimgrWriteEntry( InimgrContext context, const char *key, const char *value );

/*-----------------------------------------------
	inimgrGetCurrentUID
	
	���݂�InimgrUID��Ԃ��B
-----------------------------------------------*/
InimgrUID inimgrGetCurrentUID( InimgrContext context );

/*-----------------------------------------------
	inimgrGetCurrentOpMode
	
	���݂̃I�v���[�h��Ԃ��B
-----------------------------------------------*/
InimgrOpMode inimgrGetCurrentOpMode( InimgrContext context );

int inimgrGetInt( InimgrUID uid, const char *name, const char *key, int *res );
int inimgrGetString( InimgrUID uid, const char *name, const char *key, char *res, size_t reslen );
int inimgrGetBool( InimgrUID uid, const char *name, const char *key, bool *res );
int inimgrSetInt( InimgrUID uid, const char *name, const char *key, int32_t integer );
int inimgrSetString( InimgrUID uid, const char *name, const char *key, const char *str );
int inimgrSetBool( InimgrUID uid, const char *name, const char *key, bool bl );


#endif
