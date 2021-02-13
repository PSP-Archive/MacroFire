/* memory_types.h                 */
/* 内部で使用。外から参照しない。 */

#include "../memory.h"
#include <string.h>
#include <math.h>

/*=========================================================
	ローカルマクロ
=========================================================*/
#define MEMORY_POWER_OF_TWO( x ) ( ! ( ( x ) & ( ( x ) - 1 ) ) )
#define MEMORY_REAL_SIZE( size ) MEMORY_PAGE_SIZE * (unsigned int)ceil( (double)size / (double)MEMORY_PAGE_SIZE )

#define MEMORY_GET_PARTITION( part_type ) ( ( part_type ) >> 16 )
#define MEMORY_GET_BLOCKTYPE( part_type ) ( ( part_type ) & 0x0000FFFF )
#define MEMORY_SET_PARTITION_BLOCKTYPE( part, type ) ( ( ( part ) << 16 ) | ( type ) )

/*=========================================================
	ローカル型宣言
=========================================================*/
struct memory_header {
	/* 解放時用 */
	SceUID  blockId;
	
	/* 再割り当て用 */
	SceSize size;
	unsigned int part_type;
	unsigned int align;
};

