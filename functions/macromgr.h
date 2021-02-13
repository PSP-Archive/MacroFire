/*=========================================================

	macromgr.c

	マクロマネージャ

=========================================================*/
#ifndef FUNCTION_MACROMGR_H
#define FUNCTION_MACROMGR_H

#include "../macrofire.h"
#include "memory/memory.h"
#include "memory/dmem.h"

/*=========================================================
	マクロ
=========================================================*/
#define MACROMGR_MAX_DELAY                999999999UL

#define MACROMGR_SET_HILO_DWORD( x, y )   ( (u64)(( (u64)( x ) << 32 ) | ( y )) )
#define MACROMGR_GET_HI_DWORD( x )        ( (u32)(( x ) >> 32) )
#define MACROMGR_GET_LO_DWORD( y )        ( (u32)( y ) )

#define MACROMGR_SET_ANALOG_XY( x, y )    MACROMGR_SET_HILO_DWORD( x, y )
#define MACROMGR_GET_ANALOG_X( x )        MACROMGR_GET_HI_DWORD( x )
#define MACROMGR_GET_ANALOG_Y( y )        MACROMGR_GET_LO_DWORD( y )
#define MACROMGR_ANALOG_CENTER            127
#define MACROMGR_ANALOG_NEUTRAL           MACROMGR_SET_ANALOG_XY( MACROMGR_ANALOG_CENTER, MACROMGR_ANALOG_CENTER )

#define MACROMGR_SET_RAPIDDELAY( pd, rd ) MACROMGR_SET_HILO_DWORD( pd, rd )
#define MACROMGR_GET_RAPIDPDELAY( pd )    MACROMGR_GET_HI_DWORD( pd )
#define MACROMGR_GET_RAPIDRDELAY( rd )    MACROMGR_GET_LO_DWORD( rd )

#define MACROMGR_MACRO_VERSION           5

#define MACROMGR_INI_KEY_DELAY           "Delay"
#define MACROMGR_INI_KEY_BUTTONS_PRESS   "ButtonsPress"
#define MACROMGR_INI_KEY_BUTTONS_RELEASE "ButtonsRelease"
#define MACROMGR_INI_KEY_BUTTONS_CHANGE  "ButtonsChange"
#define MACROMGR_INI_KEY_ANALOG_MOVE     "AnalogMove"
#define MACROMGR_INI_KEY_RAPIDFIRE_START "RapidfireStart"
#define MACROMGR_INI_KEY_RAPIDFIRE_STOP  "RapidfireStop"


/*=========================================================
	型宣言
=========================================================*/
typedef intptr_t MacromgrUID;
typedef uint64_t MacromgrData;

typedef enum {
	MACROMGR_SEEK_SET = 1,
	MACROMGR_SEEK_END = 2,
	MACROMGR_SEEK_CUR = 4
} MacromgrWhence;

typedef enum {
	MACROMGR_DELAY = 0,
	MACROMGR_BUTTONS_PRESS,
	MACROMGR_BUTTONS_RELEASE,
	MACROMGR_BUTTONS_CHANGE,
	MACROMGR_ANALOG_MOVE,
	MACROMGR_RAPIDFIRE_START,
	MACROMGR_RAPIDFIRE_STOP
} MacromgrAction;

typedef struct macromgr_data {
	MacromgrAction action;
	MacromgrData data;
	MacromgrData sub;
	struct macromgr_data *next;
	struct macromgr_data *prev;
} MacromgrCommand;

/*=========================================================
	関数
=========================================================*/
MacromgrUID macromgrNew( void );
void macromgrDestroy( MacromgrUID uid );
MacromgrCommand *macromgrSeek( MacromgrUID uid, unsigned int offset, MacromgrWhence whence, MacromgrCommand *cur );
inline MacromgrCommand *macromgrNext( MacromgrCommand *macro );
inline MacromgrCommand *macromgrPrev( MacromgrCommand *macro );
MacromgrCommand *macromgrCreateRoot( MacromgrUID uid );
MacromgrCommand *macromgrInsertBefore( MacromgrUID uid, MacromgrCommand *macro );
MacromgrCommand *macromgrInsertAfter( MacromgrUID uid, MacromgrCommand *macro );
bool macromgrRemove( MacromgrUID uid, MacromgrCommand *macro );
void macromgrClear( MacromgrUID uid );
int macromgrLoader( InimgrContext ctx, void *arg );
int macromgrAppendLoader( InimgrContext ctx, void *arg );
int macromgrSaver( InimgrContext ctx, void *arg );

/*
int macromgrLoad( MacromgrUID uid, const char *path );
int macromgrAppendLoad( MacromgrUID uid, const char *path );
int macromgrSave( MacromgrUID uid, const char *path );
*/
bool macromgrGetCommand( MacromgrCommand *macro, MacromgrAction *action, uint64_t *data, uint64_t *sub );
void macromgrSetCommand( MacromgrCommand *macro, MacromgrAction action, uint64_t data, uint64_t sub );

#define macromgrSetDelay( macro, delay ) macromgrSetCommand( macro, MACROMGR_DELAY, delay, 0 )
#define macromgrSetButtonsPress( macro, buttons ) macromgrSetCommand( macro, MACROMGR_BUTTONS_PRESS, buttons, 0 )
#define macromgrSetButtonsRelease( macro, buttons ) macromgrSetCommand( macro, MACROMGR_BUTTONS_RELEASE, buttons, 0 )
#define macromgrSetButtonsChange( macro, buttons ) macromgrSetCommand( macro, MACROMGR_BUTTONS_CHANGE, buttons, 0 )
#define macromgrSetAnalogMove( macro, x, y ) macromgrSetCommand( macro, MACROMGR_ANALOG_MOVE, MACROMGR_SET_ANALOG_XY( x, y ), 0 )
#define macromgrSetRapidfireStart( macro, buttons, pdelay, rdelay ) macromgrSetCommand( macro, MACROMGR_RAPIDFIRE_START, buttons, MACROMGR_SET_RAPIDDELAY( pdelay, rdelay )
#define macromgrSetRapidfireStop( macro, buttons ) macromgrSetCommand( macro, MACROMGR_RAPIDFIRE_STOP, buttons, 0 )

#endif
