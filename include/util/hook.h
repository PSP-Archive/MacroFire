/*
	hook.
	
	API���t�b�N�B
*/

#ifndef HOOK_H
#define HOOK_H

#include <pspsdk.h>

#ifdef __cplusplus
extern "C" {
#endif

/*-----------------------------------------------
	�֐�
-----------------------------------------------*/
/*
	hookFindExportAddr
	
	���W���[�����A���C�u�������ANID����֐��̃G�N�X�|�[�g�A�h���X��Ԃ��B
	�ʏ�̃��W���[�����񋟂���֐��̃t�b�N�́A����Ŏ擾�����A�h���X��hookFunc()�ɓn���΂悢�c�c�͂��B
	
	@param: const char *modname
		���W���[�����B
	
	@param: const char *libname
		���C�u�������B
	
	@params: unsigned int nid
		NID�B
	
	@return: void**
		�G�N�X�|�[�g�A�h���X�B
*/
void **hookFindExportAddr( const char *modname, const char *libname, unsigned int nid );

/*
	hookFindSyscallAddr
	
	�t�b�N����֐����V�X�e���R�[���ł���ꍇ�́A
	hookFindExportAddr()�œ����A�h���X������ɂ��̊֐��ɓn���B
	
	@param: void **addr
		hookFindExportAddr()�œ����A�h���X�B
	
	@return: void**
		�V�X�e���R�[���A�h���X�B
*/
void **hookFindSyscallAddr( void **addr );

/*
	hookFindImportAddr
*/
void **hookFindImportAddr( const char *modname, const char *libname, unsigned int nid );

/*
	hookFunc
	
	�t�b�N���s���B
	
	@param: void **addr
		hookFindExportAddr()�A���邢��hookFindSyscallAddr()�œ����A�h���X�B
	
	@param: void *hookfunc
		�u��������֐��̃|�C���^�B
*/
void hookFunc( void **addr, void *hookfunc );

#ifdef __cplusplus
}
#endif

#endif
