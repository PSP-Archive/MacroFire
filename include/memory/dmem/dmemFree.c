/* dmemFree.c */

#include "dmem_types.h"

int dmemFree( DmemUID uid, void *ptr )
{
	struct dmem_root *root = (struct dmem_root *)uid;
	struct dmem_heap **heap;
	
	if( ! ptr || ! root->lastUse ) return CG_ERROR_OK;
	
	heap = (struct dmem_heap **)( (uintptr_t)ptr - sizeof( struct dmem_heap * ) );
	
	if( (*heap)->count == 1 ){
		/* �q�[�v�u���b�N�Ɏ����������Ȃ���΃q�[�v���Ɖ�� */
		struct dmem_heap *avail_heap;
		
		if( *heap == root->heapList ){
			/* �폜�Ώۃq�[�v���擪�̏ꍇ�̓��[�g�̃q�[�v�G���A���X�g�̊J�n�ʒu���C�� */
			avail_heap = (*heap)->next;
			root->heapList = avail_heap;
		} else{
			/* �폜�Ώۃq�[�v�̑O�̃q�[�v���擾 */
			for( avail_heap = root->heapList; avail_heap; avail_heap = avail_heap->next ){
				if( avail_heap->next == *heap ) break;
			}
			
			if( ! avail_heap ){
				/* �폜�Ώۃq�[�v�̃A�h���X�́A�n���ꂽDmemUID�̊Ǌ��O */
				return CG_ERROR_INVALID_ARGUMENT;
			}
			
			/* �ă����N */
			avail_heap->next = (*heap)->next;
		}
		
		/* �Ō�Ɏg�p�����q�[�v���폜�Ώۃq�[�v���w���Ă�����A�ǂ��ł������̂Ő����Ă���q�[�v���w���B */
		if( root->lastUse == *heap ) root->lastUse = avail_heap;
		
		heapDestroy( (*heap)->uid );
	} else{
		int ret;
		/* �q�[�v�Ɋ��蓖�čς݃J�E���g�����Z���A���������q�[�v�ɕԋp */
		(*heap)->count--;
		ret = heapFree( (*heap)->uid, (void *)heap );
		
		if( ret != CG_ERROR_OK ) return ret;
	}
	
	return CG_ERROR_OK;
}
