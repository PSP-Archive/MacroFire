/* inimgrGetCurrentUID.c */

#include "inimgr_types.h"

InimgrUID inimgrGetCurrentUID( InimgrContext context )
{
	return (InimgrUID)(((struct inimgr_context *)context)->params);
}
