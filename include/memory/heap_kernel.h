/*=========================================================

	heap_kernel.h

	ヒープとして使用するメモリ領域を作成する。
	大きさの分かっているものを複数確保する場合に有利。
	
	カーネルモード専用。
	SCEのAPIを使用して確保。

=========================================================*/
#ifndef HEAP_H
#define HEAP_H

#include <pspkernel.h>
#include <pspsysmem_kernel.h>
#include "memory/memory.h"
#include "cgerrno.h"

/*=========================================================
	マクロ
=========================================================*/
#define HEAP_LOW  1
#define HEAP_HIGH 2
#define HEAP_HEADER_SIZE 8 // ヒープからメモリをAllocする場合に指定したサイズよりこのサイズ分大きく確保される。多分管理用領域。

#ifdef __cplusplus
extern "C" {
#endif

/*=========================================================
	型宣言
=========================================================*/
typedef SceUID HeapUID;

/*=========================================================
	関数
=========================================================*/
/*-----------------------------------------------
	heapExCreate
	
	ヒープとして使用するメモリ領域を確保。
	管理領域を必要とするため、確保するサイズは、指定されたサイズより__HEAP_MGMTAREA_SIZEだけ大きくなる。
	
	@param: const char *name
		ヒープエリアの名前。
	
	@param: MemoryPartition partition
		確保するメモリパーティション
	
	@param: SceSize size
		確保するサイズ。
	
	@param: int type
		確保する位置。
		HEAP_LOW : 空きメモリのうち、アドレスの小さい方から確保する。
		HEAP_HIGH: 空きメモリのうち、アドレスの大きい方から確保する。
	
	@return HeapUID
		ヒープを使うためのハンドル。
	
-----------------------------------------------*/
HeapUID heapExCreate( const char *name, MemoryPartition partition, SceSize size, int type );

/*-----------------------------------------------
	heapCreate
	
	ヒープとして使用するメモリ領域を確保。
	管理領域を必要とするため、確保するサイズは、指定されたサイズより__HEAP_MGMTAREA_SIZEだけ大きくなる。
	
	@param: SceSize size
	確保するサイズ。
	
	@return HeapUID
		ヒープを使うためのハンドル。
	
-----------------------------------------------*/
HeapUID heapCreate( SceSize size );

/*-----------------------------------------------
	heapDestroy
	
	確保したヒープを解放する。
	ヒープ領域全体を解放するため、heapAlloc()で割り当てられているメモリが存在しても、
	それらも合わせてすべて解放される。
	これを実行した後は、このヒープから確保したアドレスに対してアクセスしてはいけない。
	
	@param: HeapUID uid
		heapCreate/heapCreateExの返り値。
-----------------------------------------------*/
void heapDestroy( HeapUID uid );

/*-----------------------------------------------
	heapAlloc
	
	ヒープから必要なサイズの容量を取得する。
	ただし、ヒープから確保するサイズは、指定したサイズにヘッダサイズ(HEAP_HEADER_SIZE)を加えたサイズになる。
	
	@param: HeapUID uid
		heapCreate/heapExCreateの返り値。
	
	@param: SceSize size
		割り当てるサイズ。
	
	@return: void *
		確保したメモリブロックを指すポインタ。
-----------------------------------------------*/
void *heapAlloc( HeapUID uid, SceSize size );

/*-----------------------------------------------
	heapCalloc
	
	ヒープから必要なサイズのストレージを取得する。
	ただし、ヒープから確保するサイズは、指定したサイズにヘッダサイズ(HEAP_HEADER_SIZE)を加えたサイズになる。
	heapAlloc()と同じことをするが、確保後に対象メモリブロックをゼロクリアする点が異なる。
	
	@param: HeapUID uid
		heapCreate/heapCreateExの返り値。
	
	@param: SceSize size
		割り当てるサイズ。
	
	@return: void *
		確保したメモリブロックを指すポインタ。
-----------------------------------------------*/
void *heapCalloc( HeapUID uid, SceSize size );

/*-----------------------------------------------
	heapFree
	
	heapAlloc()で確保したメモリブロックを解放し、再利用可能にする。
	解放されたメモリブロックは再びヒープとして使うことが出来る。
	
	ヒープそのものが必要無くなった場合はheapDestroy()を呼べばよいし、
	その場合、確保しているメモリブロックを解放しようとしてheapFree()を呼ぶ必要はない。
	heapDestroy()が実行されると、そのヒープから確保しているメモリブロックは全て解放される。
	
	
-----------------------------------------------*/
int heapFree( HeapUID uid, void *ptr );

/*-----------------------------------------------
	heapTotalFreeSize
	
	heapCreate()/heapCreateEx()で確保したヒープ領域に、
	合計何バイトの空き容量が存在するかを返す。
	仮に1000という数値が返ってきても、それは1000バイトの連続した容量であるとは限らない。
	500と500の空き容量が残っているのかもしれないし、100の空き容量が10個残っているのかもしれない。
	
	
	@param: HeapUID uid
		heapCreate/heapCreateExの返り値。
	
	@return: SceSize
		空き容量の合計。
-----------------------------------------------*/
SceSize heapTotalFreeSize( HeapUID uid );

#ifdef __cplusplus
}
#endif

#endif
