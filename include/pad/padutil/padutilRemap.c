/* padutilRemap.c */

#include "padutil_types.h"

void padutilRemap( PadutilRemap *remap, PadutilButtons src, SceCtrlData *pad, u32 *hprmkey, bool redefine )
{
	PadutilButtons rmv_buttons = 0;
	PadutilButtons new_buttons = 0;
	unsigned char new_x      = pad->Lx;
	unsigned char new_y      = pad->Ly;
	unsigned int analog_dir  = padutilGetAnalogStickDirection( pad->Lx, pad->Ly, 0 );
	
	for( ; remap; remap = remap->next ){
		if( redefine ) rmv_buttons |= remap->remapButtons;
		if( ( ( src | padutilSetPad( analog_dir ) ) & remap->realButtons ) == remap->realButtons ){
			if( padutilGetPad( remap->realButtons ) & ( PADUTIL_CTRL_ANALOG_UP | PADUTIL_CTRL_ANALOG_RIGHT | PADUTIL_CTRL_ANALOG_DOWN | PADUTIL_CTRL_ANALOG_LEFT ) ){
				new_x = PADUTIL_CENTER_X;
				new_y = PADUTIL_CENTER_Y;
			}
			
			padutilSetAnalogStickDirection( padutilGetPad( remap->remapButtons ), &new_x, &new_y );
			
			rmv_buttons |= remap->realButtons;
			new_buttons |= remap->remapButtons;
		}
	}
	
	pad->Buttons &= ~( padutilGetPad( rmv_buttons ) );
	pad->Buttons |= padutilGetPad( new_buttons );
	pad->Lx      = new_x;
	pad->Ly      = new_y;
	
	*hprmkey &= ~( padutilGetHprm( rmv_buttons ) );
	*hprmkey |= padutilGetHprm( new_buttons );
}
