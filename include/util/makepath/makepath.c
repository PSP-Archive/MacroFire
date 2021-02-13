/*
	makepath.c
*/


#include "../makepath.h"
#include <ctype.h>
#include "util/strutil.h"
#include "memory/memory.h"

/*
	パスから "drive:" の長さを取得する。
	存在しない場合は0。
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
	絶対パスを正規化する。
	/../や/./、dir/../dir2/と言った冗長表現を捨てる。
	その性質上、文字列の長さが増えることはないので、バッファサイズを考慮しない。
*/
static void makepath_normalize_abspath( char *path )
{
	int i, j;
	
	if( ! *path ) return;
	
	/* "//","///"などの無駄なスラッシュの連続を排除 */
	for( i = 0, j = 0; path[i + 1]; i++ ){
		if( path[i] == '/' && path[i + 1] == '/' ) continue;
		if( j < i ) path[j] = path[i];
		j++;
	}
	path[j++] = path[i];
	path[j]   = '\0';
	
	/* "." というカレントディレクトリ指定を排除 */
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
	
	/* 親ディレクトリ指定の".."を解決する */
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
		
		/* basepathがドライブ部を含まない場合は、フルパスではないのでエラー */
		if( ! drive_offset ) return false;
		
		if( path[0] == '/' ){
			/* パスがスラッシュから始まっていれば、basepathと同じドライブのルートからのパスと見なす */
			strutilNInsert( path, basepath, drive_offset, len );
		} else{
			/* そうでなければbasepathをコピー */
			strutilInsert( path, "/", len );
			strutilInsert( path, basepath, len );
		}
	}
	
	makepath_normalize_abspath( path );
	
	return true;
}
