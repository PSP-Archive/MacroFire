/*=========================================================

	padutil.c

	パッドデータ処理。

=========================================================*/
#ifndef PADUTIL_H
#define PADUTIL_H

#include <pspkernel.h>
#include <pspctrl.h>
#include <psphprm.h>
#include <stdlib.h>
#include <math.h>
#include "memory/memory.h"
#include "util/strutil.h"

/*=========================================================
	マクロ
=========================================================*/
#define PADUTIL_CENTER_X   127
#define PADUTIL_CENTER_Y   127
#define PADUTIL_MAX_COORD  255
#define PADUTIL_MAX_RADIUS 182 /* 辺の長さ * 2の平方根 (小数点以下切り上げ) */

#define PADUTIL_ARRAY_SELECT         0
#define PADUTIL_ARRAY_START          1
#define PADUTIL_ARRAY_UP             2
#define PADUTIL_ARRAY_RIGHT          3
#define PADUTIL_ARRAY_DOWN           4
#define PADUTIL_ARRAY_LEFT           5
#define PADUTIL_ARRAY_ANALOG_UP      6
#define PADUTIL_ARRAY_ANALOG_RIGHT   7
#define PADUTIL_ARRAY_ANALOG_DOWN    8
#define PADUTIL_ARRAY_ANALOG_LEFT    9
#define PADUTIL_ARRAY_LTRIGGER       10
#define PADUTIL_ARRAY_RTRIGGER       11
#define PADUTIL_ARRAY_TRIANGLE       12
#define PADUTIL_ARRAY_CIRCLE         13
#define PADUTIL_ARRAY_CROSS          14
#define PADUTIL_ARRAY_SQUARE         15
#define PADUTIL_ARRAY_HOME           16
#define PADUTIL_ARRAY_HOLD           17
#define PADUTIL_ARRAY_NOTE           18
#define PADUTIL_ARRAY_SCREEN         19
#define PADUTIL_ARRAY_VOLUP          20
#define PADUTIL_ARRAY_VOLDOWN        21
#define PADUTIL_ARRAY_WLAN_UP        22
#define PADUTIL_ARRAY_REMOTE         23
#define PADUTIL_ARRAY_DISC           24
#define PADUTIL_ARRAY_MS             25
#define PADUTIL_ARRAY_HPRM_PLAYPAUSE 26
#define PADUTIL_ARRAY_HPRM_FORWARD   27
#define PADUTIL_ARRAY_HPRM_BACK      28
#define PADUTIL_ARRAY_HPRM_VOLUP     29
#define PADUTIL_ARRAY_HPRM_VOLDOWN   30
#define PADUTIL_ARRAY_HPRM_HOLD      31
#define PADUTIL_ARRAY_END            32

#define PADUTIL_PAD_USERMODE_BUTTONS (\
	PSP_CTRL_START    |\
	PSP_CTRL_SELECT   |\
	PSP_CTRL_UP       |\
	PSP_CTRL_RIGHT    |\
	PSP_CTRL_DOWN     |\
	PSP_CTRL_LEFT     |\
	PSP_CTRL_LTRIGGER |\
	PSP_CTRL_RTRIGGER |\
	PSP_CTRL_TRIANGLE |\
	PSP_CTRL_CIRCLE   |\
	PSP_CTRL_CROSS    |\
	PSP_CTRL_SQUARE   \
)

#define PADUTIL_PAD_NORMAL_BUTTONS (\
	PADUTIL_PAD_USERMODE_BUTTONS |\
	PSP_CTRL_HOME                |\
	PSP_CTRL_NOTE                |\
	PSP_CTRL_SCREEN              |\
	PSP_CTRL_VOLUP               |\
	PSP_CTRL_VOLDOWN             \
)

#define PADUTIL_PAD_TOGGLE_BUTTONS (\
	PSP_CTRL_HOLD    |\
	PSP_CTRL_WLAN_UP |\
	PSP_CTRL_REMOTE  |\
	PSP_CTRL_DISC    |\
	PSP_CTRL_MS      \
)

#define PADUTIL_PAD_ANALOGDIR_BUTTONS (\
	PADUTIL_CTRL_ANALOG_UP    |\
	PADUTIL_CTRL_ANALOG_RIGHT |\
	PADUTIL_CTRL_ANALOG_DOWN  |\
	PADUTIL_CTRL_ANALOG_LEFT  \
)

#define PADUTIL_HPRM_NORMAL_KEYS (\
	PSP_HPRM_PLAYPAUSE |\
	PSP_HPRM_FORWARD   |\
	PSP_HPRM_BACK      |\
	PSP_HPRM_VOL_UP    |\
	PSP_HPRM_VOL_DOWN  \
)

#define PADUTIL_HPRM_TOGGLE_KEYS (\
	PSP_HPRM_HOLD \
)

#define PADUTIL_BUTTON_SYMBOLS \
	{ padutilSetPad( PSP_CTRL_SELECT ),           "SELECT" },\
	{ padutilSetPad( PSP_CTRL_START ),            "START"  },\
	{ padutilSetPad( PSP_CTRL_UP ),               "\x80"   },\
	{ padutilSetPad( PSP_CTRL_RIGHT ),            "\x81"   },\
	{ padutilSetPad( PSP_CTRL_DOWN ),             "\x82"   },\
	{ padutilSetPad( PSP_CTRL_LEFT ),             "\x83"   },\
	{ padutilSetPad( PADUTIL_CTRL_ANALOG_UP ),    "A\x80"  },\
	{ padutilSetPad( PADUTIL_CTRL_ANALOG_RIGHT ), "A\x81"  },\
	{ padutilSetPad( PADUTIL_CTRL_ANALOG_DOWN ),  "A\x82"  },\
	{ padutilSetPad( PADUTIL_CTRL_ANALOG_LEFT ),  "A\x83"  },\
	{ padutilSetPad( PSP_CTRL_LTRIGGER ),         "L"      },\
	{ padutilSetPad( PSP_CTRL_RTRIGGER ),         "R"      },\
	{ padutilSetPad( PSP_CTRL_TRIANGLE ),         "\x84"   },\
	{ padutilSetPad( PSP_CTRL_CIRCLE ),           "\x85"   },\
	{ padutilSetPad( PSP_CTRL_CROSS ),            "\x86"   },\
	{ padutilSetPad( PSP_CTRL_SQUARE ),           "\x87"   },\
	{ padutilSetPad( PSP_CTRL_HOME ),             "HOME"   },\
	{ padutilSetPad( PSP_CTRL_HOLD ),             "HOLD"   },\
	{ padutilSetPad( PSP_CTRL_NOTE ),             "\x88"   },\
	{ padutilSetPad( PSP_CTRL_SCREEN ),           "SCREEN" },\
	{ padutilSetPad( PSP_CTRL_VOLUP ),            "VOL+"   },\
	{ padutilSetPad( PSP_CTRL_VOLDOWN ),          "VOL-"   },\
	{ padutilSetPad( PSP_CTRL_WLAN_UP ),          "WLAN"   },\
	{ padutilSetPad( PSP_CTRL_REMOTE ),           "REMOTE" },\
	{ padutilSetPad( PSP_CTRL_DISC ),             "DISC"   },\
	{ padutilSetPad( PSP_CTRL_MS ),               "MS"     },\
	{ padutilSetHprm( PSP_HPRM_PLAYPAUSE ),       "RPLAY"  },\
	{ padutilSetHprm( PSP_HPRM_FORWARD ),         "RNEXT"  },\
	{ padutilSetHprm( PSP_HPRM_BACK ),            "RPREV"  },\
	{ padutilSetHprm( PSP_HPRM_VOL_UP ),          "RVOL+"  },\
	{ padutilSetHprm( PSP_HPRM_VOL_DOWN ),        "RVOL-"  },\
	{ padutilSetHprm( PSP_HPRM_HOLD ),            "RHOLD"  },\
	{ 0, NULL }

#define PADUTIL_BUTTON_NAMES \
	{ padutilSetPad( PSP_CTRL_SELECT ),           "SELECT"      },\
	{ padutilSetPad( PSP_CTRL_START ),            "START"       },\
	{ padutilSetPad( PSP_CTRL_UP ),               "Up"          },\
	{ padutilSetPad( PSP_CTRL_RIGHT ),            "Right"       },\
	{ padutilSetPad( PSP_CTRL_DOWN ),             "Down"        },\
	{ padutilSetPad( PSP_CTRL_LEFT ),             "Left"        },\
	{ padutilSetPad( PADUTIL_CTRL_ANALOG_UP ),    "AnalogUp"    },\
	{ padutilSetPad( PADUTIL_CTRL_ANALOG_RIGHT ), "AnalogRight" },\
	{ padutilSetPad( PADUTIL_CTRL_ANALOG_DOWN ),  "AnalogDown"  },\
	{ padutilSetPad( PADUTIL_CTRL_ANALOG_LEFT ),  "AnalogLeft"  },\
	{ padutilSetPad( PSP_CTRL_LTRIGGER ),         "LTrigger"    },\
	{ padutilSetPad( PSP_CTRL_RTRIGGER ),         "RTrigger"    },\
	{ padutilSetPad( PSP_CTRL_TRIANGLE ),         "Triangle"    },\
	{ padutilSetPad( PSP_CTRL_CIRCLE ),           "Circle"      },\
	{ padutilSetPad( PSP_CTRL_CROSS ),            "Cross"       },\
	{ padutilSetPad( PSP_CTRL_SQUARE ),           "Square"      },\
	{ padutilSetPad( PSP_CTRL_HOME ),             "HOME"        },\
	{ padutilSetPad( PSP_CTRL_HOLD ),             "HOLD"        },\
	{ padutilSetPad( PSP_CTRL_NOTE ),             "NOTE"        },\
	{ padutilSetPad( PSP_CTRL_SCREEN ),           "SCREEN"      },\
	{ padutilSetPad( PSP_CTRL_VOLUP ),            "VolUp"       },\
	{ padutilSetPad( PSP_CTRL_VOLDOWN ),          "VolDown"     },\
	{ padutilSetPad( PSP_CTRL_WLAN_UP ),          "WLAN"        },\
	{ padutilSetPad( PSP_CTRL_REMOTE ),           "REMOTE"      },\
	{ padutilSetPad( PSP_CTRL_DISC ),             "DISC"        },\
	{ padutilSetPad( PSP_CTRL_MS ),               "MS"          },\
	{ padutilSetHprm( PSP_HPRM_PLAYPAUSE ),       "RPlayPause"  },\
	{ padutilSetHprm( PSP_HPRM_FORWARD ),         "RForward"    },\
	{ padutilSetHprm( PSP_HPRM_BACK ),            "RBack"       },\
	{ padutilSetHprm( PSP_HPRM_VOL_UP ),          "RVolUp"      },\
	{ padutilSetHprm( PSP_HPRM_VOL_DOWN ),        "RVolDown"    },\
	{ padutilSetHprm( PSP_HPRM_HOLD ),            "RHOLD"       },\
	{ 0, NULL }

#ifdef __cplusplus
extern "C" {
#endif

/*=========================================================
	型宣言
=========================================================*/
typedef uint64_t       PadutilButtons;
typedef unsigned char  PadutilCoord;
typedef char           PadutilRelCoord;
typedef unsigned int   PadutilSensitivity;

typedef struct {
	PadutilCoord       originX;
	PadutilCoord       originY;
	PadutilCoord       deadzone;
	PadutilSensitivity sensitivity;
} PadutilAnalogStick;

typedef struct PadutilRemap {
	PadutilButtons realButtons;
	PadutilButtons remapButtons;
	struct PadutilRemap *prev;
	struct PadutilRemap *next;
} PadutilRemap;

typedef struct {
	PadutilButtons button;
	char *name;
} PadutilButtonName;

/*
	アナログパッドの方向を格納。
	ボタンコードとして使用し、enum PspCtrlButtonsが使用していない空きビットを使用。
*/
typedef enum {
	PADUTIL_CTRL_ANALOG_UP    = 0x00000002,
	PADUTIL_CTRL_ANALOG_RIGHT = 0x00000004,
	PADUTIL_CTRL_ANALOG_DOWN  = 0x00000400,
	PADUTIL_CTRL_ANALOG_LEFT  = 0x00000800
} PadutilAnalogDirection;

typedef enum {
	PADUTIL_OPT_TOKEN_SP  = 0x00000001,
	PADUTIL_OPT_IGNORE_SP = 0x00000002,
	PADUTIL_OPT_CASE_SENS = 0x00000004,
} PadutilOption;

/*=========================================================
	関数
=========================================================*/
/*-----------------------------------------------
	padutilCreateButtonSymbols/Names
	
	ボタンコードをボタン名のプリセット。
	Symbolsでは各ボタンのシンボル、Namesでは各ボタンの名前をセットしたPadutilButtonNames配列を返す。
	PadutilButtonNamesは本来リストだが、連続したメモリを確保しているため、配列としてアクセス可能。
	
	配列の末尾はボタンコード0、ボタン名NULLのエントリが必ずセットされる。
	
	PADUTIL_ARRAY_*定数で任意のボタンの添え字を指定可能。
	PADUTIL_ARRAY_ENDは、前述の末尾のエントリを指す。
-----------------------------------------------*/
PadutilButtonName *padutilCreateButtonSymbols( void );
void padutilDestroyButtonSymbols( void );

PadutilButtonName *padutilCreateButtonNames( void );
void padutilDestroyButtonNames( void );

/*-----------------------------------------------
	padutilGetButtonNamesByCode
	
	指定されたボタンコードをボタン名リストとデリミタに従って文字列化する。
-----------------------------------------------*/
char *padutilGetButtonNamesByCode( PadutilButtonName *availbtn, PadutilButtons buttons, const char *delim, unsigned int opt, char *buf, size_t len );

/*-----------------------------------------------
	padutilGetButtonCodeByNames
	
	ボタンの組み合わせを示す文字列をデリミタで区切ってボタン名リストと照合し、ボタンコードに変換する。
	ボタン名を示す文字列を直接操作するため、元の文字列は破壊される。
	元の文字列が必要である場合は、コピーを渡すこと。
-----------------------------------------------*/
PadutilButtons padutilGetButtonCodeByNames( PadutilButtonName *availbtn, char *names, const char *delim, unsigned int opt );

/*-----------------------------------------------
	padutilGetAnalogStickDirection
	
	アナログスティックの上下左右を、独自定義したアナログスティック方向コードで返す。
	アナログスティック方向コードは、通常のPSPボタンコードが使用していない隙間を使用している。
-----------------------------------------------*/
unsigned int padutilGetAnalogStickDirection( PadutilCoord x, PadutilCoord y, PadutilCoord deadzone );

/*-----------------------------------------------
	padutilSetAnalogStickDirection
	
-----------------------------------------------*/
void padutilSetAnalogStickDirection( unsigned int analog_direction, PadutilCoord *x, PadutilCoord *y );

/*-----------------------------------------------
	padutilCreateRemapArray
	
	リマップ定義用の配列を作成する。
	PadutilRemapは双方向リンクリストであるが、
	これを使うことにより、next/prevが前後の要素を指す配列を作成できる。
	全ての要素は配列のように隣り合っているため、forやポインタ演算で簡単に回せる。
	
	要素を初期化する目的でmemsetなどでゼロクリアしてはいけない。
	あらかじめ設定されたnext/prevまでゼロクリアされてしまう。
	要素のrealButtons/remapButtonsは自動的にに0で初期化されるため、
	改めて初期化する必要はない。
-----------------------------------------------*/
PadutilRemap *padutilCreateRemapArray( size_t count );

/*-----------------------------------------------
	padutilDestroyRemapArray
	
	PadutilCreateRemapArray()で作成した配列を解放する。
-----------------------------------------------*/
void padutilDestroyRemapArray( PadutilRemap *remap );

void padutilAdjustAnalogStick( const PadutilAnalogStick *analogstick, SceCtrlData *pad );
void padutilRemap( PadutilRemap *remap, PadutilButtons src, SceCtrlData *pad, u32 *hprmkey, bool redefine );

/*-----------------------------------------------
	padutilSetPad
	padutilSetHprm
	
	PadutilButtons型にはこのマクロをつかってボタンをセットする。
	
	padutilSetPadはボタンデータをセットする。とはいえ、
	今のところ実際には何もしていない。
	
	padutilSetHprmはリモコンのキーデータをセットする。
	ビットをずらしてボタンデータと重ならないようにしているため、必ずこのマクロを通さなければいけない。
	
	PadutilButtons型へ、パッドボタンとリモコンキーを組み合わせてセットする場合は、
	それぞれの返り値の論理和を渡す。
	    PadutilButtons buttons = padutilSetPad( PSP_CTRL_START ) | padutilSetHprm( PSP_HPRM_PLAYPAUSE );
	
	@param: uint32_t btn
		enum PspCtrlButtonsかenum PspHprmKeysの値。
	
	@return uint64_t
		変換された値。
-----------------------------------------------*/
#define padutilSetPad( btn )  ( (PadutilButtons)( btn ) )
#define padutilSetHprm( btn ) ( (PadutilButtons)( (PadutilButtons)( btn ) << 32 ) )

/*-----------------------------------------------
	padutilGetPad
	padutilGetHprm
	
	PadutilButtons型にはこれらのマクロをつかってデータを抽出する。
	
	padutilGetPadはボタンデータを取り出す。
	
	padutilGetHprmはリモコンのキーデータを取り出す。
		
	@param: uint64_t btn
		padutilButtons()マクロで生成した値。
	
	@return uint32_t
		取り出された値。
-----------------------------------------------*/
#define padutilGetPad( btn )  ( (unsigned int)btn )
#define padutilGetHprm( btn ) ( (unsigned int)( (PadutilButtons)( btn ) >> 32 ) )


#ifdef __cplusplus
}
#endif

#endif

