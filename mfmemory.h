/*=========================================================

	mfmemory.h

	作業用メモリマネージメント。

=========================================================*/
#ifndef MFMEMORY_H
#define MFMEMORY_H

#include "memory/dmem.h"
#include "memory/heap_kernel.h"
#include "memory/memory.h"

#define mfMemoryAllocHi( size )        memoryExalloc( "MacroFireUsed", MEMORY_USER, 0, size, PSP_SMEM_High, NULL )
#define mfMemoryAllocLo( size )        memoryExalloc( "MacroFireUsed", MEMORY_USER, 0, size, PSP_SMEM_Low, NULL )
#define mfMemoryAlignHi( align, size ) memoryExalloc( "MacroFireUsed", MEMORY_USER, align, size, PSP_SMEM_High, NULL )
#define mfMemoryAlignLo( align, size ) memoryExalloc( "MacroFireUsed", MEMORY_USER, align, size, PSP_SMEM_Low, NULL )
#define mfMemoryRealloc( src, size )   memoryRealloc( "MacroFireUsed", src, size )
#define mfMemoryFree( addr )           memoryFree( addr )

#define mfMemoryHeapCreateHi( count, size )   heapExCreate( "MacroFireHeap", MEMORY_USER, size + HEAP_HEADER_SIZE * count, HEAP_HIGH )
#define mfMemoryHeapCreateLo( count, size )   heapExCreate( "MacroFireHeap", MEMORY_USER, size + HEAP_HEADER_SIZE * count, HEAP_LOW )
#define mfMemoryHeapAlloc                     heapAlloc
#define mfMemoryHeapCalloc                    heapCalloc
#define mfMemoryHeapFree                      heapFree
#define mfMemoryHeapDestroy                   heapDestroy

#endif
