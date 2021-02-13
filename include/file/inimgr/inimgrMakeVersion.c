/* inimgrMakeVersion.c */

#include "inimgr_types.h"

InimgrVersion inimgrMakeVersion( unsigned char major, unsigned char minor, unsigned char rev )
{
	return ( major << 16 | minor << 8 | rev );
}
