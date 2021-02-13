/*=========================================================

	cdialog.c

	共通ダイアログ。

=========================================================*/
#include "dev.h"

/*=========================================================
	ローカル宣言
=========================================================*/

/*=========================================================
	ローカル変数
=========================================================*/
static SceUID                    st_semaid;
static struct cdialog_dev_color  st_color;
static PadutilRemap              *st_remap;

static unsigned int st_envvar;

/* ヘルプメッセージ用 */
static struct cdialog_dev_help *st_help;
static size_t st_help_count;

/*=========================================================
	関数
=========================================================*/
bool cdialogInit( void )
{
	st_semaid = sceKernelCreateSema( "cg_cdialog_sema", 0, 1, 1, 0 );
	if( ! st_semaid ) return false;
	
	st_color.fg     = 0xff222222;
	st_color.bg     = 0xddffffff;
	st_color.fcfg   = 0xff0000ff;
	st_color.fcbg   = 0xffbbffff;
	st_color.border = 0x66000000;
	st_color.title  = 0xff222244;
	st_color.help   = 0xffaa0000;
	st_color.extra  = 0xffff0000;
	
	st_envvar = 0;
	
	return true;
}

void cdialogEnable( unsigned int env )
{
	st_envvar |= env;
}

void cdialogDisable( unsigned int env )
{
	st_envvar &= ~env;
}

void cdialogFinish( void )
{
	sceKernelDeleteSema( st_semaid );
}

void cdialogSetRemap( PadutilRemap *remap )
{
	st_remap = remap;
}

void cdialogClearRemap( void )
{
	st_remap = NULL;
}

/* 開発用 */
bool cdialogDevLock( void )
{
	int ret = sceKernelPollSema( st_semaid, 1 );
	return ret < 0 ? false : true;
}

bool cdialogDevUnlock( void )
{
	int ret = sceKernelSignalSema( st_semaid, 1 );
	return ret < 0 ? false : true;
}

unsigned int cdialogDevGetEnvvar( void )
{
	return st_envvar;
}

unsigned int cdialogDevAcceptButton( void )
{
	return st_envvar & CDIALOG_ACCEPT_CROSS ? PSP_CTRL_CROSS : PSP_CTRL_CIRCLE;
}

unsigned int cdialogDevCancelButton( void )
{
	return st_envvar & CDIALOG_ACCEPT_CROSS ? PSP_CTRL_CIRCLE : PSP_CTRL_CROSS;
}

char *cdialogDevAcceptSymbol( void )
{
	return st_envvar & CDIALOG_ACCEPT_CROSS ? PB_SYM_PSP_CROSS : PB_SYM_PSP_CIRCLE;
}

char *cdialogDevCancelSymbol( void )
{
	return st_envvar & CDIALOG_ACCEPT_CROSS ? PB_SYM_PSP_CIRCLE: PB_SYM_PSP_CROSS;
}

void cdialogDevInitBaseParams( struct cdialog_dev_base_params *params )
{
	params->status = CDIALOG_INIT;
	params->result = CDIALOG_UNKNOWN;
	params->x      = 0;
	params->y      = 0;
	params->width  = 0;
	params->height = 0;
	params->color  = &st_color;
	params->paduid = 0;
	params->analogStick.originX     = PADUTIL_CENTER_X;
	params->analogStick.originY     = PADUTIL_CENTER_Y;
	params->analogStick.deadzone    = 40;
	params->analogStick.sensitivity = 1.0f;
	params->remap = NULL;
	params->help = NULL;
}

int cdialogDevPrepareToStart( struct cdialog_dev_base_params *params, unsigned int options )
{
	params->paduid = padctrlNew();
	if( ! params->paduid ) return CG_ERROR_NOT_ENOUGH_MEMORY;
	
	if( options & CDIALOG_DISPLAY_CENTER ){
		params->x = ( SCR_WIDTH  >> 1 ) - ( params->width  >> 1 );
		params->y = ( SCR_HEIGHT >> 1 ) - ( params->height >> 1 );
		if( params->x > SCR_WIDTH  ) params->x = 0;
		if( params->y > SCR_HEIGHT ) params->y = 0;
	}
	
	return CG_ERROR_OK;
}

void cdialogDevPrepareToFinish( struct cdialog_dev_base_params *params )
{
	padctrlDestroy( params->paduid );
	params->paduid = 0;
}

void cdialogDevSetAnalogStickAdjust( struct cdialog_dev_base_params *params, PadutilCoord x, PadutilCoord y, PadutilCoord deadzone, PadutilSensitivity sens )
{
	params->analogStick.originX     = x;
	params->analogStick.originY     = y;
	params->analogStick.deadzone    = deadzone;
	params->analogStick.sensitivity = sens;
}

void cdialogDevSetRepeatButtons( struct cdialog_dev_base_params *params, unsigned int buttons )
{
	padctrlSetRepeatButtons( params->paduid, buttons );
}

PadutilButtons cdialogDevReadCtrlBuffer( struct cdialog_dev_base_params *params, SceCtrlData *pad, u32 *hprmkey )
{
	SceCtrlData real_pad;
	u32 real_hprmkey;
	
	real_pad.Buttons = padctrlPeekBuffer( params->paduid, pad, 1 );
	padutilAdjustAnalogStick( &(params->analogStick), pad );
	sceHprmPeekCurrentKey( &real_hprmkey );
	
	real_pad.Lx = pad->Lx;
	real_pad.Ly = pad->Ly;
	real_pad.Buttons |= padutilGetAnalogStickDirection( real_pad.Lx, real_pad.Ly, params->analogStick.deadzone );
	
	if( st_remap ) padutilRemap( st_remap, padutilSetPad( real_pad.Buttons ) | padutilSetHprm( real_hprmkey ), &real_pad, &real_hprmkey, true );
	
	if( pad->Buttons & ( PADUTIL_PAD_NORMAL_BUTTONS | PADUTIL_PAD_ANALOGDIR_BUTTONS ) ) pad->Buttons = real_pad.Buttons;
	if( hprmkey && ( *hprmkey & PADUTIL_HPRM_NORMAL_KEYS ) ) *hprmkey = real_hprmkey;
	
	return (PadutilButtons)( padutilSetPad( real_pad.Buttons ) | padutilSetHprm( real_hprmkey ) );
}

void cdialogDevHelp( struct cdialog_dev_help *help, unsigned int x, unsigned int y, const char *str )
{
	help->x   = x;
	help->y   = y;
	help->str = str;
}

void cdialogDevSetHelp( struct cdialog_dev_help *help, size_t count )
{
	st_help = help;
	st_help_count = count;
}

int cdialogDevDrawHelp( unsigned int x, unsigned int y, struct cdialog_dev_color *colors )
{
	size_t count = st_help_count;
	while( count-- ){
		pbPrint( x + st_help[count].x, y + st_help[count].y, colors->fg, PB_TRANSPARENT, st_help[count].str );
	}
	return CG_ERROR_OK;
}

