/* memory_types.h                 */
/* �����Ŏg�p�B�O����Q�Ƃ��Ȃ��B */

#include "../memory.h"
#include <string.h>
#include <math.h>

/*=========================================================
	���[�J���}�N��
=========================================================*/
#define MEMORY_POWER_OF_TWO( x ) ( ! ( ( x ) & ( ( x ) - 1 ) ) )
#define MEMORY_REAL_SIZE( size ) MEMORY_PAGE_SIZE * (unsigned int)ceil( (double)size / (double)MEMORY_PAGE_SIZE )

#define MEMORY_GET_PARTITION( part_type ) ( ( part_type ) >> 16 )
#define MEMORY_GET_BLOCKTYPE( part_type ) ( ( part_type ) & 0x0000FFFF )
#define MEMORY_SET_PARTITION_BLOCKTYPE( part, type ) ( ( ( part ) << 16 ) | ( type ) )

/*=========================================================
	���[�J���^�錾
=========================================================*/
struct memory_header {
	/* ������p */
	SceUID  blockId;
	
	/* �Ċ��蓖�ėp */
	SceSize size;
	unsigned int part_type;
	unsigned int align;
};

