/* truncate.c */

#include <pspkerneltypes.h>
#include <pspiofilemgr.h>
	
/*
	sceIoChstat()�ɓn���r�b�g�t���O�B
	truncate����������͗l�B
	ps2sdk�ɓ��������������炵��?
	
	�@�\���Ă��Ȃ��B
*/
#define FIO_CST_SIZE 0x0004

int truncate( const char *path, SceOff length )
{
	SceIoStat stat;
	
	if( length < 0 ) return -1;
	stat.st_size = length;
	
	return sceIoChstat( path, &stat, FIO_CST_SIZE );
}
