/* dirhTell.c */

#include "dirh_types.h"

int dirhTell( DirhUID uid )
{
	return ((struct dirh_params *)uid)->data.entry.pos;
}
