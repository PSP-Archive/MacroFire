/*
	makepath.c
*/


#include "../makepath.h"
#include <ctype.h>
#include "util/strutil.h"
#include "memory/memory.h"

/*
	�p�X���� "drive:" �̒������擾����B
	���݂��Ȃ��ꍇ��0�B
*/
static int makepath_get_drive_offset( const char *path )
{
	const char *drive_end = strchr( path, ':' );
	
	if( drive_end ){
		return drive_end - path;
	} else{
		return 0;
	}
}

/*
	��΃p�X�𐳋K������B
	/../��/./�Adir/../dir2/�ƌ������璷�\�����̂Ă�B
	���̐�����A������̒����������邱�Ƃ͂Ȃ��̂ŁA�o�b�t�@�T�C�Y���l�����Ȃ��B
*/
static void makepath_normalize_abspath( char *path )
{
	int i, j;
	
	if( ! *path ) return;
	
	/* "//","///"�Ȃǂ̖��ʂȃX���b�V���̘A����r�� */
	for( i = 0, j = 0; path[i + 1]; i++ ){
		if( path[i] == '/' && path[i + 1] == '/' ) continue;
		if( j < i ) path[j] = path[i];
		j++;
	}
	path[j++] = path[i];
	path[j]   = '\0';
	
	/* "." �Ƃ����J�����g�f�B���N�g���w���r�� */
	for( i = 0, j = 0; path[i + 1]; i++ ){
		if(
			path[i] == '/' &&
			path[i + 1] == '.' &&
			( path[i + 2] == '/' || path[i + 2] == '\0' )
		){
			i += 1;
			continue;
		}
		if( j < i ) path[j] = path[i];
		j++;
	}
	path[j++] = path[i];
	path[j]   = '\0';
	
	/* �e�f�B���N�g���w���".."���������� */
	for( i = 0, j = 0; path[i]; i++ ){
		if(
			path[i] == '/' &&
			path[i + 1] == '.' &&
			path[i + 2] == '.' &&
			( path[i + 3] == '/' || path[i + 3] == '\0' )
		){
			i += 2;
			if( j ) for( j--; j && path[j] != '/'; j-- );
			continue;
		}
		if( j < i ) path[j] = path[i];
		j++;
	}
	path[j] = '\0';
	
	if( path[j - 1] == '/' ) path[j - 1] = '\0';
}

bool makepath( const char* basepath, char *path, size_t len )
{
	unsigned int drive_offset;
	
	if( ! path || ! len ) return false;
	
	drive_offset = makepath_get_drive_offset( path );
	if( ! drive_offset ){
		drive_offset = makepath_get_drive_offset( basepath );
		
		/* basepath���h���C�u�����܂܂Ȃ��ꍇ�́A�t���p�X�ł͂Ȃ��̂ŃG���[ */
		if( ! drive_offset ) return false;
		
		if( path[0] == '/' ){
			/* �p�X���X���b�V������n�܂��Ă���΁Abasepath�Ɠ����h���C�u�̃��[�g����̃p�X�ƌ��Ȃ� */
			strutilNInsert( path, basepath, drive_offset, len );
		} else{
			/* �����łȂ����basepath���R�s�[ */
			strutilInsert( path, "/", len );
			strutilInsert( path, basepath, len );
		}
	}
	
	makepath_normalize_abspath( path );
	
	return true;
}
