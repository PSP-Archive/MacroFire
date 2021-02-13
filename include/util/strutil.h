/*
	�����񏈗��p�֐�
*/

#ifndef STRUTIL_H
#define STRUTIL_H

#include <ctype.h>
#include <string.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/*-----------------------------------------------
	�֐��v���g�^�C�v
-----------------------------------------------*/

/*
	strutilCopy
	
	�W�����C�u������strncpy()�Ɏ��Ă��邪�A�K��������NULL��t������B
	
	@param: char *dest
		������̃R�s�[��B
		���̃|�C���^�Ɍ��X�������܂�Ă���������́Asrc�ŏ㏑�����ꎸ����B
	
	@param: const char *src
		������̃R�s�[���B
		���̕�����dest�փR�s�[�����B
	
	@param: size_t max
		�R�s�[����ő啶�����B
		NULL�������܂ށB
		1�ł͏��NULL�݂̂��A0�ł͂Ȃɂ����Ȃ��B
	
	@return: char*
		�R�s�[������������Ԃ��B
		�k��������������B
*/
int strutilCopy( char *__restrict__ dest, const char *__restrict__ src, size_t max );
int strutilNCopy( char *__restrict__ dest, const char *__restrict__ src, size_t n, size_t max );

/*
	strutilCat
	
	�W�����C�u������strncat()�Ɏ��Ă��邪�A�K��������NULL��t������B
	�܂��A�T�C�Y��src�̕������ł͂Ȃ��A������̍ő啶�����B
	�܂�Adest�͕K��max�ȉ��ɂȂ�B
	dest + src�̕�����max�����傫���ꍇ�́Asrc�͑S�ăR�s�[����Ȃ��B
	
	@param: char *dest
		��������B���̌���src�����������B
	
	@param: const char *src
		�������镶����Bdest�̌�ɂ��̕����񂪌��������B
	
	@param: size_t max
		������̍ő啶�����B
		dest�͕K�����̒����ȉ��ɂȂ�B
	
	@return: char*
		������̕�������Ԃ��B
		�k��������������B
*/
int strutilCat( char *__restrict__ dest, const char *__restrict__ src, size_t max );
int strutilNCat( char *__restrict__ dest, const char *__restrict__ src, size_t n, size_t max );

/*
	strutilCounterPbrk
	
	�W�����C�u������strpbrk()�Ɏ��Ă��邪�A
	������src�ɁA������search���̂����ꂩ��"�܂܂�Ȃ�"�������ŏ��Ɍ��������ʒu��Ԃ��B
	
	@param: const char *src
		�����Ώە�����B
	
	@param: const char *search
		�����Q�B
	
	@return: char*
		"������src���ŁA�����Qsearch�Ɋ܂܂�Ȃ��������ŏ��Ɍ��������ʒu�B
*/
char *strutilCounterPbrk( const char *__restrict__ src, const char *__restrict__ search );

/*
	strutilCounterReversePbrk
	
	��L��strutilCounterPbrk()�Ɠ������Ƃ��A������̖������猟������B
	
	@param: const char *src
		�����Ώە�����B
	
	@param: const char *search
		�����Q�B
	
	@return: char*
		"������src���ŁA�����Qsearch�Ɋ܂܂�Ȃ��������ŏ��Ɍ��������ʒu�B
*/
char *strutilCounterReversePbrk( const char *__restrict__ src, const char *__restrict__ search );

/*
	strutilTrim
	
	�����񂩂�A�w�肵���������폜���A����ɋl�߂�B
	
	@param: char *str
		�����Ώۂ̕�����B
	
	@param: const char *search
		��菜���������w��B
		������Ŏw�肷�邪�A��菜������������S�ď����o���B
	
	@return: int
		��菜�����������B
*/
int strutilTrim( char *__restrict__ str, const char *__restrict__ search );

/*
	strutilLTrim
	
	�����񂩂�A�w�肵���������폜���A����ɋl�߂�B
	
	@param: char *str
		�����Ώۂ̕�����B
	
	@param: const char *search
		��菜���������w��B
		������Ŏw�肷�邪�A��菜������������S�ď����o���B
	
	@return: int
		��菜�����������B
*/
int strutilLTrim( char *__restrict__ str, const char *__restrict__ search );

/*
	strutilRTrim
	
	�����񂩂�A�w�肵���������폜���A����ɋl�߂�B
	
	@param: char *str
		�����Ώۂ̕�����B
	
	@param: const char *search
		��菜���������w��B
		������Ŏw�肷�邪�A��菜������������S�ď����o���B
	
	@return: int
		��菜�����������B
*/
int strutilRTrim( char *__restrict__ str, const char *__restrict__ search );

/*
	strutilToUpperFirst
	
	������̐擪��啶���ɕϊ�����B
	
	@param: char *str
		�ϊ��Ώە�����B
	
	@return: char*
		str��Ԃ��B
*/
char *strutilToUpperFirst( char *str );

/*
	strutilToLowerFirst
	
	������̐擪���������ɕϊ�����B
	
	@param: char *str
		�ϊ��Ώە�����B
	
	@return: char*
		str��Ԃ��B
*/
char *strutilToLowerFirst( char *str );

/*
	strutilCountChar
	
	�����񒆂Ɋ܂܂�镶���̐��𐔂���B
	
	@param: char *str
		�����Ώە�����B
	
	@param: char c
		���������B
	
	@return: unsigned int
		���������������B
*/
unsigned int strutilCountChar( const char *str, char c );

/*
	strutilInsert
	
	������̐擪�ɕ������}������B
	
	@param: char *__restrict__ dest
		�}����B
	
	@param: const char *__restrict__ src
		�}�����镶����B
	
	@param: size_t max
		�}����̃o�b�t�@�T�C�Y�B
	
	@return: int
		�}����̕������B
		�Ԃ�l��max���z���Ă���ꍇ�́A���肫��Ȃ��������Ƃ������A����������Ă���B
*/
int strutilInsert( char *__restrict__ dest, const char *__restrict__ src, size_t max );
int strutilNInsert( char *__restrict__ dest, const char *__restrict__ src, size_t n, size_t max );

/*
	strutilWildcard
	
	���C���h�J�[�h�̔�����s���B
	
	@param: const char *__restrict__ wc
		���C���h�J�[�h������B
	
	@param: const char *__restrict__ name
		�}�b�`���邩���ׂ镶����B
	
	@return: int
		0        = �}�b�`���Ă��Ȃ�
		����ȊO = �}�b�`
*/
int strutilWildcard( const char *__restrict__ wc, const char *__restrict__ name );

#ifdef __cplusplus
}
#endif

#endif
