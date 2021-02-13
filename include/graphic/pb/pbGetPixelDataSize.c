/* pbGetPixelDataSize.c */

#include "pb_types.h"

int pbGetPixelDataSize( int format )
{
	if( format == PB_PXF_8888 ){
		return 4;
	} else{
		return 2;
	}
}
