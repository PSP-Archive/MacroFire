/* mfmenu_types.h */

#include "mfmenu.h"
#include <pspge.h>
#include <pspsdk.h>
#include <pspsuspend.h>
#include <psputility.h>
#include <stdarg.h>

/*=========================================================
	extern用
=========================================================*/
#ifdef MFMENU_INIT
#define GLOBAL
#define INIT( x ) = x
#else
#define GLOBAL extern
#define INIT( x )
#endif

/*=========================================================
	定数/マクロ
=========================================================*/
#define MF_MENU_MAX_NUMBER_OF_THREADS 64

#define MF_MENU_ALT_BUTTONS_COUNT 13
#define MF_MENU_ALT_BUTTONS \
	{ PSP_CTRL_SELECT,   "SELECT"   },\
	{ PSP_CTRL_START,    "START"    },\
	{ PSP_CTRL_UP,       "UP"       },\
	{ PSP_CTRL_RIGHT,    "RIGHT"    },\
	{ PSP_CTRL_DOWN,     "DOWN"     },\
	{ PSP_CTRL_LEFT,     "LEFT"     },\
	{ PSP_CTRL_LTRIGGER, "LTRIGGER" },\
	{ PSP_CTRL_RTRIGGER, "RTRIGGER" },\
	{ PSP_CTRL_TRIANGLE, "TRIANGLE" },\
	{ PSP_CTRL_CIRCLE,   "CIRCLE"   },\
	{ PSP_CTRL_CROSS,    "CROSS"    },\
	{ PSP_CTRL_SQUARE,   "SQUARE"   },\
	{ PSP_CTRL_HOME,     "HOME"     },\
	{ 0, NULL }


/*=========================================================
	ローカル型宣言
=========================================================*/
struct MfMenuFrameBufferAddr {
	void *Addr;
	void *Restore;
};

struct MfMenuFrameBuffer {
	unsigned int Size;
	void *Backup;
	
	struct MfMenuFrameBufferAddr Frame[3];
};

struct MfMenuThreads {
	SceUID List[MF_MENU_MAX_NUMBER_OF_THREADS];
	int    Count;
};

struct MfMenuPosition {
	unsigned int CurTable;
	unsigned int CurRow;
	unsigned int CurCol;
};

struct MfMenuMessage {
	MfMenuMessage   Main;
	MfMessage       Sub;
	MfMessage       Signal;
};

struct MfMenuExtra {
	MfMenuExtraProc Proc;
	void            *Argp;
};

struct MfMenuStackData {
	MfProc *Proc;
	struct MfMenuPosition Pos;
};

struct MfMenuStack {
	struct MfMenuStackData *Data;
	unsigned int Index;
	unsigned int Size;
};

struct MfMenuInput {
	SceCtrlData *Pad;
	MfHprmKey   *HprmKey;
	PadctrlUID  Uid;
	
	struct {
		unsigned int Button;
		char *Symbol;
	} Accept, Cancel;
};

struct MfMenuPref {
	bool LowMemoryMode;
	PadutilRemap *Remap;
};

struct MfMenuInfoBar {
	char Text[MF_MENU_INFOTEXT_LENGTH];
	unsigned int Options;
};

struct MfMenuTempMemory {
	uintptr_t procAddr;
	void **memList;
	struct MfMenuTempMemory *next;
	struct MfMenuTempMemory *prev;
};

struct MfMenuParams {
	DmemUID MemoryUid;
	struct MfMenuFrameBuffer FrameBuffer;
	struct MfMenuThreads     Thread;
	struct MfMenuPosition    Menu;
	struct MfMenuStack       Stack;
	struct MfMenuTempMemory  *Tmem;
	struct MfMenuInput       Ctrl;
	struct MfMenuInfoBar     Info;
	struct MfMenuMessage     Message;
	struct MfMenuExtra       Extra;
	PadutilButtonName        *ButtonSymbolTable;
	MfDialogType LastDialogType;
	unsigned int  Flags;
	unsigned int  WaitMicroSecForUpdate;
	int ProcRetv;
	MfFuncMenu Proc, NextProc;
};

enum MfMenuStackAction {
	MF_MENU_STACK_CREATE,
	MF_MENU_STACK_PUSH,
	MF_MENU_STACK_POP,
	MF_MENU_STACK_DESTROY
};

enum MfMenuMoveDirection {
	MF_MENU_MOVE_UP,
	MF_MENU_MOVE_RIGHT,
	MF_MENU_MOVE_DOWN,
	MF_MENU_MOVE_LEFT
};
	
struct MfMenuMainPref{
	bool engine;
	PadutilButtons menu;
	PadutilButtons toggle;
	bool statusNotification;
};

/* MfMenuMessageと競合しない値 */
enum MfMenuInternalMessage {
	MF_MENU_IM_BACK = 0x01000000,
};

/*=========================================================
	内部関数
=========================================================*/
void __mfmenu_clear_temp_memory( void *proc );
void __mfmenu_send_message_for_items( MfMenuTable *menu, unsigned int menucnt, MfMessage message );
void __mfmenu_table_entry_activator( MfMenuTable *menu, unsigned int row, unsigned int col, bool mode );

/*=========================================================
	グローバル変数
=========================================================*/
GLOBAL struct MfMenuThreads __mfmenu__gSysThreads;
GLOBAL struct MfMenuPref    __mfmenu__gPref;
GLOBAL struct MfMenuParams  *__mfmenu__gParams;

#undef GLOBAL
#undef INIT
