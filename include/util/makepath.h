/*
	makepath.h
	
	�t���p�X���쐬����B
	realpath()�̂悤�ɃJ�����g�ւ̃p�X�������擾���Ȃ��_���قȂ�B
*/

#ifndef MAKEPATH_H
#define MAKEPATH_H

#include <stdbool.h>
#include <unistd.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
	�t���p�X���쐬����B
	
	@param: const char *basepath
*/
bool makepath( const char* basepath, char *path, size_t len );

#ifdef __cplusplus
}
#endif

#endif
