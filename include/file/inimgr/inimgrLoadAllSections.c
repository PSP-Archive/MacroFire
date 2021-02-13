/* inimgrLoadAllSections.c */

#include "inimgr_types.h"

int inimgrLoadAllSections( InimgrUID uid )
{
	struct inimgr_params  *params = (struct inimgr_params *)uid;
	struct inimgr_section *section;
	for( section = params->section; section; section = section->next ) inimgrLoadSection( uid, section->name );
	
	return CG_ERROR_OK;
}
