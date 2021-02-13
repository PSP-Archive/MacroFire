/* truncate.c */

#include <pspkerneltypes.h>
#include <pspiofilemgr.h>
	
/*
	sceIoChstat()に渡すビットフラグ。
	truncateを実現する模様。
	ps2sdkに同じ物があったらしい?
	
	機能していない。
*/
#define FIO_CST_SIZE 0x0004

int truncate( const char *path, SceOff length )
{
	SceIoStat stat;
	
	if( length < 0 ) return -1;
	stat.st_size = length;
	
	return sceIoChstat( path, &stat, FIO_CST_SIZE );
}
