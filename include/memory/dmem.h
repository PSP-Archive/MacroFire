/*=========================================================
	
	dmem.h
	
	動的メモリ管理。
	ヒープを数珠上に繋いで、物理メモリが許す限りのメモリを動的に確保する。
	自動化している都合上オーバーヘッドが大きいので、
	サイズが分かっている場合は、heap.hやmemory.hで直接確保する方がよい。
	
	伸び縮みするリンクリストなどで有効。
	
	*メモリが断片化した際の対策がまだない*
	
=========================================================*/
#ifndef DMEM_H
#define DMEM_H

#ifdef HEAP_KERNEL
#include "memory/heap_kernel.h"
#else
#include "memory/heap.h"
#endif

/*=========================================================
	マクロ
=========================================================*/
#define DMEM_LOW  HEAP_LOW
#define DMEM_HIGH HEAP_HIGH

#ifdef __cplusplus
extern "C" {
#endif

/*=========================================================
	型宣言
=========================================================*/
typedef uintptr_t DmemUID;

/*=========================================================
	関数
=========================================================*/

/*-----------------------------------------------
	dmemNew
	
	動的メモリUIDを生成する。
	パーティションIDは、最初に作成するヒープエリアの位置。
	
	@param: size_t minblock
		ヒープ1ブロックの最小サイズ。
		ヒープが足りなくなると、このサイズの新しいヒープを作成しそれをつなぐ。
		
	@param: MemoryPartition partid
		ヒープをを作成するメモリパーティション。
	
	@param: int type
		割り当て方向。
		DMEM_HIGHかDMEM_LOWを指定。
	
	@return: DmemUID
		動的メモリUID。
		0で失敗。
-----------------------------------------------*/
DmemUID dmemNew( size_t minblock, MemoryPartition partid, int type );

/*-----------------------------------------------
	dmemAlloc
	
	動的メモリUIDからメモリ領域を確保する。
	
	@param: DmemUID
		dmemNew()で得たUID。
	
	@param: size_t size
		確保するサイズ。
	
	@return: void*
		メモリ領域へのポインタ。
	
-----------------------------------------------*/
void *dmemAlloc( DmemUID uid, size_t size );
void *dmemCalloc( DmemUID uid, size_t nelem, size_t size );
int dmemFree( DmemUID uid, void *ptr );
void dmemDestroy( DmemUID uid );

#ifdef __cplusplus
}
#endif

#endif
