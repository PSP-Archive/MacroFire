/*
	文字列処理用関数
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
	関数プロトタイプ
-----------------------------------------------*/

/*
	strutilCopy
	
	標準ライブラリのstrncpy()に似ているが、必ず末尾にNULLを付加する。
	
	@param: char *dest
		文字列のコピー先。
		このポインタに元々書き込まれていた文字列は、srcで上書きされ失われる。
	
	@param: const char *src
		文字列のコピー元。
		この文字列がdestへコピーされる。
	
	@param: size_t max
		コピーする最大文字数。
		NULL文字を含む。
		1では常にNULLのみを、0ではなにもしない。
	
	@return: char*
		コピーした文字数を返す。
		ヌル文字も数える。
*/
int strutilCopy( char *__restrict__ dest, const char *__restrict__ src, size_t max );
int strutilNCopy( char *__restrict__ dest, const char *__restrict__ src, size_t n, size_t max );

/*
	strutilCat
	
	標準ライブラリのstrncat()に似ているが、必ず末尾にNULLを付加する。
	また、サイズはsrcの文字数ではなく、結合後の最大文字数。
	つまり、destは必ずmax以下になる。
	dest + srcの文字列がmaxよりも大きい場合は、srcは全てコピーされない。
	
	@param: char *dest
		元文字列。この後ろにsrcが結合される。
	
	@param: const char *src
		結合する文字列。destの後にこの文字列が結合される。
	
	@param: size_t max
		結合後の最大文字数。
		destは必ずこの長さ以下になる。
	
	@return: char*
		結合後の文字数を返す。
		ヌル文字も数える。
*/
int strutilCat( char *__restrict__ dest, const char *__restrict__ src, size_t max );
int strutilNCat( char *__restrict__ dest, const char *__restrict__ src, size_t n, size_t max );

/*
	strutilCounterPbrk
	
	標準ライブラリのstrpbrk()に似ているが、
	文字列srcに、文字列search中のいずれかに"含まれない"文字が最初に見つかった位置を返す。
	
	@param: const char *src
		検索対象文字列。
	
	@param: const char *search
		文字群。
	
	@return: char*
		"文字列src中で、文字群searchに含まれない文字が最初に見つかった位置。
*/
char *strutilCounterPbrk( const char *__restrict__ src, const char *__restrict__ search );

/*
	strutilCounterReversePbrk
	
	上記のstrutilCounterPbrk()と同じことを、文字列の末尾から検索する。
	
	@param: const char *src
		検索対象文字列。
	
	@param: const char *search
		文字群。
	
	@return: char*
		"文字列src中で、文字群searchに含まれない文字が最初に見つかった位置。
*/
char *strutilCounterReversePbrk( const char *__restrict__ src, const char *__restrict__ search );

/*
	strutilTrim
	
	文字列から、指定した文字を削除し、さらに詰める。
	
	@param: char *str
		処理対象の文字列。
	
	@param: const char *search
		取り除く文字を指定。
		文字列で指定するが、取り除きたい文字を全て書き出す。
	
	@return: int
		取り除いた文字数。
*/
int strutilTrim( char *__restrict__ str, const char *__restrict__ search );

/*
	strutilLTrim
	
	文字列から、指定した文字を削除し、さらに詰める。
	
	@param: char *str
		処理対象の文字列。
	
	@param: const char *search
		取り除く文字を指定。
		文字列で指定するが、取り除きたい文字を全て書き出す。
	
	@return: int
		取り除いた文字数。
*/
int strutilLTrim( char *__restrict__ str, const char *__restrict__ search );

/*
	strutilRTrim
	
	文字列から、指定した文字を削除し、さらに詰める。
	
	@param: char *str
		処理対象の文字列。
	
	@param: const char *search
		取り除く文字を指定。
		文字列で指定するが、取り除きたい文字を全て書き出す。
	
	@return: int
		取り除いた文字数。
*/
int strutilRTrim( char *__restrict__ str, const char *__restrict__ search );

/*
	strutilToUpperFirst
	
	文字列の先頭を大文字に変換する。
	
	@param: char *str
		変換対象文字列。
	
	@return: char*
		strを返す。
*/
char *strutilToUpperFirst( char *str );

/*
	strutilToLowerFirst
	
	文字列の先頭を小文字に変換する。
	
	@param: char *str
		変換対象文字列。
	
	@return: char*
		strを返す。
*/
char *strutilToLowerFirst( char *str );

/*
	strutilCountChar
	
	文字列中に含まれる文字の数を数える。
	
	@param: char *str
		検索対象文字列。
	
	@param: char c
		検索文字。
	
	@return: unsigned int
		発見した文字数。
*/
unsigned int strutilCountChar( const char *str, char c );

/*
	strutilInsert
	
	文字列の先頭に文字列を挿入する。
	
	@param: char *__restrict__ dest
		挿入先。
	
	@param: const char *__restrict__ src
		挿入する文字列。
	
	@param: size_t max
		挿入先のバッファサイズ。
	
	@return: int
		挿入後の文字数。
		返り値がmaxを越えている場合は、入りきらなかったことを示し、末尾が削られている。
*/
int strutilInsert( char *__restrict__ dest, const char *__restrict__ src, size_t max );
int strutilNInsert( char *__restrict__ dest, const char *__restrict__ src, size_t n, size_t max );

/*
	strutilWildcard
	
	ワイルドカードの判定を行う。
	
	@param: const char *__restrict__ wc
		ワイルドカード文字列。
	
	@param: const char *__restrict__ name
		マッチするか調べる文字列。
	
	@return: int
		0        = マッチしていない
		それ以外 = マッチ
*/
int strutilWildcard( const char *__restrict__ wc, const char *__restrict__ name );

#ifdef __cplusplus
}
#endif

#endif
