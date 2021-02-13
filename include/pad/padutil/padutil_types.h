/* padutil_types.h                */
/* �����Ŏg�p�B�O����Q�Ƃ��Ȃ��B */

#include "../padutil.h"

#define PADUTIL_INVALID_RIGHT_TRIANGLE_DEGREE 45
#define PADUTIL_DEGREE_TO_RADIAN( d )         ( ( d ) *  ( 3.14 / 180 ) )
#define PADUTIL_IN_DEADZONE( x, y, r )        ( ( ( x ) * ( x ) ) + ( ( y ) * ( y ) ) <= ( ( r ) * ( r ) ) )
