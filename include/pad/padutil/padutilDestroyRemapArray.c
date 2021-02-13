/* padutilDestroyRemapArray.c */

#include "padutil_types.h"

void padutilDestroyRemapArray( PadutilRemap *remap )
{
	memoryFree( remap );
}
