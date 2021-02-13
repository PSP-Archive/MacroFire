/*=========================================================
	
	ovmsg.h
	
	オーバーレイメッセージ。
	
	ovmsgInit()後、必要なら色や位置の設定をして、
	新しいスレッドを生成する。
	
=========================================================*/
#ifndef OVMSG_H
#define OVMSG_H

#include <pspkernel.h>
#include <stdarg.h>
#include <stdbool.h>

/*=========================================================
	マクロ
=========================================================*/
#define OVMSG_MESSAGE_MAX_LENGTH 128

#ifdef __cplusplus
extern "C" {
#endif

/*=========================================================
	型宣言
=========================================================*/
typedef intptr_t OvmsgUID;

/*=========================================================
	関数
=========================================================*/

/*-----------------------------------------------
	ovmsgInit
	
	ovmsgに必要なメモリを確保して初期化。
	
	@return: OvmsgUID
		他のovmsg関数で必要なUID。
-----------------------------------------------*/
OvmsgUID ovmsgInit( void );

/*-----------------------------------------------
	ovmsgStart
	
	ovmsgスレッドを作成して開始する。
	
	@param: OvmsgUID uid
		ovmsgInit()で得たUID。
	
	@param: const char *name
		スレッドの名前。
	
	@param: int pri
		スレッドの優先度。
	
	@return: int
		= CG_ERROR_OK: 成功
		< 0: エラー
-----------------------------------------------*/
int ovmsgStart( OvmsgUID uid, const char *name, int pri );

/*-----------------------------------------------
	ovmsgGetThreadId
	
	ovmsgスレッドのIDを取得する。
	
	@param: OvmsgUID uid
		ovmsgInit()で得たUID。
	
	@return: SceUID
		スレッドID。
		0の場合はスレッドは起動していないか、既に終了している。
-----------------------------------------------*/
SceUID ovmsgGetThreadId( OvmsgUID uid );

/*-----------------------------------------------
	ovmsgSetColor
	
	***今のところ、この関数はスレッドセーフではないため、ovmsgStart()後に呼んではいけない***
	
	表示するメッセージの文字色とその背景色を指定する。
	
	@param: OvmsgUID uid
		ovmsgInit()で得たUID。
	
	@param: unsigned int fg
		文字色。
	
	@param: unsigned int bg
		背景色。
-----------------------------------------------*/
void ovmsgSetColor( OvmsgUID uid, unsigned int fg, unsigned int bg );

/*-----------------------------------------------
	ovmsgSetPosition

	***今のところ、この関数はスレッドセーフではないため、ovmsgStart()後に呼んではいけない***
	
	表示するメッセージの位置を指定する。
	
	@param: OvmsgUID uid
		ovmsgInit()で得たUID。
	
	@param: unsigned int x
		表示するX座標。
	
	@param: unsigned int y
		表示するY座標。
-----------------------------------------------*/
void ovmsgSetPosition( OvmsgUID uid, unsigned int x, unsigned int y );

/*-----------------------------------------------
	ovmsgVprintf
	
	標準ライブラリのvprintfのように、va_listを引数にとって表示する。
	
	@param: OvmsgUID uid
		ovmsgInit()で得たUID。
	
	@param: const char *format
		フォーマット文字列。
	
	@param: va_list ap
	
	@return: int
		表示した文字数。
-----------------------------------------------*/
int ovmsgVprintf( OvmsgUID uid, const char *format, va_list ap );

/*-----------------------------------------------
	ovmsgPrintf
	
	標準ライブラリのprintfのように表示する。
	
	@param: OvmsgUID uid
		ovmsgInit()で得たUID。
	
	@param: const char *format
		フォーマット文字列。
	
	@param: ...
		変数
	
	@return: int
		表示した文字数。
-----------------------------------------------*/
int ovmsgPrintf( OvmsgUID uid, const char *format, ... );

/*-----------------------------------------------
	ovmsgSuspend
	
	もし文字列を表示していたらそれを中断させる。
	
	@param: OvmsgUID uid
		ovmsgInit()で得たUID。
-----------------------------------------------*/
void ovmsgSuspend( OvmsgUID uid );

/*-----------------------------------------------
	ovmsgShutdown
	
	ovmsgスレッドを終了させる。
	
	@param: OvmsgUID uid
		ovmsgInit()で得たUID。
-----------------------------------------------*/
void ovmsgShutdown( OvmsgUID uid );

/*-----------------------------------------------
	ovmsgDestroy
	
	OvmsgUIDを削除する。
	実行後は該当OvmsgUIDを使ってはいけない。
	
	@param: OvmsgUID uid
		ovmsgInit()で得たUID。
-----------------------------------------------*/
void ovmsgDestroy( OvmsgUID uid );

#ifdef __cplusplus
}
#endif

#endif
