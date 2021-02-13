/* dmemFree.c */

#include "dmem_types.h"

int dmemFree( DmemUID uid, void *ptr )
{
	struct dmem_root *root = (struct dmem_root *)uid;
	struct dmem_heap **heap;
	
	if( ! ptr || ! root->lastUse ) return CG_ERROR_OK;
	
	heap = (struct dmem_heap **)( (uintptr_t)ptr - sizeof( struct dmem_heap * ) );
	
	if( (*heap)->count == 1 ){
		/* ヒープブロックに自分しかいなければヒープごと解放 */
		struct dmem_heap *avail_heap;
		
		if( *heap == root->heapList ){
			/* 削除対象ヒープが先頭の場合はルートのヒープエリアリストの開始位置を修正 */
			avail_heap = (*heap)->next;
			root->heapList = avail_heap;
		} else{
			/* 削除対象ヒープの前のヒープを取得 */
			for( avail_heap = root->heapList; avail_heap; avail_heap = avail_heap->next ){
				if( avail_heap->next == *heap ) break;
			}
			
			if( ! avail_heap ){
				/* 削除対象ヒープのアドレスは、渡されたDmemUIDの管轄外 */
				return CG_ERROR_INVALID_ARGUMENT;
			}
			
			/* 再リンク */
			avail_heap->next = (*heap)->next;
		}
		
		/* 最後に使用したヒープが削除対象ヒープを指していたら、どこでもいいので生きているヒープを指す。 */
		if( root->lastUse == *heap ) root->lastUse = avail_heap;
		
		heapDestroy( (*heap)->uid );
	} else{
		int ret;
		/* ヒープに割り当て済みカウントを減算し、メモリをヒープに返却 */
		(*heap)->count--;
		ret = heapFree( (*heap)->uid, (void *)heap );
		
		if( ret != CG_ERROR_OK ) return ret;
	}
	
	return CG_ERROR_OK;
}
