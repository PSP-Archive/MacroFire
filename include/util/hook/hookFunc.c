/* hookFunc.c */

#include "hook_types.h"

void hookFunc( void **addr, void *hookfunc )
{
	unsigned int intc;
	
	if( ! addr || ! hookfunc ) return;
	
	/* セマフォで排他処理 */
	intc = pspSdkDisableInterrupts();
	
	/* 関数を変更 */
	*addr = hookfunc;
	
	/* 変更部分を適用 */
	sceKernelDcacheWritebackInvalidateRange( addr, sizeof( addr ) );
	sceKernelIcacheInvalidateRange( addr, sizeof( addr ) );
	
	pspSdkEnableInterrupts( intc );
}
