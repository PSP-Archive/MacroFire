/*
	hook.
	
	APIをフック。
*/

#ifndef HOOK_H
#define HOOK_H

#include <pspsdk.h>

#ifdef __cplusplus
extern "C" {
#endif

/*-----------------------------------------------
	関数
-----------------------------------------------*/
/*
	hookFindExportAddr
	
	モジュール名、ライブラリ名、NIDから関数のエクスポートアドレスを返す。
	通常のモジュールが提供する関数のフックは、これで取得したアドレスをhookFunc()に渡せばよい……はず。
	
	@param: const char *modname
		モジュール名。
	
	@param: const char *libname
		ライブラリ名。
	
	@params: unsigned int nid
		NID。
	
	@return: void**
		エクスポートアドレス。
*/
void **hookFindExportAddr( const char *modname, const char *libname, unsigned int nid );

/*
	hookFindSyscallAddr
	
	フックする関数がシステムコールである場合は、
	hookFindExportAddr()で得たアドレスをさらにこの関数に渡す。
	
	@param: void **addr
		hookFindExportAddr()で得たアドレス。
	
	@return: void**
		システムコールアドレス。
*/
void **hookFindSyscallAddr( void **addr );

/*
	hookFindImportAddr
*/
void **hookFindImportAddr( const char *modname, const char *libname, unsigned int nid );

/*
	hookFunc
	
	フックを行う。
	
	@param: void **addr
		hookFindExportAddr()、あるいはhookFindSyscallAddr()で得たアドレス。
	
	@param: void *hookfunc
		置き換える関数のポインタ。
*/
void hookFunc( void **addr, void *hookfunc );

#ifdef __cplusplus
}
#endif

#endif
