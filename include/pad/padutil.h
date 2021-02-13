/*=========================================================

	padutil.c

	�p�b�h�f�[�^�����B

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
	�}�N��
=========================================================*/
#define PADUTIL_CENTER_X   127
#define PADUTIL_CENTER_Y   127
#define PADUTIL_MAX_COORD  255
#define PADUTIL_MAX_RADIUS 182 /* �ӂ̒��� * 2�̕����� (�����_�ȉ��؂�グ) */

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
	�^�錾
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
	�A�i���O�p�b�h�̕������i�[�B
	�{�^���R�[�h�Ƃ��Ďg�p���Aenum PspCtrlButtons���g�p���Ă��Ȃ��󂫃r�b�g���g�p�B
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
	�֐�
=========================================================*/
/*-----------------------------------------------
	padutilCreateButtonSymbols/Names
	
	�{�^���R�[�h���{�^�����̃v���Z�b�g�B
	Symbols�ł͊e�{�^���̃V���{���ANames�ł͊e�{�^���̖��O���Z�b�g����PadutilButtonNames�z���Ԃ��B
	PadutilButtonNames�͖{�����X�g�����A�A���������������m�ۂ��Ă��邽�߁A�z��Ƃ��ăA�N�Z�X�\�B
	
	�z��̖����̓{�^���R�[�h0�A�{�^����NULL�̃G���g�����K���Z�b�g�����B
	
	PADUTIL_ARRAY_*�萔�ŔC�ӂ̃{�^���̓Y�������w��\�B
	PADUTIL_ARRAY_END�́A�O�q�̖����̃G���g�����w���B
-----------------------------------------------*/
PadutilButtonName *padutilCreateButtonSymbols( void );
void padutilDestroyButtonSymbols( void );

PadutilButtonName *padutilCreateButtonNames( void );
void padutilDestroyButtonNames( void );

/*-----------------------------------------------
	padutilGetButtonNamesByCode
	
	�w�肳�ꂽ�{�^���R�[�h���{�^�������X�g�ƃf���~�^�ɏ]���ĕ����񉻂���B
-----------------------------------------------*/
char *padutilGetButtonNamesByCode( PadutilButtonName *availbtn, PadutilButtons buttons, const char *delim, unsigned int opt, char *buf, size_t len );

/*-----------------------------------------------
	padutilGetButtonCodeByNames
	
	�{�^���̑g�ݍ��킹��������������f���~�^�ŋ�؂��ă{�^�������X�g�Əƍ����A�{�^���R�[�h�ɕϊ�����B
	�{�^����������������𒼐ڑ��삷�邽�߁A���̕�����͔j�󂳂��B
	���̕����񂪕K�v�ł���ꍇ�́A�R�s�[��n�����ƁB
-----------------------------------------------*/
PadutilButtons padutilGetButtonCodeByNames( PadutilButtonName *availbtn, char *names, const char *delim, unsigned int opt );

/*-----------------------------------------------
	padutilGetAnalogStickDirection
	
	�A�i���O�X�e�B�b�N�̏㉺���E���A�Ǝ���`�����A�i���O�X�e�B�b�N�����R�[�h�ŕԂ��B
	�A�i���O�X�e�B�b�N�����R�[�h�́A�ʏ��PSP�{�^���R�[�h���g�p���Ă��Ȃ����Ԃ��g�p���Ă���B
-----------------------------------------------*/
unsigned int padutilGetAnalogStickDirection( PadutilCoord x, PadutilCoord y, PadutilCoord deadzone );

/*-----------------------------------------------
	padutilSetAnalogStickDirection
	
-----------------------------------------------*/
void padutilSetAnalogStickDirection( unsigned int analog_direction, PadutilCoord *x, PadutilCoord *y );

/*-----------------------------------------------
	padutilCreateRemapArray
	
	���}�b�v��`�p�̔z����쐬����B
	PadutilRemap�͑o���������N���X�g�ł��邪�A
	������g�����Ƃɂ��Anext/prev���O��̗v�f���w���z����쐬�ł���B
	�S�Ă̗v�f�͔z��̂悤�ɗׂ荇���Ă��邽�߁Afor��|�C���^���Z�ŊȒP�ɉ񂹂�B
	
	�v�f������������ړI��memset�ȂǂŃ[���N���A���Ă͂����Ȃ��B
	���炩���ߐݒ肳�ꂽnext/prev�܂Ń[���N���A����Ă��܂��B
	�v�f��realButtons/remapButtons�͎����I�ɂ�0�ŏ���������邽�߁A
	���߂ď���������K�v�͂Ȃ��B
-----------------------------------------------*/
PadutilRemap *padutilCreateRemapArray( size_t count );

/*-----------------------------------------------
	padutilDestroyRemapArray
	
	PadutilCreateRemapArray()�ō쐬�����z����������B
-----------------------------------------------*/
void padutilDestroyRemapArray( PadutilRemap *remap );

void padutilAdjustAnalogStick( const PadutilAnalogStick *analogstick, SceCtrlData *pad );
void padutilRemap( PadutilRemap *remap, PadutilButtons src, SceCtrlData *pad, u32 *hprmkey, bool redefine );

/*-----------------------------------------------
	padutilSetPad
	padutilSetHprm
	
	PadutilButtons�^�ɂ͂��̃}�N���������ă{�^�����Z�b�g����B
	
	padutilSetPad�̓{�^���f�[�^���Z�b�g����B�Ƃ͂����A
	���̂Ƃ�����ۂɂ͉������Ă��Ȃ��B
	
	padutilSetHprm�̓����R���̃L�[�f�[�^���Z�b�g����B
	�r�b�g�����炵�ă{�^���f�[�^�Əd�Ȃ�Ȃ��悤�ɂ��Ă��邽�߁A�K�����̃}�N����ʂ��Ȃ���΂����Ȃ��B
	
	PadutilButtons�^�ցA�p�b�h�{�^���ƃ����R���L�[��g�ݍ��킹�ăZ�b�g����ꍇ�́A
	���ꂼ��̕Ԃ�l�̘_���a��n���B
	    PadutilButtons buttons = padutilSetPad( PSP_CTRL_START ) | padutilSetHprm( PSP_HPRM_PLAYPAUSE );
	
	@param: uint32_t btn
		enum PspCtrlButtons��enum PspHprmKeys�̒l�B
	
	@return uint64_t
		�ϊ����ꂽ�l�B
-----------------------------------------------*/
#define padutilSetPad( btn )  ( (PadutilButtons)( btn ) )
#define padutilSetHprm( btn ) ( (PadutilButtons)( (PadutilButtons)( btn ) << 32 ) )

/*-----------------------------------------------
	padutilGetPad
	padutilGetHprm
	
	PadutilButtons�^�ɂ͂����̃}�N���������ăf�[�^�𒊏o����B
	
	padutilGetPad�̓{�^���f�[�^�����o���B
	
	padutilGetHprm�̓����R���̃L�[�f�[�^�����o���B
		
	@param: uint64_t btn
		padutilButtons()�}�N���Ő��������l�B
	
	@return uint32_t
		���o���ꂽ�l�B
-----------------------------------------------*/
#define padutilGetPad( btn )  ( (unsigned int)btn )
#define padutilGetHprm( btn ) ( (unsigned int)( (PadutilButtons)( btn ) >> 32 ) )


#ifdef __cplusplus
}
#endif

#endif

