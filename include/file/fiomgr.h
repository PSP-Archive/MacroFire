/*=========================================================
	
	fiomgr.h
	
	[変更履歴]
	09/07/16 初版
	
	ファイルI/Oマネージャ。
	
=========================================================*/
#ifndef FIOMGR_H
#define FIOMGR_H

#include <pspkernel.h>
#include <stdio.h>
#include <stdarg.h>

/*=========================================================
	マクロ
=========================================================*/
#define FH_PATH_MAX     255

#define FH_O_RDONLY     PSP_O_RDONLY
#define FH_O_WRONLY     PSP_O_WRONLY
#define FH_O_RDWR       PSP_O_RDWR
#define FH_O_NONBLOCK   PSP_O_NBLOCK
#define FH_O_APPEND     PSP_O_APPEND
#define FH_O_CREAT      PSP_O_CREAT
#define FH_O_TRUNC      PSP_O_TRUNC
#define FH_O_EXCL       PSP_O_EXCL
#define FH_O_NOWAIT     PSP_O_NOWAIT
#define FH_O_LARGEFILE  0x80000000
#define FH_O_ALLOC_HIGH 0x40000000

#define FH_SEEK_SET     PSP_SEEK_SET
#define FH_SEEK_END     PSP_SEEK_END
#define FH_SEEK_CUR     PSP_SEEK_CUR

#ifdef __cplusplus
extern "C" {
#endif

/*=========================================================
	型宣言
=========================================================*/
typedef intptr_t FiomgrHandle;

/*=========================================================
	関数
=========================================================*/
/*-----------------------------------------------
	fiomgrOpen
	
	ファイルを開いてファイルIOハンドルを得る。
	
	@param: const char *path
		ファイルパス。
	
	@param: int flags
		オープンフラグ。
		PSPSDKで定義されているPSP_O_*でも、このヘッダファイルで定義されているFH_O_*でもどちらでもよい。
		32bit以上のサイズのファイルを開くときは、FH_O_LARGEFILEを指定する。
	
	@param: int perm
		ファイルのパーミッション。
	
	@return: FiomgrHandle
		! 0: 有効なファイルI/Oハンドル
		= 0: エラー (メモリ不足か不正なファイルパス)
-----------------------------------------------*/
FiomgrHandle fiomgrOpen( const char *path, int flags, int perm );

/*-----------------------------------------------
	fiomgrClose
	
	ファイルを閉じる。
	
	@param: FiomgrHandle fh
		有効なファイルI/Oハンドル。
	
	@return: int
		< 0: エラー
-----------------------------------------------*/
int fiomgrClose( FiomgrHandle fh );

/*-----------------------------------------------
	fiomgrRead
	
	ファイルから指定バイト数読み出す。
	終端文字は付加されない。
	
	@param: FiomgrHandle fh
		有効なファイルI/Oハンドル。
	
	@param: void *data
		読み出したデータの保存先ポインタ。
	
	@param: size_t length
		読み出すバイト数。
	
	@return: int
		読み出したバイト数。
-----------------------------------------------*/
int fiomgrRead( FiomgrHandle fh, void *data, size_t length );

/*-----------------------------------------------
	fiomgrReadln
	
	ファイルから1行読み出す。
	標準ライブラリのfgets()のようなもの。
	ただし、終端の改行文字は削除される。
	1行が保存先バッファのサイズを超えている場合は、途中までコピーする。
	いずれの場合も必ず終端文字'\0'は含まれる。
	
	返り値に読み出したバイト数を返すが、場合によってはmaxlengthを超えることがある。
	それは、改行コードがCRLFの2バイトで表現されている場合で、
	このライブラリはCRLFも1文字として処理するため、
	保存先バッファに格納されるバイト数よりも1バイト多く読み出す可能性がある。
	保存先バッファを超えて文字をコピーしているわけではないので、特に気にする必要はない。
	
	@param: FiomgrHandle fh
		有効なファイルI/Oハンドル。
	
	@param: char *str
		読み出した文字列の保存先ポインタ。
	
	@param: size_t maxlength
		保存先バッファのサイズ。
	
-----------------------------------------------*/
int fiomgrReadln( FiomgrHandle fh, char *str, size_t maxlength );

SceOff fiomgrTell( FiomgrHandle fh );
SceOff fiomgrSeek( FiomgrHandle fh, SceOff offset, int whence );
int fiomgrWrite( FiomgrHandle fh, void *data, size_t length );
int fiomgrWriteln( FiomgrHandle fh, void *data, size_t length );
int fiomgrWritef( FiomgrHandle fh, char *buf, size_t buflen, const char *format, ... );
int fiomgrWritefln( FiomgrHandle fh, char *buf, size_t buflen, const char *format, ... );

/*-----------------------------------------------
	fiomgrTruncate
	
	対象のファイルの指定したバイト数へ伸長、または切り詰める。
	ftruncate()と同じような結果をもたらす。
	
	ただし、ftruncate()と決定的に違うのは、
	一度開いているファイルを閉じてからtruncate()相当をコールしてもう一度開き直す
	という動作をする点。
	
	PSPではftruncate()相当の機能が見つかっていないため(たぶん)。
	
	@param: FiomgrHandle fh
		有効なファイルI/Oハンドル。
	
	@param: SceOff length
		バイト数。
	
	@return: int
		= 0: 成功
		< 0: エラー
-----------------------------------------------*/
int fiomgrTruncate( FiomgrHandle fh, SceOff length );

#ifdef __cplusplus
}
#endif

#endif
