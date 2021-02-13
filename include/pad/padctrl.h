/*=========================================================

	padctrl.h

	パッドデータの取得時に、キーリピートを処理する。

=========================================================*/
#ifndef PADCTRL_H
#define PADCTRL_H

#include <pspkernel.h>
#include <pspctrl.h>
#include <psprtc.h>
#include <stdbool.h>
#include <stdint.h>
#include "memory/memory.h"
#include "pad/padutil.h"
#include "cgerrno.h"

/*=========================================================
	マクロ
=========================================================*/
#define PADCTRL_REPEAT_START_DELAY 250000
#define PADCTRL_REPEAT_NEXT_DELAY  100000

#ifdef __cplusplus
extern "C" {
#endif

/*=========================================================
	型定義
=========================================================*/
typedef intptr_t PadctrlUID;

/*=========================================================
	関数
=========================================================*/
/*-----------------------------------------------
	padctrlNew
	
	Padctrlを初期化し、新しいPadctrlハンドルを得る。
-----------------------------------------------*/
PadctrlUID padctrlNew( void );

/*-----------------------------------------------
	padctrlDestroy
	
	Padctrlハンドルを破棄する。
-----------------------------------------------*/
void padctrlDestroy( PadctrlUID uid );

/*-----------------------------------------------
	padctrlSetRepeatButtons
	
	キーリピートを有効にするボタンを設定する。
-----------------------------------------------*/
void padctrlSetRepeatButtons( PadctrlUID uid, PadutilButtons buttons );

/*-----------------------------------------------
	padctrlReadBuffer
	
	キーバッファをsceCtrlReadBufferPositive()のAPIを使って読み出す。
	キーリピート間隔外の時はトグルボタン以外のボタンがすべて押されていない物として返される。
	
	キーリピートが有効になっていないボタンは、押しっぱなしにされている間は常に押されていない物として返される。
	
	返り値にキーリピート処理をしていない生のボタンコードか、エラーコードを返す。
	押しっぱなしを判定する必要がある場合は、こちらのボタンコードから判断すること。
-----------------------------------------------*/
unsigned int padctrlReadBuffer( PadctrlUID uid, SceCtrlData *pad, int count );

/*-----------------------------------------------
	padctrlPeekBuffer
	
	padctrlReadBuffer()と同じ事をするが、キーバッファ読み取りに使うAPIに、
	sceCtrlPeekBufferPositive()を使う点が異なる。
-----------------------------------------------*/
unsigned int padctrlPeekBuffer( PadctrlUID uid, SceCtrlData *pad, int count );

/*-----------------------------------------------
	padctrlResetRepeat
	
	キーリピート状態をリセットする。
-----------------------------------------------*/
void padctrlResetRepeat( PadctrlUID uid );

#ifdef __cplusplus
}
#endif

#endif
