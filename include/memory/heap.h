/*=========================================================

	heap.h

	ヒープとして使用するメモリ領域を作成する。
	大きさの分かっているものを複数確保する場合に有利。
	
	とはいえ、まともな実装にしようとすると結構大変そうなので、とりあえずそれっぽく。
	今のところ速度面は無視。

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

#define HEAP_HEADER_SIZE sizeof( struct heap_allocated_memblock_header )

#ifdef __cplusplus
extern "C" {
#endif

/*=========================================================
	型宣言
=========================================================*/
typedef uintptr_t HeapUID;

struct heap_allocated_memblock_header {
	SceSize size;                                /* 使用しているメモリサイズ(ヘッダを含む) */
	struct heap_allocated_memblock_header *next; /* 次の使用中メモリブロックアドレス */
};

/*=========================================================
	関数
=========================================================*/

/*-----------------------------------------------
	heapExCreate
	
	ヒープとして使用するメモリ領域を確保する。
	ヘッダを必要とするため、確保されるサイズは指定したサイズよりヘッダサイズ(HEAP_HEADER_SIZE)だけ大きくなる。
	
	@param: const char *name
		メモリブロック名。
	
	@param: MemoryPartition partition
		メモリを確保するパーティション。
	
	@param: SceSize size
		ヒープとして使用するサイズ。
	
	@return: HeapUID
		ヒープを使うためのUID。
-----------------------------------------------*/
HeapUID heapExCreate( const char *name, MemoryPartition partition, SceSize size, int type );

/*-----------------------------------------------
	heapCreate
	
	ヒープとして使用するメモリ領域を確保する。
	この関数は内部でheapCreateEx()を呼ぶ。
	確保するメモリパーティションはMEMORY_USERで固定。
	
	@param: SceSize size
		ヒープとして使用するサイズ。
	
	@return: HeapUID
		ヒープを使うためのUID。
-----------------------------------------------*/
HeapUID heapCreate( SceSize size );

/*-----------------------------------------------
	heapDestroy
	
	確保したヒープを解放する。
	ヒープ領域全体を解放するため、heapAlloc()で割り当てられているメモリが存在しても、
	それらも合わせてすべて解放される。
	これを実行した後は、このヒープから貰ったアドレスに対してアクセスしてはいけない。
	
	@param: HeapUID uid
		heapCreate/heapCreateExの返り値。
-----------------------------------------------*/
void heapDestroy( HeapUID uid );

/*-----------------------------------------------
	heapAlloc
	
	ヒープから必要なサイズのストレージを取得する。
	ただし、ヒープから確保するサイズは、指定したサイズにヘッダサイズ(HEAP_HEADER_SIZE)を加えたサイズになる。
	
	@param: HeapUID uid
		heapCreate/heapCreateExの返り値。
	
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
	
	heapAlloc()/heapCalloc()で確保したメモリブロックを解放し、再利用可能にする。
	解放されたメモリブロックは再びヒープとして使うことが出来る。
	
	ヒープそのものが必要無くなった場合はheapDestroy()を呼べばよいし、
	その場合、確保しているメモリブロックを解放しようとしてheapFree()を呼ぶ必要はない。
	heapDestroy()が実行されると、そのヒープから確保しているメモリブロックは全て解放される。
-----------------------------------------------*/
int heapFree( HeapUID uid, void *heap );

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
