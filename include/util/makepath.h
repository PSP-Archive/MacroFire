/*
	makepath.h
	
	フルパスを作成する。
	realpath()のようにカレントへのパスを自動取得しない点が異なる。
*/

#ifndef MAKEPATH_H
#define MAKEPATH_H

#include <stdbool.h>
#include <unistd.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
	フルパスを作成する。
	
	@param: const char *basepath
*/
bool makepath( const char* basepath, char *path, size_t len );

#ifdef __cplusplus
}
#endif

#endif
