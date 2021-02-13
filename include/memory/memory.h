/*=========================================================

	memory.h

	malloc()/calloc()/realloc()/memalign()/free()もどき。
	
	EBOOT.PBPを作成する場合は素直にPSPSDKのmallocらを使おう。

=========================================================*/
#ifndef MEMORY_H
#define MEMORY_H

#include <pspkernel.h>

/*=========================================================
	マクロ
=========================================================*/
#define MEMORY_PAGE_SIZE 256

/* どこぞから拝借した、メモリアラインを調整するマクロ */
#define MEMORY_ALIGN_ADDR( align, addr ) ( ( (uintptr_t)( addr ) + ( align ) - 1 ) & ( ~( ( align ) - 1 ) ) )

#ifdef __cplusplus
extern "C" {
#endif

/*=========================================================
	型宣言
=========================================================*/
typedef enum {
	MEMORY_KERN_HI = 1,
	MEMORY_USER,
	MEMORY_KERN_HI_MIRROR,
	MEMORY_KERN_LO,
	MEMORY_VOLATILE,
	MEMORY_USER_MIRROR
} MemoryPartition;

/*=========================================================
	プロトタイプ
=========================================================*/
/*-----------------------------------------------
	memoryAlloc
	
	malloc()もどき。
	メモリを割り当てる。
-----------------------------------------------*/
#define memoryAlloc( size ) memoryExalloc( "user_space", MEMORY_USER, 0, size, PSP_SMEM_Low, NULL )

/*-----------------------------------------------
	memoryCalloc
	
	calloc()もどき。
	メモリを割り当て、領域をゼロクリアする。
-----------------------------------------------*/
void *memoryCalloc( SceSize nelem, SceSize size );

/*-----------------------------------------------
	memoryRealloc
	
	realloc()もどき。
	割り当てたメモリ領域を増減させる。
	
	リサイズに成功した場合、元の領域は自動的に解放される。
	失敗した場合は、返り値にNULLが返るものの、依然として元の領域が有効であるためメモリリークに注意。
	場合によってmemoryReallocf()を使うのもいいかもしれない。
	
	ポインタアドレスはほとんどの場合、元のアドレスとは違うアドレスを返す。
	例外として、メモリブロックサイズ(256バイト)以下の増減で、なおかつ、変更後のサイズが、
	メモリブロック境界をまたがない場合はアドレスは変更されない。
-----------------------------------------------*/
void *memoryRealloc( const char *name, void *src_memblock, SceSize new_size );

/*-----------------------------------------------
	memoryReallocf
	
	FreeBSDで実装されているreallocf()もどき。
	これは、要求されたサイズへリサイズ出来なかった場合も、
	元の領域を自動的に解放する点を除いて、memoryRealloc()と同じ。
-----------------------------------------------*/
void *memoryReallocf( const char *name, void *src_memblock, SceSize new_size );

/*-----------------------------------------------
	memoryAlign
	
	memalign()もどき。
	メモリを確保したあと、
	指定されたアライメントに合わせたポインタアドレスを返す。
-----------------------------------------------*/
#define memoryAlign( align, size ) memoryExalloc( "user_aligned_space", MEMORY_USER, align, size, PSP_SMEM_Low, NULL )

/*-----------------------------------------------
	memoryUsableSize
	
	実際に確保された領域のサイズを取得する。
	要求したメモリサイズより大きく、なおかつもっとも近いメモリページサイズの倍数になる。
-----------------------------------------------*/
SceSize memoryUsableSize( const void *memblock );

/*-----------------------------------------------
	memoryExalloc
	
	実際にメモリの割り当てを行う関数。
	memoryAlloc()/memoryCalloc()/memoryRealloc()/memoryReallocf()/memoryAlign()は
	最終的にこの関数を呼び出す。
	
	引数を元に、sceKernelAllocPartitionMemory()を使って実際にメモリを確保する。
	確保されるサイズは、指定されたサイズに加えて、
	メモリ管理情報を記録するために、8バイト多く確保される。
	ただし、確保されるサイズは必ずページ(256バイト)区切りとなる。
	
	partitionは、この関数を直接呼んでメモリを割り当てる場合にのみ変更可能で、
	memoryAlloc()などで確保する場合は、必ずMEMORY_USERが使用される。
	MEMORY_VOLATILEは、これで確保せずにscePowerVolatileMemLock()といったAPIを使うべき。
	
	alignでアライメントを指定することもできる。
	アライメントに0を指定した場合は、このライブラリは特に調整を行わない。
	
	typeは以下の3つが指定可能。
		PSP_SMEM_Low
		PSP_SMEM_High
		PSP_SMEM_Addr
	PSP_SMEM_Lowはメモリアドレスの小さい方からメモリを確保するよう試み、
	逆にPSP_SMEM_Highはメモリアドレスの大きい方からメモリを確保するように試みる。
	
	最後のPSP_SMEM_Addrは、よくわからないが第6引数のvoid *addrに指定するアドレスから確保するっぽい。
	おそらく、そのアドレスが既に使用されている場合はエラーを返す。
-----------------------------------------------*/
void *memoryExalloc( const char *name, MemoryPartition partition, unsigned int align, SceSize size, int type, void *addr );

/*-----------------------------------------------
	memoryFree
	
	free()もどき。
	確保されたメモリを解放する。
-----------------------------------------------*/
int memoryFree( void *memblock );

#ifdef __cplusplus
}
#endif

#endif
