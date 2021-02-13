/* dmemAlloc.c */

#include "dmem_types.h"

static size_t dmem_need_heapsize( size_t minblock, size_t requestsize );
static struct dmem_heap *dmem_heap_new( size_t heapsize, MemoryPartition partid, int type );

void *dmemAlloc( DmemUID uid, size_t size )
{
	struct dmem_root *root = (struct dmem_root *)uid;
	struct dmem_heap **ptr = NULL;
	
	if( ! root->lastUse ){
		/* �܂��q�[�v�����������΍ŏ��̃q�[�v���쐬���� */
		root->heapList = dmem_heap_new( dmem_need_heapsize( root->minHeapSize, size ), root->memPart, root->allocType );
		if( ! root->heapList ) return NULL;
		root->lastUse = root->heapList;
		ptr = heapAlloc( root->lastUse->uid, size + sizeof( struct dmem_heap * ) );
	} else if( ! ( ptr = heapAlloc( root->lastUse->uid, size + sizeof( struct dmem_heap * ) ) ) ){
		struct dmem_heap *heap;
		
		/* �\���ȋ󂫗e�ʂ̂���q�[�v��T�� */
		for( heap = root->heapList; heap; heap = heap->next ){
			ptr = heapAlloc( heap->uid, size + sizeof( struct dmem_heap * ) );
			if( ptr ) break;
		}
		
		/* �󂫂�������ΐV�K�쐬 */
		if( ! ptr ){
			struct dmem_heap *last;
			
			heap = dmem_heap_new( dmem_need_heapsize( root->minHeapSize, size ), root->memPart, root->allocType );
			if( ! heap ) return NULL;
			
			for( last = root->lastUse; last->next; last = last->next );
			last->next = heap;
			
			ptr = heapAlloc( heap->uid, size + sizeof( struct dmem_heap * ) );
		}
		root->lastUse = heap;
	}
	
	/* �e�ƂȂ�q�[�v�̃|�C���^���Z�b�g */
	*ptr = root->lastUse;
	
	/* ���蓖�ăJ�E���g�𑝉� */
	root->lastUse->count++;
	
	/* �e�ƂȂ�q�[�v�̃|�C���^��������Ă��镔���̌����������̈�Ƃ��ĕԂ� */
	return (void *)( (uintptr_t)ptr + sizeof( struct dmem_heap * ) );
}

static size_t dmem_need_heapsize( size_t minblock, size_t requestsize )
{
	size_t first_data_size = requestsize + DMEM_HEADER_SIZE;
	
	if( first_data_size > minblock ){
		return first_data_size;
	} else{
		return minblock;
	}
}

static struct dmem_heap *dmem_heap_new( size_t heapsize, MemoryPartition partid, int type )
{
	struct dmem_heap *newheap;
	HeapUID huid = heapExCreate( "DmemHeap", partid, heapsize, type );
	if( ! CG_IS_VALID_UID( huid ) ) return NULL;
	
	newheap        = heapAlloc( huid, sizeof( struct dmem_heap ) );
	newheap->uid   = huid;
	newheap->count = 0;
	newheap->next  = NULL;
	
	return newheap;
}
