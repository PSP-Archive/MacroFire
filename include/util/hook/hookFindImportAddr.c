#include "hook_types.h"

#include "pspkernel.h"

void **hookFindImportAddr( const char *modname, const char *libname, unsigned int nid )
{
	SceModule *module;
	struct SceLibraryStubTable *stub = NULL;
	unsigned int i = 0;
	
	/* ���[�h���̃��W���[������K�v�ȃ��W���[�������� */
	module = sceKernelFindModuleByName( modname );
	if( ! module ) return NULL;
	
	/* �����������W���[������K�v�ȃ��C�u���������� */
	while( i < module->stub_size ){
		stub = (struct SceLibraryStubTable *)( (unsigned int)(module->stub_top) + i );
		
		if( ( libname && ( strcmp( libname, stub->libname ) == 0 ) ) || ( ! libname && ! stub->libname ) ){
			break;
		}
		
		i += stub->len * sizeof( uint32_t );
	}
	
	if( ! stub->nidtable || ! stub->stubcount ) return NULL;
	
	/* ���C�u�������C���|�[�g���Ă���NID�̊i�[�A�h���X��Ԃ� */
	for( i = 0; i < stub->stubcount; i++ ){
		if( stub->nidtable[i] == nid ) return (void **)&(((uintptr_t *)stub->stubtable)[i * 2]);
	}
	
	return NULL;
}
