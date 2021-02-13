/* dirhGetCwd.c */

#include "dirh_types.h"

char *dirhGetCwd( DirhUID uid )
{
	return ((struct dirh_params *)uid)->data.cwd.path;
}
