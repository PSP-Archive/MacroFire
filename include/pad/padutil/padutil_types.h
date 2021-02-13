/* padutil_types.h                */
/* 内部で使用。外から参照しない。 */

#include "../padutil.h"

#define PADUTIL_INVALID_RIGHT_TRIANGLE_DEGREE 45
#define PADUTIL_DEGREE_TO_RADIAN( d )         ( ( d ) *  ( 3.14 / 180 ) )
#define PADUTIL_IN_DEADZONE( x, y, r )        ( ( ( x ) * ( x ) ) + ( ( y ) * ( y ) ) <= ( ( r ) * ( r ) ) )
