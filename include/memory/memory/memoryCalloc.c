/* memoryCalloc.c */

#include "memory_types.h"

void *memoryCalloc( SceSize nelem, SceSize size )
{
	void *memblock;
	
	/* どちらかが0の場合、1バイト確保 */
	if( nelem == 0 || size == 0 ){
		nelem = 1;
		size  = 1;
	}
	
	/*
		サイズチェック
		
		nelem * sizeがSceSize型の最大値を超えた(オーバーフローした)場合、
		この式は偽となりNULLを返す。
	*/
	if( ( nelem * size ) / size != nelem ) return NULL;
	
	memblock = memoryAlloc( nelem * size );
	if( memblock ) memset( memblock, 0, nelem * size );
	
	return memblock;
}
