/* inimgrDestroy.c */

#include "inimgr_types.h"

int inimgrDestroy( InimgrUID uid )
{
	struct inimgr_params *params = (struct inimgr_params *)uid;
	
	if( params->ini > 0 ) fiomgrClose( params->ini );
	dmemDestroy( params->dmem );
	
	return CG_ERROR_OK;
}
