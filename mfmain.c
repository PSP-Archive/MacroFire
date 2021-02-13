/*==========================================================

	mfmain.c

	MacroFire メインループ

==========================================================*/
#include "mfmain.h"
#include "ovmsg.h"
#include <pspinit.h>
#include <pspumd.h>
#include <psputils.h>

PSP_MODULE_INFO( "MacroFire", PSP_MODULE_KERNEL, 3, 2 );

/*=========================================================
	定数
=========================================================*/


/*=========================================================
	ローカル関数
=========================================================*/
static bool mf_init( void );
static void mf_shutdown( void );
static void mf_get_gameid( SceUID fd, char *gameid );
static void mf_get_gameid_from_sha1hash( SceUID fd, char *gameid );
static void mf_ini( const char *path );
static void mf_ready( const char *cwd );
static void mf_find_hook_address( void );
static void mf_sending_toggle_message( bool engine );
static void mf_ini_load( InimgrUID ini );
static void mf_ini_save( InimgrUID ini );
static int mf_get_current_ctrl( SceCtrlData *pad, MfHprmKey *hk );
static unsigned int mf_syscon_peek_dword( uint8_t *addr );
static void mf_syscon_poke_dword( uint8_t *addr, unsigned int data );
static unsigned char mf_syscon_make_rx_checksum( SceSysconPacket *packet );
static void mf_syscon_handler_handler1( SceSysconPacket *packet );
static void mf_syscon_handler_transmit( SceSysconPacket *packet );
static void mf_syscon_handler_recieve( SceSysconPacket *packet );
static void mf_syscon_handler_handler4( SceSysconPacket *packet );
#ifdef DEBUG
static void mf_syscom_dump_packet( SceSysconPacket *packet );
#endif
static unsigned int mf_conv_buttons_syscon_to_psp( unsigned int sysconbuttons );
static unsigned int mf_conv_buttons_psp_to_syscon( unsigned int buttons );
static void mf_call_functions( MfHookAction action, SceCtrlData *pad, MfHprmKey *hk );

/*=========================================================
	ローカル変数
=========================================================*/
static char st_game_id[10];
static const char *st_ini_sections[3];

static bool         st_hooked       = false;
static MfWorldId    st_world        = 0;
static u32          st_prev_hprmkey = 0;

static bool           st_is_enabled          = MF_INI_STARTUP;
static PadutilButtons st_menu_buttons        = MF_INI_MENU_BUTTONS;
static PadutilButtons st_toggle_buttons      = MF_INI_TOGGLE_BUTTONS;
static bool           st_status_notification = MF_INI_STATUS_NOTIFICATION;

static PadutilButtonName *st_cvbtn;

static SceCtrlData st_pad, st_raw_pad;
static MfHprmKey   st_hk,  st_raw_hk;

static OvmsgUID st_ovmsg;

static MfHookEntry            st_syscon_hook = { { NULL }, "sceSYSCON_Driver", "sceSyscon_driver", 0x5EE92F3C, mfSysconSetDebugHandlers };
static SceSysconDebugHandlers *st_syscon_chain_handlers = NULL;
static SceSysconDebugHandlers st_syscon_handlers = {
	mf_syscon_handler_handler1,
	mf_syscon_handler_transmit,
	mf_syscon_handler_recieve,
	mf_syscon_handler_handler4
};

#ifdef DEBUG
int gDebug[10];
#endif

SceUID st_thid;

/*-----------------------------------------------
	MacroFireの初期化と終了
-----------------------------------------------*/
static bool mf_init( void )
{
	const char *executable = sceKernelInitFileName();
	
	st_ini_sections[2] = MF_INI_SECTION_DEFAULT;
	
	/* 最初に行う */
	dbgprintf( "Initializing menu");
	if( ! mfMenuInit() ) gRunning = false;
	
	/* 実行環境取得 */
	switch( sceKernelInitKeyConfig() ){
		case PSP_INIT_KEYCONFIG_VSH:  
			st_world = MF_WORLD_VSH;
			st_ini_sections[1] = MF_INI_SECTION_VSH;
			break;
		case PSP_INIT_KEYCONFIG_POPS: 
			st_world = MF_WORLD_POPS;
			st_ini_sections[1] = MF_INI_SECTION_POPS;
			break;
		case PSP_INIT_KEYCONFIG_GAME: 
			st_world = MF_WORLD_GAME;
			st_ini_sections[1] = MF_INI_SECTION_GAME;
			break;
	}
	
	/* ゲームIDを取得 */
	if( st_world == MF_WORLD_GAME && sceKernelBootFrom() == PSP_BOOT_DISC && sceKernelInitApitype() != PSP_INIT_APITYPE_DISC_UPDATER  ){
		SceUID sfo;
		
		/* UMDをdisc0:へマウント */
		sceUmdActivate( 1, "disc0:" );
		
		/* 初期化終了を待つ */
		sceUmdWaitDriveStat( PSP_UMD_READY );
		
		sfo = sceIoOpen( "disc0:/PSP_GAME/PARAM.SFO", PSP_O_RDONLY, 0777 ); 
		if( CG_IS_VALID_UID( sfo ) ){
			mf_get_gameid( sfo, st_game_id );
			sceIoClose( sfo );
		}
		
		/* PSPのオリジナルのローダーがUMDをマウントするはずなので、アンマウント */
		sceUmdDeactivate( 1, "disc0:" );
	} else if( st_world == MF_WORLD_POPS ){
		SceUID pbp = sceIoOpen( executable, PSP_O_RDONLY, 0777 ); 
		if( CG_IS_VALID_UID( pbp ) ){
			struct MfPbpHeader pbp_header;
			sceIoRead( pbp, &pbp_header, sizeof( pbp_header ) );
			if( memcmp( pbp_header.Signature, "\0PBP", 4 ) == 0 ){
				sceIoLseek( pbp, pbp_header.Offset[PARAM_SFO], PSP_SEEK_SET );
				mf_get_gameid( pbp, st_game_id );
				if( st_game_id[0] == '\0' ) mf_get_gameid_from_sha1hash( pbp, st_game_id );
			}
			sceIoClose( pbp );
		}
	} else if( st_world != MF_WORLD_VSH && executable ){
		SceUID pbp = sceIoOpen( executable, PSP_O_RDONLY, 0777 );
		if( CG_IS_VALID_UID( pbp ) ){
			mf_get_gameid_from_sha1hash( pbp, st_game_id );
			sceIoClose( pbp );
		}
	}
	
	if( *st_game_id != '\0' ){
		st_ini_sections[0] = st_game_id;
	} else{
		strutilCopy( st_game_id, st_ini_sections[1], sizeof( st_game_id ) );
	}
	
	return true;
}

static void mf_shutdown( void )
{
	int i;
	MfFuncTerm termfunc;
	
	mfUnhook();
	
	dbgprint( "Sending message MF_MS_TERM to all functions..." );
	for( i = 0; i < gMftabEntryCount; i++ ){
		if( gMftab[i].proc ){
			termfunc = ( gMftab[i].proc )( MF_MS_TERM );
			if( termfunc ) termfunc();
		}
	}
	
	if( mfNotificationThreadId() ) mfNotificationShutdownStart();
	
	mfMenuDestroy();
	
	if( st_syscon_chain_handlers ) sceSysconSetDebugHandlers( st_syscon_chain_handlers );
}

static void mf_get_gameid_from_sha1hash( SceUID fd, char *gameid )
{
	SceKernelUtilsSha1Context ctx;
	int readbytes;
	uint8_t digest[20];
	uint8_t buf[2048]; /* MacroFireのメインスレッドではなくmodule_startから呼び出すことによって、3KBほどのスタックがある */
	
	sceIoLseek( fd, 0, PSP_SEEK_SET );
	
	sceKernelUtilsSha1BlockInit( &ctx );
	
	while( ( readbytes = sceIoRead( fd, buf, sizeof( buf ) ) ) )
		sceKernelUtilsSha1BlockUpdate( &ctx, buf, readbytes );
	
	if( CG_IS_ERROR( sceKernelUtilsSha1BlockResult( &ctx, digest ) ) ) return;
	snprintf( gameid, 10, "%08X", (digest[3]) | (digest[2]) << 8 | (digest[1]) << 16 | (digest[0]) << 24 ); 
}

static void mf_get_gameid( SceUID fd, char *gameid )
{
	struct MfPsfHeader  psf_header;
	struct MfPsfSection psf_section;
	SceOff psf_start;
	SceOff save_section_offset;
	char key[7];
	
	psf_start = sceIoLseek( fd, 0, PSP_SEEK_CUR );
	
	sceIoRead( fd, &psf_header, sizeof( psf_header ) );
	save_section_offset = sceIoLseek( fd, 0, PSP_SEEK_CUR );
	
	while( psf_header.KeysCount-- ){
		save_section_offset = sceIoLseek( fd, save_section_offset, PSP_SEEK_SET ) + sizeof( psf_section );
		sceIoRead( fd, &psf_section, sizeof( psf_section ) );
		
		sceIoLseek( fd, psf_start + psf_header.KeyTableOffset + psf_section.KeyOffset, PSP_SEEK_SET );
		sceIoRead( fd, key, sizeof( key ) );
		if( strncmp( key, "DISC_ID", 7 ) == 0 ){
			sceIoLseek( fd, psf_start + psf_header.ValueTableOffset + psf_section.ValueOffset, PSP_SEEK_SET );
			sceIoRead( fd, gameid, 10 );
			gameid[9] = '\0';
		}
	}
}

/*-----------------------------------------------
	Sysconデータコントロール
-----------------------------------------------*/
static unsigned int mf_syscon_peek_dword( uint8_t *addr )
{
	return addr[0] | addr[1] << 8 | addr[2] << 16 | addr[3] << 24;
}

static void mf_syscon_poke_dword( uint8_t *addr, unsigned int data )
{
	addr[0] = (uint8_t)( data );
	addr[1] = (uint8_t)( data >> 8 );
	addr[2] = (uint8_t)( data >> 16 );
	addr[3] = (uint8_t)( data >> 24 );
}

static unsigned char mf_syscon_make_rx_checksum( SceSysconPacket *packet )
{
	/* チェックサムらしき値を計算してセットしないとスリープに入れない模様? */
	unsigned char checksum = 0xFF - packet->rx_sts - packet->rx_len - packet->rx_response;
	unsigned char offset  = packet->rx_len - 2 - 1 /* 末尾2バイトの0xFFは計算しない */;
	
	while( offset-- ) checksum -= packet->rx_data[offset];
	
	return checksum;
}

/*-----------------------------------------------
	Sysconハンドラ
-----------------------------------------------*/
static void mf_syscon_handler_handler1( SceSysconPacket *packet )
{
	/* 他からのデバッグハンドラが登録されていれば、それを呼ぶ */
	if( st_syscon_chain_handlers && st_syscon_chain_handlers->handler1 ) ( st_syscon_chain_handlers->handler1 )( packet );
}

static void mf_syscon_handler_transmit( SceSysconPacket *packet )
{
	/* 他からのデバッグハンドラが登録されていれば、それを呼ぶ */
	if( st_syscon_chain_handlers && st_syscon_chain_handlers->before_tx ) ( st_syscon_chain_handlers->before_tx )( packet );
}

/*
	デバッグハンドラのスタックサイズはどうなっておるのか?
	今のところ何も考えずに関数をコールしているが問題無い模様。
*/
static void mf_syscon_handler_recieve( SceSysconPacket *packet )
{
	//覚え書: syscon_pad.TimeStamp = mf_syscom_peek_dword( packet->unk38 ) (?);
	
	SceCtrlData syscon_pad;
	MfHprmKey   syscon_hk;
	
	/* Sysconパケットがコントローラデータ取得かどうか調べる */
	if(
		( packet->tx_cmd != SCE_SYSCON_CMD_GET_CONTROLLER_DIGITAL && packet->tx_cmd != SCE_SYSCON_CMD_GET_CONTROLLER_ANALOG ) ||
		packet->tx_cmd != packet->rx_response
	) goto skip;
	
	syscon_pad.Buttons = mf_conv_buttons_syscon_to_psp( mf_syscon_peek_dword( packet->rx_data ) );
	if( packet->rx_response == SCE_SYSCON_CMD_GET_CONTROLLER_ANALOG ){
		syscon_pad.Lx = packet->rx_data[4];
		syscon_pad.Ly = packet->rx_data[5];
	} else{
		syscon_pad.Lx = PADUTIL_CENTER_X;
		syscon_pad.Ly = PADUTIL_CENTER_Y;
	}
	
	sceHprmPeekCurrentKey( &syscon_hk );
	
	st_raw_pad = syscon_pad;
	st_raw_hk  = syscon_hk;
	
	if( st_hooked ){
		/*
			mfAnalogStickAdjustが内部でコールするpadutilAdjustAnalogStickで
			float型の演算をするとなぜか落ちる。
		*/
		mf_call_functions( MF_UPDATE, &syscon_pad, &syscon_hk );
		
		mf_syscon_poke_dword( packet->rx_data, (mf_conv_buttons_psp_to_syscon( syscon_pad.Buttons ) & ~SCE_SYSCON_CTRL_UNK_BUTTONS ) | ( mf_syscon_peek_dword( packet->rx_data ) & SCE_SYSCON_CTRL_UNK_BUTTONS ) );
		if( packet->rx_response == SCE_SYSCON_CMD_GET_CONTROLLER_ANALOG ){
			packet->rx_data[4] = syscon_pad.Lx;
			packet->rx_data[5] = syscon_pad.Ly;
			packet->rx_data[6] = mf_syscon_make_rx_checksum( packet );
		} else{
			packet->rx_data[4] = mf_syscon_make_rx_checksum( packet );
		}
	}

skip:
	/* 他からのデバッグハンドラが登録されていれば、それを呼ぶ */
	if( st_syscon_chain_handlers && st_syscon_chain_handlers->after_rx ) ( st_syscon_chain_handlers->after_rx )( packet );
}

static void mf_syscon_handler_handler4( SceSysconPacket *packet )
{
	/* 他からのデバッグハンドラが登録されていれば、それを呼ぶ */
	if( st_syscon_chain_handlers && st_syscon_chain_handlers->handler4 ) ( st_syscon_chain_handlers->handler4 )( packet );
}

#ifdef DEBUG
static void mf_syscom_dump_packet( SceSysconPacket *packet ){
	/* 
		呼ぶ前にどこかで
			pbInit();
			pbSetDisplayBuffer( PSP_DISPLAY_PIXEL_FORMAT_8888, (void*)0x44000000, 512 );
			pbApply();
		をしておく
	*/
	
	unsigned int y = 1;
	pbPrintf( 10, pbOffsetLine( y++ ), 0xffffffff, 0xff000000, "?1: unk00  = %02X, %02X, %02X, %02X", packet->unk00[0], packet->unk00[1], packet->unk00[2], packet->unk00[3] );
	pbPrintf( 10, pbOffsetLine( y++ ), 0xffffffff, 0xff000000, "?1: unk04  = %02X, %02X", packet->unk04[0], packet->unk04[1] );
	pbPrintf( 10, pbOffsetLine( y++ ), 0xffffffff, 0xff000000, "?1: status = %02X", packet->status );
	pbPrintf( 10, pbOffsetLine( y++ ), 0xffffffff, 0xff000000, "?1: unk07  = %02X", packet->unk07 );
	pbPrintf( 10, pbOffsetLine( y++ ), 0xffffffff, 0xff000000, "?1: unk08  = %02X, %02X, %02X, %02X", packet->unk08[0], packet->unk08[1], packet->unk08[2], packet->unk08[3] );

	pbPrintf( 10, pbOffsetLine( y++ ), 0xffffffff, 0xff000000, "T : CMD  = %02X", packet->tx_cmd );
	pbPrintf( 10, pbOffsetLine( y++ ), 0xffffffff, 0xff000000, "T : LEN  = %02X", packet->tx_len );
	pbPrintf( 10, pbOffsetLine( y++ ), 0xffffffff, 0xff000000, "T : DATA = %02X, %02X, %02X, %02X, %02X, %02X, %02X, %02X, %02X, %02X, %02X, %02X, %02X, %02X", packet->tx_data[0], packet->tx_data[1], packet->tx_data[2], packet->tx_data[3], packet->tx_data[4], packet->tx_data[5], packet->tx_data[6], packet->tx_data[7], packet->tx_data[8], packet->tx_data[9], packet->tx_data[10], packet->tx_data[11], packet->tx_data[12], packet->tx_data[13] );

	pbPrintf( 10, pbOffsetLine( y++ ), 0xffffffff, 0xff000000, "R : STS  = %02X", packet->rx_sts );
	pbPrintf( 10, pbOffsetLine( y++ ), 0xffffffff, 0xff000000, "R : LEN  = %02X", packet->rx_len );
	pbPrintf( 10, pbOffsetLine( y++ ), 0xffffffff, 0xff000000, "R : RESPONSE = %02X", packet->rx_response );
	pbPrintf( 10, pbOffsetLine( y++ ), 0xffffffff, 0xff000000, "R : DATA = %02X, %02X, %02X, %02X, %02X, %02X, %02X, %02X, %02X", packet->rx_data[0], packet->rx_data[1], packet->rx_data[2], packet->rx_data[3], packet->rx_data[4], packet->rx_data[5], packet->rx_data[6], packet->rx_data[7], packet->rx_data[8] );

	pbPrintf( 10, pbOffsetLine( y++ ), 0xffffffff, 0xff000000, "?2: UNK28 = %08X", packet->unk28 );

	pbPrintf( 10, pbOffsetLine( y++ ), 0xffffffff, 0xff000000, "CB: ADDR = %08X", packet->callback );
	pbPrintf( 10, pbOffsetLine( y++ ), 0xffffffff, 0xff000000, "CB: R28  = %08X", packet->callback_r28 );
	pbPrintf( 10, pbOffsetLine( y++ ), 0xffffffff, 0xff000000, "CB: ARG2 = %08X", packet->callback_arg2 );

	pbPrintf( 10, pbOffsetLine( y++ ), 0xffffffff, 0xff000000, "?3: unk38 = %02X, %02X, %02X, %02X, %02X, %02X, %02X, %02X, %02X, %02X, %02X, %02X, %02X", packet->unk38[0], packet->unk38[1], packet->unk38[2], packet->unk38[3], packet->unk38[4], packet->unk38[5], packet->unk38[6], packet->unk38[7], packet->unk38[8], packet->unk38[9], packet->unk38[10], packet->unk38[11], packet->unk38[12] );
	pbPrintf( 10, pbOffsetLine( y++ ), 0xffffffff, 0xff000000, "?3: OLDSTS  = %02X", packet->old_sts );
	pbPrintf( 10, pbOffsetLine( y++ ), 0xffffffff, 0xff000000, "?3: CURSTS  = %02X", packet->cur_sts );
	pbPrintf( 10, pbOffsetLine( y++ ), 0xffffffff, 0xff000000, "?3: UNK47 =" );
	pbPrintf( 20, pbOffsetLine( y++ ), 0xffffffff, 0xff000000, "%02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X",
		packet->unk47[0],
		packet->unk47[1],
		packet->unk47[2],
		packet->unk47[3],
		packet->unk47[4],
		packet->unk47[5],
		packet->unk47[6],
		packet->unk47[7],
		packet->unk47[8],
		packet->unk47[9],
		packet->unk47[10],
		packet->unk47[11],
		packet->unk47[12],
		packet->unk47[13],
		packet->unk47[14],
		packet->unk47[15]
	);
	pbPrintf( 20, pbOffsetLine( y++ ), 0xffffffff, 0xff000000, "%02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X",
		packet->unk47[16],
		packet->unk47[17],
		packet->unk47[18],
		packet->unk47[19],
		packet->unk47[20],
		packet->unk47[21],
		packet->unk47[22],
		packet->unk47[23],
		packet->unk47[24],
		packet->unk47[25],
		packet->unk47[26],
		packet->unk47[27],
		packet->unk47[28],
		packet->unk47[29],
		packet->unk47[30],
		packet->unk47[31]
	);
	pbPrintf( 20, pbOffsetLine( y++ ), 0xffffffff, 0xff000000, "%02X", packet->unk47[32] );
}
#endif

/*-----------------------------------------------
	SysconボタンとPspボタンのコードの変換
-----------------------------------------------*/
static unsigned int mf_conv_buttons_syscon_to_psp( unsigned int sysconbuttons )
{
	unsigned int buttons = 0;
	
	/* ビット反転 */
	sysconbuttons = ~sysconbuttons;
	
	/* EJECT系のフラグを反転させる */
	sysconbuttons ^= SCE_SYSCON_CTRL_REMOTE_EJECT | SCE_SYSCON_CTRL_DISC_EJECT | SCE_SYSCON_CTRL_MS_EJECT;
	
	/* ボタンを変換 */
	buttons |= ( sysconbuttons & ( SCE_SYSCON_CTRL_SELECT   | SCE_SYSCON_CTRL_START ) ) >> 8;
	buttons |= ( sysconbuttons & ( SCE_SYSCON_CTRL_LTRIGGER | SCE_SYSCON_CTRL_RTRIGGER ) ) >> 1;
	buttons |= ( sysconbuttons & ( SCE_SYSCON_CTRL_TRIANGLE | SCE_SYSCON_CTRL_CIRCLE | SCE_SYSCON_CTRL_CROSS | SCE_SYSCON_CTRL_SQUARE ) ) << 8;
	buttons |= ( sysconbuttons & (
		SCE_SYSCON_CTRL_UP           |
		SCE_SYSCON_CTRL_DOWN         |
		SCE_SYSCON_CTRL_RIGHT        |
		SCE_SYSCON_CTRL_LEFT         |
		SCE_SYSCON_CTRL_HOME         |
		SCE_SYSCON_CTRL_HOLD         |
		SCE_SYSCON_CTRL_WLAN_UP      |
		SCE_SYSCON_CTRL_VOLUP        |
		SCE_SYSCON_CTRL_VOLDOWN      |
		SCE_SYSCON_CTRL_SCREEN       |
		SCE_SYSCON_CTRL_NOTE         |
		SCE_SYSCON_CTRL_REMOTE_EJECT |
		SCE_SYSCON_CTRL_DISC_EJECT   |
		SCE_SYSCON_CTRL_MS_EJECT
	) ) << 4;
	
	return buttons;
}

static unsigned int mf_conv_buttons_psp_to_syscon( unsigned int buttons )
{
	unsigned int sysconbuttons = 0;
	
	/* EJECT系のフラグを反転させる */
	buttons ^= PSP_CTRL_REMOTE | PSP_CTRL_DISC | PSP_CTRL_MS;
	
	/* ボタンを変換 */
	sysconbuttons |= ( buttons & ( PSP_CTRL_SELECT   | PSP_CTRL_START ) ) << 8;
	sysconbuttons |= ( buttons & ( PSP_CTRL_LTRIGGER | PSP_CTRL_RTRIGGER ) ) << 1;
	sysconbuttons |= ( buttons & ( PSP_CTRL_TRIANGLE | PSP_CTRL_CIRCLE | PSP_CTRL_CROSS | PSP_CTRL_SQUARE ) ) >> 8;
	sysconbuttons |= ( buttons & (
		PSP_CTRL_UP      |
		PSP_CTRL_DOWN    |
		PSP_CTRL_RIGHT   |
		PSP_CTRL_LEFT    |
		PSP_CTRL_HOME    |
		PSP_CTRL_HOLD    |
		PSP_CTRL_WLAN_UP |
		PSP_CTRL_VOLUP   |
		PSP_CTRL_VOLDOWN |
		PSP_CTRL_SCREEN  |
		PSP_CTRL_NOTE    |
		PSP_CTRL_REMOTE  |
		PSP_CTRL_DISC    |
		PSP_CTRL_MS
	) ) >> 4;
	
	/* ビット反転 */
	return ~sysconbuttons;
}

/*-----------------------------------------------
	メインスレッド開始時に最初に呼ばれる。
-----------------------------------------------*/
static void mf_ready( const char *cwd )
{
	int i;
	MfFuncInit initfunc;
	
	/* ファンクションの初期化 */
	dbgprint( "Sending message MF_MS_INIT to all functions..." );
	
	mfAnalogStickInit();
	for( i = 0; i < gMftabEntryCount; i++ ){
		if( gMftab[i].proc ){
			initfunc = ( gMftab[i].proc )( MF_MS_INIT );
			if( initfunc ) initfunc();
		}
	}
	
	/* 起動待ち (なんとかなくせないものか?) */
	dbgprint( "Waiting for loading modules..." );
	
	if( st_world == MF_WORLD_VSH ){
		while( sceKernelFindModuleByName( "sceVshBridge_Driver" ) == NULL ){
			sceKernelDelayThread( 2000000 );
		}
	} else if( st_world == MF_WORLD_POPS ){
		while( sceKernelFindModuleByName( "scePops_Manager" ) == NULL && sceKernelFindModuleByName( "popsloader_trademark" ) == NULL ){
			sceKernelDelayThread( 2000000 );
		}
	} else{
		int stat;
		
		sceKernelDelayThread( 12000000 );
		
		stat = sceUmdGetDriveStat();
		if( stat > PSP_UMD_NOT_PRESENT ){
			SceUID sfo;
			if( stat != PSP_UMD_READY ) sceUmdWaitDriveStat( PSP_UMD_READY );
			
			sfo = sceIoOpen( "disc0:/PSP_GAME/PARAM.SFO", PSP_O_RDONLY, 0777 ); 
			if( CG_IS_VALID_UID( sfo ) ){
				mf_get_gameid( sfo, st_game_id );
				sceIoClose( sfo );
			}
		}
	}
	
	dbgprint( "Checking ini..." );
	mf_ini( cwd );
}

/*-----------------------------------------------
	mf_ready()終了後、フックアドレスを取得。
-----------------------------------------------*/
static void mf_find_hook_address( void )
{
	/* フックアドレスを取得 */
	unsigned int i;
	void *syscall;
	
	dbgprint( "Find hook address..." );
	for( i = 0; i < hooktableEntryCount; i++ ){
		dbgprintf( "Module: %s, Library: %s, NID: 0x%X", hooktable[i].module, hooktable[i].library, hooktable[i].nid );
		hooktable[i].restore.addr = hookFindExportAddr( hooktable[i].module, hooktable[i].library, hooktable[i].nid );
		if( ( syscall = hookFindSyscallAddr( hooktable[i].restore.addr ) ) ) hooktable[i].restore.addr = syscall;
		
		dbgprintf( "\tNID:0x%08x Addr:%p Orig:%p Hook:%p", hooktable[i].nid, hooktable[i].restore.addr, hooktable[i].restore.addr ? *(hooktable[i].restore.addr): NULL, hooktable[i].hook );
		if( hooktable[i].restore.addr ) hooktable[i].restore.api = *(hooktable[i].restore.addr);
	}
}

/*-----------------------------------------------
	INIの読み込みと保存
-----------------------------------------------*/
static void mf_ini( const char *path )
{
	char inipath[MF_PATH_MAX];
	InimgrUID ini;
	
	/* 設定ファイルのパスを決定 */
	{
		char *delim;
		
		strutilCopy(
			inipath,
			( ( ! path || path[0] == '\0' || strncasecmp( path, "flash", 5 ) == 0 || ! strrchr( path, '/' ) ) ? MF_INI_PATH_DEFAULT : path ),
			sizeof( inipath ) - strlen( MF_INI_FILENAME )
		);
		delim = strrchr( inipath, '/' );
		strcpy( delim + 1, MF_INI_FILENAME );
	}
	
	ini = inimgrNew();
	if( ini < CG_OK ){
		dbgprintf( "Failed: %X", ini );
	} else{
		if( mfConvertButtonReady() ){
			dbgprintf( "Open %s ...", inipath );
			if( inimgrLoad( ini, inipath, NULL, 0 ) == CG_OK ){
				dbgprint( "LOAD mode" );
				mf_ini_load( ini );
			} else{
				dbgprint( "SAVE mode" );
				mf_ini_save( ini );
				if( inimgrSave( ini, inipath, NULL, 0 ) != CG_OK ){
					strutilCopy( st_game_id, MF_INI_LOAD_FAILED, sizeof( st_game_id ) );
					dbgprint( "Failed to save" );
				}
			}
			mfConvertButtonFinish();
		}
		inimgrDestroy( ini );
	}
}

static void mf_ini_load( InimgrUID ini )
{
	int i;
	char buf[255];
	MfFuncIni inifunc;
	
	/* 必要なセクションをロード */
	for( i = 0; i < sizeof( st_ini_sections ) >> 2; i++ ){
		if( st_ini_sections[i] ) inimgrLoadSection( ini, st_ini_sections[i] );
	}
	
	/* 設定値をロード */
	mfIniGetBool( ini, "Startup", &st_is_enabled );
	if( mfIniGetString( ini, "MenuButtons", buf, sizeof( buf ) ) == CG_OK )   st_menu_buttons = mfConvertButtonN2C( buf );
	if( mfIniGetString( ini, "ToggleButtons", buf, sizeof( buf ) ) == CG_OK ) st_toggle_buttons = mfConvertButtonN2C( buf );
	mfIniGetBool( ini, "StatusNotification", &st_status_notification );
	
	mfAnalogStickIniLoad( ini, buf, sizeof( buf ) );
	mfMenuIniLoad( ini, buf, sizeof( buf ) );

	dbgprint( "Sending message MF_MS_INI_LOAD to all functions..." );
	for( i = 0; i < gMftabEntryCount; i++ ){
		if( gMftab[i].proc ){
			dbgprintf( "Find load-function and to run for %s", gMftab[i].name );
			inifunc = ( gMftab[i].proc )( MF_MS_INI_LOAD );
			if( inifunc ) inifunc( ini, buf, sizeof( buf ) );
		}
	}
}

static void mf_ini_save( InimgrUID ini )
{
	int i;
	char buf[255];
	MfFuncIni inifunc;
	
	/* デフォルトセクションを作成 */
	inimgrAddSection( ini, MF_INI_SECTION_DEFAULT );
	
	inimgrSetBool( ini, MF_INI_SECTION_DEFAULT, "Startup", st_is_enabled );
	
	mfConvertButtonC2N( st_menu_buttons, buf, sizeof( buf ) );
	inimgrSetString( ini, MF_INI_SECTION_DEFAULT, "MenuButtons", buf );
	
	mfConvertButtonC2N( st_toggle_buttons, buf, sizeof( buf ) );
	inimgrSetString( ini, MF_INI_SECTION_DEFAULT, "ToggleButtons", buf );
	
	inimgrSetBool( ini, MF_INI_SECTION_DEFAULT, "StatusNotification", MF_INI_STATUS_NOTIFICATION );
	
	mfAnalogStickIniSave( ini, buf, sizeof( buf ) );
	mfMenuIniSave( ini, buf, sizeof( buf ) );
	
	dbgprint( "Sending message MF_MS_INI_CREATE to all functions..." );
	for( i = 0; i < gMftabEntryCount; i++ ){
		if( gMftab[i].proc ){
			inifunc = ( gMftab[i].proc )( MF_MS_INI_CREATE );
			if( inifunc ) inifunc( ini, buf, sizeof( buf ) );
		}
	}
}

static int mf_get_current_ctrl( SceCtrlData *pad, MfHprmKey *hk )
{
	if( st_syscon_chain_handlers || st_hooked ){
		unsigned int intc = pspSdkDisableInterrupts();
		*pad = st_raw_pad;
		*hk  = st_raw_hk;
		pspSdkEnableInterrupts( intc );
	} else{
		sceCtrlPeekBufferPositive( pad, 1 );
		sceHprmPeekCurrentKey( hk );
	}
	return 0;
}

static void mf_call_functions( MfHookAction action, SceCtrlData *pad, MfHprmKey *hk )
{
	MfFuncHook hookfunc;
	int i;
	
	/* アナログスティック調整 */
	mfAnalogStickAdjust( pad );
	
	for( i = 0; i < gMftabEntryCount; i++ ){
		if( gMftab[i].proc ){
			hookfunc = ( gMftab[i].proc )( MF_MS_HOOK );
			if( hookfunc ) hookfunc( action, pad, hk );
		}
	}
}

/*-----------------------------------------------
	MF_MS_TOGGLEメッセージを全てのファンクションに送る
-----------------------------------------------*/
static void mf_sending_toggle_message( bool engine )
{
	int i;
	MfFuncToggle togglefunc;
	
	if( mfNotificationThreadId() ){
		mfNotificationPrintf( "%s: %s", MF_STR_HOME_MACROFIRE_ENGINE, engine ? MF_STR_CTRL_ON : MF_STR_CTRL_OFF );
	}
	
	dbgprint( "Sending message MF_MS_TOGGLE to all functions..." );
	for( i = 0; i < gMftabEntryCount; i++ ){
		if( gMftab[i].proc ){
			togglefunc = ( gMftab[i].proc )( MF_MS_TOGGLE );
			if( togglefunc ) togglefunc( engine );
		}
	}
}

/*-----------------------------------------------
	MacroFire メインループ
-----------------------------------------------*/
static int mf_main( SceSize arglen, void *argp )
{
	bool wait_toggle_buttons_release = false;
	PadutilButtons buttons;
	
	mf_ready( argp );

	mf_find_hook_address();
	
	if( st_status_notification ) mfNotificationStart();
	
	dbgprint( "Starting main-loop" );
	while( gRunning ){
		sceKernelDelayThreadCB( 50000 );
		
#ifdef DEBUG
	if( gDebug[0] ){
		dbgprintf( "0:%X 1:%X 2:%X 3:%X 4:%X 5:%X 6:%X 7:%X 8:%X 9:%X",
			gDebug[0],
			gDebug[1],
			gDebug[2],
			gDebug[3],
			gDebug[4],
			gDebug[5],
			gDebug[6],
			gDebug[7],
			gDebug[8],
			gDebug[9]
		);
	}
#endif
		
		mf_get_current_ctrl( &st_pad, &st_hk );
		st_pad.Buttons |= padutilGetAnalogStickDirection( st_pad.Lx, st_pad.Ly, 0 );
		
		buttons = padutilSetPad( st_pad.Buttons ) | padutilSetHprm( st_hk );
		
		if( st_toggle_buttons ){
			if( ( buttons & st_toggle_buttons ) == st_toggle_buttons ){
				if( ! wait_toggle_buttons_release ){
					st_is_enabled = ( st_is_enabled ? false : true );
					wait_toggle_buttons_release = true;
					dbgprintf( "Changing engine state: %d", st_is_enabled );
				}
			} else if( wait_toggle_buttons_release ){
				wait_toggle_buttons_release = false;
			}
		}
		
		if( ! st_hooked && st_is_enabled ){
			mfHook();
			mf_sending_toggle_message( st_is_enabled );
		} else if( st_hooked && ! st_is_enabled ){
			mfUnhook();
			mf_sending_toggle_message( st_is_enabled );
		}
		
		if( ( buttons & st_menu_buttons ) == st_menu_buttons ){
			dbgprint( "Launching menu" );
			mfMenuMain( &st_pad, &st_hk );
		}
	}
	/* module_stop()が呼ばれないので、以下は実行されていない */
	
	dbgprint( "Shutting down..." );
	mf_shutdown();
	
	dbgprint( "Exit" );
	return sceKernelExitDeleteThread( 0 );
}

/*-----------------------------------------------
	APIのフック
	
	既にフックされている状態でもう一度呼んでもなにもしない。
-----------------------------------------------*/
void mfHook( void )
{
	int i;
	
	if( st_hooked ) return;
	
	dbgprint( "Hooking API" );
	
	for( i = 0; i < hooktableEntryCount; i++ ){
		dbgprintf( "Hooking %p -> %p", hooktable[i].restore.addr, hooktable[i].hook );
		hookFunc( hooktable[i].restore.addr, hooktable[i].hook );
	}
	
	dbgprint( "Set syscon debug handlers" );
	if( ! st_syscon_chain_handlers ) sceSysconSetDebugHandlers( &st_syscon_handlers );
	
	st_hooked = true;
}

/*-----------------------------------------------
	フックの解除
	
	まだフックされていない状態で呼んでも何もしない。
-----------------------------------------------*/
void mfUnhook( void )
{
	int i;
	
	if( ! st_hooked ) return;
	
	dbgprint( "Unhooking API" );
	
	dbgprint( "Clear syscon debug handlers" );
	if( ! st_syscon_chain_handlers ) sceSysconSetDebugHandlers( NULL );
	
	for( i = 0; i < hooktableEntryCount; i++ ){
		dbgprintf( "Unhooking %p -> %p", hooktable[i].hook, hooktable[i].restore.addr );
		hookFunc( hooktable[i].restore.addr, hooktable[i].restore.api );
	}
	
	st_hooked = false;
}

/*-----------------------------------------------
	フック関数
-----------------------------------------------*/
int mfHprmPeekCurrentKey( u32 *key )
{
	SceCtrlData pad;
	
	int ret = mf_get_current_ctrl( &pad, key );
	mf_call_functions( MF_UPDATE, &pad, key );
	
	return ret;
}

int mfHprmPeekLatch( u32 *latch )
{
	SceCtrlData pad;
	MfHprmKey hk;
	int ret;
	
	ret = mf_get_current_ctrl( &pad, &hk );
	mf_call_functions( MF_KEEP, &pad, &hk );
	
	*latch |= ( st_prev_hprmkey ^ hk ) & hk;
	st_prev_hprmkey = hk;
	
	return ret;
}

int mfHprmReadLatch( u32 *latch )
{
	/* mfHprmPeekLatch()との違いがよく分からない */
	return mfHprmPeekLatch( latch );
}

void mfSysconSetDebugHandlers( SceSysconDebugHandlers *handlers )
{
	/* 他からのデバッグハンドラを記録 */
	st_syscon_chain_handlers = handlers;
	
	if( handlers ){
		/* ハンドラがあれば、それを呼び出す自分のデバッグハンドラを登録 */
		sceSysconSetDebugHandlers( &st_syscon_handlers );
	} else if( ! st_hooked ){
		sceSysconSetDebugHandlers( NULL );
	}
	return;
}

/*-----------------------------------------------
	MacroFireAPI
-----------------------------------------------*/
void mfEnable( void )
{
	st_is_enabled = true;
}

void mfDisable( void )
{
	st_is_enabled = false;
}

bool mfIsEnabled( void )
{
	return st_is_enabled;
}

void mfSetMenuButtons( PadutilButtons buttons )
{
	st_menu_buttons = buttons;
}

void mfSetToggleButtons( PadutilButtons buttons )
{
	st_toggle_buttons = buttons;
}

PadutilButtons mfGetMenuButtons( void )
{
	return st_menu_buttons;
}

PadutilButtons mfGetToggleButtons( void )
{
	return st_toggle_buttons;
}

bool mfConvertButtonReady( void )
{
	if( ! st_cvbtn ){
		st_cvbtn = padutilCreateButtonNames();
		if( ! st_cvbtn ) return false;
	}
	return true;
}

void mfConvertButtonFinish( void )
{
	if( st_cvbtn ){
		padutilDestroyButtonNames();
		st_cvbtn = NULL;
	}
}

char *mfConvertButtonC2N( PadutilButtons buttons, char *buf, size_t len )
{
	padutilGetButtonNamesByCode( st_cvbtn, buttons, "+", PADUTIL_OPT_TOKEN_SP, buf, len );
	return buf;
}

PadutilButtons mfConvertButtonN2C( char *buttons )
{
	PadutilButtons btncode;
	btncode = padutilGetButtonCodeByNames( st_cvbtn, buttons, "+", PADUTIL_OPT_IGNORE_SP );
	return btncode;
}

const char *mfGetGameId()
{
	return st_game_id;
}

unsigned int mfGetWorld( void )
{
	return st_world;
}

bool mfIsRunningApp( MfAppId app )
{
	switch( app ){
		case MF_APP_WEBBROWSER:
			if( sceKernelFindModuleByName( "sceHVNetfront_Module" ) != NULL ) return true;
			break;
		default:
			break;
	}
	return false;
}

int mfNotificationStart( void )
{
	st_ovmsg = ovmsgInit();
	if( st_ovmsg < 0 ) return st_ovmsg;
	
	return ovmsgStart( st_ovmsg, "MacroFireNotifier", 16 );
}

void mfNotificationShutdownStart( void )
{
	ovmsgShutdown( st_ovmsg );
	ovmsgDestroy( st_ovmsg );
	st_ovmsg = 0;
}

SceUID mfNotificationThreadId( void )
{
	return st_ovmsg ? ovmsgGetThreadId( st_ovmsg ) : 0;
}

void mfNotificationPrintTerm( void )
{
	ovmsgSuspend( st_ovmsg );
}

bool mfNotificationPrintf( const char *format, ... )
{
	va_list ap;
	bool ret;
	
	va_start( ap, format );
	ret = ovmsgVprintf( st_ovmsg, format, ap );
	va_end( ap );
	
	return ret;
}

int mfIniGetInt( InimgrUID ini, const char *key, int *data )
{
	int i;
	for( i = 0; i < sizeof( st_ini_sections ) >> 2; i++ ){
		if( st_ini_sections[i] && inimgrGetInt( ini, st_ini_sections[i], key, data ) == CG_OK ) return CG_OK;
	}
	return -1;
}

int mfIniGetString( InimgrUID ini, const char *key, char *buf, size_t buflen )
{
	int i;
	for( i = 0; i < sizeof( st_ini_sections ) >> 2; i++ ){
		if( st_ini_sections[i] && inimgrGetString( ini, st_ini_sections[i], key, buf, buflen ) > 0 ){
			return CG_OK;
		}
	}
	return -1;
}

int mfIniGetBool( InimgrUID ini, const char *key, bool *bl )
{
	int i;
	for( i = 0; i < sizeof( st_ini_sections ) >> 2; i++ ){
		if( st_ini_sections[i] && inimgrGetBool( ini, st_ini_sections[i], key, bl ) == CG_OK ) return CG_OK;
	}
	return -1;
}

/*-----------------------------------------------
	エントリポイント
-----------------------------------------------*/
#ifdef MF_WITH_EXCEPTION_HANDLER
static const unsigned char regName[32][5] =
{
    "zr", "at", "v0", "v1", "a0", "a1", "a2", "a3",
    "t0", "t1", "t2", "t3", "t4", "t5", "t6", "t7", 
    "s0", "s1", "s2", "s3", "s4", "s5", "s6", "s7",
    "t8", "t9", "k0", "k1", "gp", "sp", "fp", "ra"
};

static const char *codeTxt[32] =
{
	"Interrupt", "TLB modification", "TLB load/inst fetch", "TLB store",
	"Address load/inst fetch", "Address store", "Bus error (instr)",
	"Bus error (data)", "Syscall", "Breakpoint", "Reserved instruction",
	"Coprocessor unusable", "Arithmetic overflow", "Unknown 13", "Unknown 14",
	"FPU Exception", "Unknown 16", "Unknown 17", "Unknown 18",
	"Unknown 20", "Unknown 21", "Unknown 22", "Unknown 23",
	"Unknown 24", "Unknown 25", "Unknown 26", "Unknown 27",
	"Unknown 28", "Unknown 29", "Unknown 30", "Unknown 31"
};

void mf_exception( PspDebugRegBlock *regs )
{
	SceUID fd;
	int i;
	
	fd = sceIoOpen( "ms0:/macrofire_error.txt", PSP_O_WRONLY | PSP_O_CREAT | PSP_O_TRUNC, 0777 );
	if( fd ){
		char buf[256];
		SceModule *mod;
		SceKernelModuleInfo modinfo;
		unsigned int baseaddr = 0;
		
		mod = sceKernelFindModuleByAddress( regs->epc );
		if( mod )
		{
			memset( &modinfo, 0, sizeof( SceKernelModuleInfo ) );
			modinfo.size = sizeof( SceKernelModuleInfo );
			
			if( sceKernelQueryModuleInfo( mod->modid, &modinfo ) == 0 ){
				baseaddr = modinfo.text_addr;
			}
		}
		
		snprintf( buf, sizeof( buf ), "MacroFire Version %s\n", MF_VERSION );
		sceIoWrite( fd, buf, strlen( buf ) );
		snprintf( buf, sizeof( buf ), "Exception - %s\n", codeTxt[(regs->cause >> 2) & 31] );
		sceIoWrite( fd, buf, strlen( buf ) );
		snprintf( buf, sizeof( buf ), "EPC       - %08X\n", regs->epc );
		sceIoWrite( fd, buf, strlen( buf ) );
		snprintf( buf, sizeof( buf ), "Cause     - %08X\n", regs->cause );
		sceIoWrite( fd, buf, strlen( buf ) );
		snprintf( buf, sizeof( buf ), "Status    - %08X\n", regs->status );
		sceIoWrite( fd, buf, strlen( buf ) );
		snprintf( buf, sizeof( buf ), "BadVAddr  - %08X\n", regs->badvaddr );
		sceIoWrite( fd, buf, strlen( buf ) );
		snprintf( buf, sizeof( buf ), "BaseAddr  - %08X\n", baseaddr );
		sceIoWrite( fd, buf, strlen( buf ) );
		for(i = 0; i < 32; i+=4)
		{
			snprintf( buf, sizeof( buf ), "%s:%08X %s:%08X %s:%08X %s:%08X\n",
				regName[i],
				regs->r[i],
				regName[i+1],
				regs->r[i+1],
				regName[i+2],
				regs->r[i+2],
				regName[i+3],
				regs->r[i+3]
			);
			sceIoWrite( fd, buf, strlen( buf ) );
		}
		sceIoClose( fd );
	}
}
#endif

int module_start( SceSize arglen, void *argp )
{
	SceUID thid;
	dbgprintf( "MacroFire %s - Build %s", MF_VERSION, __DATE__ );
	dbgprint( "pen@classg (http://classg.sytes.net)" );
	dbgprint( "------------------------------------" );
	
#ifdef MF_WITH_EXCEPTION_HANDLER
	pspDebugInstallErrorHandler( mf_exception );
#endif
	
	dbgprint( "Initializing" );
	mf_init();
	
	/*
		MacroFireが起動される前に設定されているデバッグハンドラを外す。
		vsh.txt/game.txt/pops.txtで後から起動されたプラグインの機能が生きた方がわかりやすいでしょう?
	*/
	sceSysconSetDebugHandlers( NULL );
	
	/*
		APIの元のアドレスも記録して、戻せるようにしておく。
		module_stop()は呼ばれていないので、戻せるようにする必要は無いと思われるが、
		今後どうなるかわからないので、一応module_stop()が呼ばれると仮定にする。
	*/
	st_syscon_hook.restore.addr = hookFindExportAddr( st_syscon_hook.module, st_syscon_hook.library, st_syscon_hook.nid );
	if( ! st_syscon_hook.restore.addr ) return 0;
	
	/*
		sceSysconSetDebugHandlersをフックして、他からのデバッグハンドラ登録を監視する。
		
		mfSysconSetDebugHandlersは、他からデバッグハンドラの登録があれば、
		そのハンドラを記録して、MacroFireのデバッグハンドラを登録する。
		MacroFireのデバッグハンドラは、記録された他からのデバッグハンドラを呼び出す。
		
		他からのデバッグハンドラを覚えておける数は1つだけ。
		2つ以上覚えておくようにもできるが、解除に問題がある。
		このAPIは、解除に必要なステップが、引数がNULLを渡すだけで、どのデバッグハンドラを解除しようとしているのかを特定できない。
	*/
	
	st_syscon_hook.restore.api = *(st_syscon_hook.restore.addr);
	hookFunc( st_syscon_hook.restore.addr, mfSysconSetDebugHandlers );
	
	thid = sceKernelCreateThread( "MacroFire", mf_main, 32, 0x900, 0, 0 );
	if( thid > 0 ){
		mfMenuAddSysThreadId( thid );
		sceKernelStartThread( thid, arglen, argp );
	}
	
#ifdef MF_LOW_MEMORY_TEST
	{
		unsigned int allocsize = sceKernelMaxFreeMemSize() - MF_LOW_MEMORY_TEST;
		SceUID muid = sceKernelAllocPartitionMemory( 2, "DummyBlock", PSP_SMEM_Low, allocsize, NULL );
		dbgprintf( "Allocating dummy block: %d bytes: %X", allocsize, muid );
		dbgprintf( "Free space: %d", sceKernelMaxFreeMemSize() );
	}
#endif
	
	return 0;
}

int module_stop( SceSize arglen, void *argp )
{
	gRunning = false;
	
	return 0;
}
