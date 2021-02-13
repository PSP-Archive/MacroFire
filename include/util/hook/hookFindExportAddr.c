/* hookFindExportAddr.c */

#include "hook_types.h"

void **hookFindExportAddr( const char *modname, const char *libname, unsigned int nid )
{
	SceModule *module;
	struct SceLibraryEntryTable *entry = NULL;
	unsigned int i = 0;
	
	/* ロード中のモジュールから必要なモジュールを検索 */
	module = sceKernelFindModuleByName( modname );
	if( ! module ) return NULL;
	
	/* 検索したモジュールから必要なライブラリを検索 */
	while( i < module->ent_size ){
		entry = (struct SceLibraryEntryTable *)( (unsigned int)(module->ent_top) + i );
		
		if( ( libname && ( strcmp( libname, entry->libname ) == 0 ) ) || ( ! libname && ! entry->libname ) ){
			break;
		}
		
		i += entry->len * sizeof( uint32_t );
	}
	
	if( ! entry->entrytable || ! entry->stubcount ) return NULL;
	
	/* ライブラリがエクスポートしているNIDの格納アドレスを返す */
	for( i = 0; i < entry->stubcount; i++ ){
		if( ((uintptr_t *)entry->entrytable)[i] == nid ) return (void **)&(((uintptr_t *)entry->entrytable)[entry->vstubcount + entry->stubcount + i]);
	}
	
	return NULL;
}
