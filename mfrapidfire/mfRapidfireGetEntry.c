/* mfRapidfireGetEntry.c */

#include "mfrapidfire_types.h"

bool mfRapidfireGetEntry( MfRapidfireUID uid, MfRapidfireData *data, enum PspCtrlButtons button )
{
	unsigned int i;
	
	/* ボタンコードから配列の要素番号を取得 */
	switch( button ){
		case PSP_CTRL_SELECT:   i = 0;  break;
		case PSP_CTRL_START:    i = 1;  break;
		case PSP_CTRL_UP:       i = 2;  break;
		case PSP_CTRL_RIGHT:    i = 3;  break;
		case PSP_CTRL_DOWN:     i = 4;  break;
		case PSP_CTRL_LEFT:     i = 5;  break;
		case PSP_CTRL_LTRIGGER: i = 6;  break;
		case PSP_CTRL_RTRIGGER: i = 7;  break;
		case PSP_CTRL_TRIANGLE: i = 8;  break;
		case PSP_CTRL_CIRCLE:   i = 9;  break;
		case PSP_CTRL_CROSS:    i = 10; break;
		case PSP_CTRL_SQUARE:   i = 11; break;
		default: return false;
	}
	
	*data = ((MfRapidfireParams *)uid)->pref[i];
	
	return true;
}
