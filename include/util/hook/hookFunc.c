/* hookFunc.c */

#include "hook_types.h"

void hookFunc( void **addr, void *hookfunc )
{
	unsigned int intc;
	
	if( ! addr || ! hookfunc ) return;
	
	/* �Z�}�t�H�Ŕr������ */
	intc = pspSdkDisableInterrupts();
	
	/* �֐���ύX */
	*addr = hookfunc;
	
	/* �ύX������K�p */
	sceKernelDcacheWritebackInvalidateRange( addr, sizeof( addr ) );
	sceKernelIcacheInvalidateRange( addr, sizeof( addr ) );
	
	pspSdkEnableInterrupts( intc );
}
