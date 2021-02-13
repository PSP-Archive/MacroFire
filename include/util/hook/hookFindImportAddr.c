#include "hook_types.h"

#include "pspkernel.h"

void **hookFindImportAddr( const char *modname, const char *libname, unsigned int nid )
{
	SceModule *module;
	struct SceLibraryStubTable *stub = NULL;
	unsigned int i = 0;
	
	/* ロード中のモジュールから必要なモジュールを検索 */
	module = sceKernelFindModuleByName( modname );
	if( ! module ) return NULL;
	
	/* 検索したモジュールから必要なライブラリを検索 */
	while( i < module->stub_size ){
		stub = (struct SceLibraryStubTable *)( (unsigned int)(module->stub_top) + i );
		
		if( ( libname && ( strcmp( libname, stub->libname ) == 0 ) ) || ( ! libname && ! stub->libname ) ){
			break;
		}
		
		i += stub->len * sizeof( uint32_t );
	}
	
	if( ! stub->nidtable || ! stub->stubcount ) return NULL;
	
	/* ライブラリがインポートしているNIDの格納アドレスを返す */
	for( i = 0; i < stub->stubcount; i++ ){
		if( stub->nidtable[i] == nid ) return (void **)&(((uintptr_t *)stub->stubtable)[i * 2]);
	}
	
	return NULL;
}
