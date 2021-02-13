/* mfMenuMain.c */

#include "mfmenu_types.h"

/*-----------------------------------------------
	定数
-----------------------------------------------*/
#define MF_MENU_STACK_SIZE( x ) ( sizeof( struct MfMenuStackData ) * ( x ) )
#define MF_MENU_STACK_NUM       4

#define MF_MENU_SUSPEND_THREADS sceKernelSuspendThread
#define MF_MENU_RESUME_THREADS  sceKernelResumeThread

#define MF_MENU_MUTE_ENABLE  sceSysregAudioIoDisable
#define MF_MENU_MUTE_DISABLE sceSysregAudioIoEnable

#define MF_MENU_FB_PIXELFORMAT PB_PXF_5650
#define MF_MENU_FB_WIDTH       512

#define MF_MENU_FB_DISP  0
#define MF_MENU_FB_DRAW  1
#define MF_MENU_FB_CLEAR 2

/*-----------------------------------------------
	マクロ
-----------------------------------------------*/

/*-----------------------------------------------
	ローカル関数プロトタイプ
-----------------------------------------------*/
static bool mfmenu_init_params( struct MfMenuParams *params, SceCtrlData *pad, MfHprmKey *hk );
static bool mfmenu_init_graphics( void );
static void mfmenu_mute( int ( *audio )( int ) );
static void mfmenu_indicator( void );
static bool mfmenu_stack( struct MfMenuStack *stack, enum MfMenuStackAction ctrl, void *menu, struct MfMenuPosition *pos );
static bool mfmenu_get_target_threads( SceUID *thread_id_list, int *thread_id_count );
static void mfmenu_change_threads_stat( int ( *func )( SceUID ), SceUID *thread_id_list, int thread_id_count );
static bool mfmenu_alloc_buffers( struct MfMenuFrameBuffer *fb, bool graphics_mem );
static void mfmenu_free_buffers( struct MfMenuFrameBuffer *fb );
static void mfmenu_draw_frame( void );
static int  mfmenu_root_menu( MfMenuMessage message );
static int  mfmenu_control_engine( MfMessage message, const char *label, void *var, void *unused, void *ex );

/*-----------------------------------------------
	デバッグ用
-----------------------------------------------*/
#ifdef DEBUG_ENABLED
unsigned int __mfmenu__g_memalloc_count;
static void mfmenu_debug( void );
#endif

void mfMenuMain( SceCtrlData *pad, MfHprmKey *hk )
{
	{
		DmemUID dmem = dmemNew( 1024, MEMORY_USER, DMEM_HIGH );
		if( ! dmem ){
			dbgprint( "Not enough available memory for DMEM" );
			return;
		}
		
		/* ユーザメモリからメニュー用の作業域を確保 */
		__mfmenu__gParams = dmemAlloc( dmem, sizeof( struct MfMenuParams ) );
		dbgprintf( "Allocating memory for menu: %p: %d bytes", __mfmenu__gParams, sizeof( struct MfMenuParams ) );
		if( ! __mfmenu__gParams ){
			dbgprint( "Not enough available memory for menu" );
			return;
		}
		memset( __mfmenu__gParams, 0, sizeof( struct MfMenuParams ) );
		
		__mfmenu__gParams->MemoryUid = dmem;

#ifdef DEBUG_ENABLED
		__mfmenu__g_memalloc_count = 0;
#endif
	}
	
	/* サスペンド/リジュームするスレッドの一覧を作成し、サスペンドする */
	if( ! mfmenu_get_target_threads( __mfmenu__gParams->Thread.List, &(__mfmenu__gParams->Thread.Count) ) ) goto DESTROY;
	mfmenu_change_threads_stat( MF_MENU_SUSPEND_THREADS, __mfmenu__gParams->Thread.List, __mfmenu__gParams->Thread.Count );
	
	/* すぐに止まらないスレッドもある? */
	sceDisplayWaitVblankStart();
	
	/* サウンドをミュートにする */
	mfmenu_mute( MF_MENU_MUTE_ENABLE );
	
	/* APIがフックされている場合、一時的に元に戻す */
	if( mfIsEnabled() ) mfUnhook();
	
	/* paramsを初期化 */
	if( ! mfmenu_init_params( __mfmenu__gParams, pad, hk ) ) goto DESTROY;
	
	/* メニューフラグの初期化 */
	mfMenuEnable( MF_MENU_SCREEN_UPDATE | MF_MENU_EXIT );
	
	/* ルートメニュー設定 */
	__mfmenu__gParams->Proc = (MfFuncMenu)mfmenu_root_menu;
	( __mfmenu__gParams->Proc )( MF_MM_INIT );
	
	/* グラフィックを初期化 */
	if( ! mfmenu_init_graphics() ) goto DESTROY;
	
	dbgprint( "Starting menu-loop" );
	while( gRunning ){
		/* 背景を描画 */
		if( __mfmenu__gParams->FrameBuffer.Frame[MF_MENU_FB_CLEAR].Addr ){
			sceDmacMemcpy( pbGetCurrentDrawBufferAddr(), __mfmenu__gParams->FrameBuffer.Frame[MF_MENU_FB_CLEAR].Addr, __mfmenu__gParams->FrameBuffer.Size );
			mfmenu_indicator();
		} else if( __mfmenu__gParams->FrameBuffer.Frame[MF_MENU_FB_DRAW].Addr ){
			memset( pbGetCurrentDrawBufferAddr(), 0, __mfmenu__gParams->FrameBuffer.Size );
			mfmenu_draw_frame();
			mfmenu_indicator();
		} else{
			SceCtrlData dupe_pad = *(__mfmenu__gParams->Ctrl.Pad);
			mfAnalogStickAdjust( &dupe_pad );
			if( ( ( padutilSetPad( dupe_pad.Buttons | padutilGetAnalogStickDirection( dupe_pad.Lx,  dupe_pad.Ly, 0 ) )  ) & MF_HOTKEY_BUTTONS ) || ( __mfmenu__gParams->Flags & MF_MENU_SCREEN_UPDATE ) ){
				pbDisable( PB_NO_DRAW );
				mfMenuDisable( MF_MENU_SCREEN_UPDATE );
				memset( pbGetCurrentDrawBufferAddr(), 0, __mfmenu__gParams->FrameBuffer.Size );
				mfmenu_draw_frame();
				pbPrint( 0, 0, MF_COLOR_EX4, MF_COLOR_TEXT_BG, MF_STR_HOME_LOW_MEMORY_MODE );
				mfmenu_indicator();
			} else{
				pbEnable( PB_NO_DRAW );
			}
		}
		
		padctrlPeekBuffer( __mfmenu__gParams->Ctrl.Uid, __mfmenu__gParams->Ctrl.Pad, 1 );
		// キーリピートが必要 sceHprmPeekCurrentKey( __mfmenu__gParams->Ctrl.HprmKey );
		padutilRemap( __mfmenu__gPref.Remap, padutilSetPad( mfMenuGetCurrentButtons() ), __mfmenu__gParams->Ctrl.Pad, __mfmenu__gParams->Ctrl.HprmKey, true );
		
#ifdef DEBUG_ENABLED
		mfmenu_debug();
#endif
		
		/*
			MF_MENU_FORCED_QUITがセットされたか、
			MF_MENU_EXITが有効な状態でSTART/HOMEを押下したか、
			次のプロシージャが存在しなければメニューを抜ける。
		*/
		if(
			( __mfmenu__gParams->Flags & MF_MENU_FORCED_QUIT ) ||
			( ( __mfmenu__gParams->Flags & MF_MENU_EXIT ) && ( __mfmenu__gParams->Ctrl.Pad->Buttons & ( PSP_CTRL_START | PSP_CTRL_HOME ) ) ) ||
			! __mfmenu__gParams->Proc
		) break;
		
		/* 情報バーをクリア */
		__mfmenu__gParams->Info.Text[0] = '\0';
		__mfmenu__gParams->Info.Options = 0;
		
		/* 次のプロシージャは今回と同じと仮定 */
		__mfmenu__gParams->NextProc = __mfmenu__gParams->Proc;
		
		/* ダイアログ状況取得 */
		MfDialogType dialog = mfDialogCurrentType();
		if( dialog ){
			if( __mfmenu__gParams->LastDialogType != dialog ){
				__mfmenu__gParams->Message.Sub |= MF_DM_START;
				mfMenuEnable( MF_MENU_SCREEN_UPDATE );
			} else{
				__mfmenu__gParams->Message.Sub |= MF_DM_UPDATE;
			}
		} else if( __mfmenu__gParams->LastDialogType ){
			__mfmenu__gParams->Message.Sub |= MF_DM_FINISH;
			mfMenuEnable( MF_MENU_SCREEN_UPDATE );
		}
		__mfmenu__gParams->LastDialogType = dialog;
		
		/* プロシージャ実行 */
		__mfmenu__gParams->ProcRetv = ( __mfmenu__gParams->Proc )( __mfmenu__gParams->Message.Main | __mfmenu__gParams->Message.Sub );
		if( __mfmenu__gParams->ProcRetv != MF_MENU_OK ){
			if( CG_IS_ERROR( __mfmenu__gParams->ProcRetv ) ) mfMenuError( __mfmenu__gParams->ProcRetv, NULL );
			mfMenuProc( NULL );
		}
		
		/* メニューメッセージを確定 */
		if( __mfmenu__gParams->Extra.Proc || mfDialogCurrentType() != MFDIALOG_NONE ){
			__mfmenu__gParams->Message.Main = MF_MM_WAIT;
		} else if( __mfmenu__gParams->Message.Main == MF_MM_WAIT ){
			__mfmenu__gParams->Message.Main = MF_MM_CONTINUE;
		} else{
			__mfmenu__gParams->Message.Main = MF_MM_PROC;
		}
		
		/* サブメッセージをクリアしてシグナルを設定 */
		__mfmenu__gParams->Message.Sub  = __mfmenu__gParams->Message.Signal;
		__mfmenu__gParams->Message.Signal = 0;
		
		/* 次のメニュープロシージャが変更されていれば設定 */
		if( ! __mfmenu__gParams->NextProc ){
			dbgprint( "Menu back" );
			( __mfmenu__gParams->Proc )( MF_MM_TERM );
			__mfmenu_clear_temp_memory( __mfmenu__gParams->Proc );
			mfmenu_stack( &(__mfmenu__gParams->Stack), MF_MENU_STACK_POP, &(__mfmenu__gParams->Proc), &(__mfmenu__gParams->Menu) );
		} else if( __mfmenu__gParams->NextProc != __mfmenu__gParams->Proc ){
			dbgprint( "Menu enter" );
			int ret;
			struct MfMenuPosition pos = __mfmenu__gParams->Menu;
			if( ( ret = ( __mfmenu__gParams->NextProc )( MF_MM_INIT ) ) == MF_MENU_OK ){
				mfmenu_stack( &(__mfmenu__gParams->Stack), MF_MENU_STACK_PUSH, __mfmenu__gParams->Proc, &pos );
				__mfmenu__gParams->Proc = __mfmenu__gParams->NextProc;
			} else{
				if( CG_IS_ERROR( ret ) ) mfMenuError( ret, NULL );
				__mfmenu__gParams->Menu = pos;
			}
		}
		
		/* ダイアログ実行 */
		if( __mfmenu__gParams->LastDialogType ){
			__mfmenu__gParams->Info.Text[0] = '\0';
			
			if( dialog == MFDIALOG_NONE ){
				__mfmenu__gParams->LastDialogType = dialog;
				mfMenuResetKeyRepeat();
			} else{
				int ret = mfDialogDraw();
				if( ret == MFDIALOG_DRAW_FINISH ){
					mfDialogResult();
				} else if( CG_IS_ERROR( ret ) ){
					mfDialogResult();
					mfMenuError( ret, NULL );
				}
			}
		}
		
		/* 情報バー描画 */
		if( __mfmenu__gParams->Info.Text[0] != '\0' ){
			pbPrint(
				pbOffsetChar( 3 ),
				pbOffsetLine( 32 ) - ( __mfmenu__gParams->Info.Options & MF_MENU_INFOTEXT_SET_MIDDLE_LINE ? 0 : ( pbOffsetLine( 1 ) >> 1 ) ),
				__mfmenu__gParams->Info.Options & MF_MENU_INFOTEXT_ERROR ? MF_COLOR_TEXT_FC : MF_COLOR_TEXT_FG,
				MF_COLOR_TEXT_BG,
				__mfmenu__gParams->Info.Text
			);
		}
		
		/* 表示バッファと描画バッファを切り替える */
		sceDisplayWaitVblankStart();
		pbSwapBuffers( PB_BUFSYNC_IMMEDIATE );
		
		/* 指定時間待つ */
		if( __mfmenu__gParams->WaitMicroSecForUpdate ){
			sceKernelDelayThread( __mfmenu__gParams->WaitMicroSecForUpdate );
			__mfmenu__gParams->WaitMicroSecForUpdate = 0;
			mfMenuEnable( MF_MENU_SCREEN_UPDATE );
			sceDisplayWaitVblankStart();
		}
	}
	
	pbPrint( pbOffsetChar( 30 ), pbOffsetLine( 17 ), MF_COLOR_TEXT_FG, MF_COLOR_TEXT_BG, MF_STR_HOME_RETURN_TO_GAME );
	sceDisplayWaitVblankStart();
	pbSwapBuffers( PB_BUFSYNC_NEXTFRAME );
	
	/* メニュースタックに詰まれているメニュー全てにMF_MM_TERMを送る */
	dbgprint( "Terminating menu..." );
	while( __mfmenu__gParams->Proc ){
		( __mfmenu__gParams->Proc )( MF_MM_TERM );
		mfmenu_stack( &(__mfmenu__gParams->Stack), MF_MENU_STACK_POP, &(__mfmenu__gParams->Proc), &(__mfmenu__gParams->Menu) );
	}
	
	dbgprint( "Quitting menu-loop" );
	
	/* ダイアログを終了 */
	mfDialogFinish();
	
	goto DESTROY;
	
	DESTROY:
		dbgprint( "Destroying menu stack..." );
		mfmenu_stack( &(__mfmenu__gParams->Stack), MF_MENU_STACK_DESTROY, NULL, NULL );
		
		dbgprint( "Memory block returns to the system..." );
		padctrlDestroy( __mfmenu__gParams->Ctrl.Uid );
		padutilDestroyButtonSymbols();
		
		/* メニューとゲームの壁 */
		sceKernelDelayThread( 300000 );
		
		/* フレームバッファを戻して解放 */
		mfmenu_free_buffers( &(__mfmenu__gParams->FrameBuffer) );
		
		/* サウンドを戻す */
		mfmenu_mute( MF_MENU_MUTE_DISABLE );
		
		/* メニュー起動前にAPIをフックしていた場合は、フックしなおす */
		if( mfIsEnabled() ) mfHook();
		
		/* 他のスレッドをリジューム */
		dbgprint( "Resuming threads..." );
		sceDisplayWaitVblankStart();
		mfmenu_change_threads_stat( MF_MENU_RESUME_THREADS, __mfmenu__gParams->Thread.List, __mfmenu__gParams->Thread.Count );
		
#ifdef DEBUG_ENABLED
		dbgprintf( "Memory leaking for menu: %d", __mfmenu__g_memalloc_count );
#endif
		/* メニュー用のメモリを解放 */
		dbgprint( "Menu finish" );
		dmemDestroy( __mfmenu__gParams->MemoryUid );
		
		/* ディスプレイを点灯させる */
		sceDisplayWaitVblankStart();
		sceDisplayWaitVblankStart();
		sceDisplayEnable();
}

#ifdef DEBUG_ENABLED
static void mfmenu_debug( void )
{
	static int fps = 0;
	static int st_fps = 0;
	static uint64_t st_fps_tick_now = 0;
	static uint64_t st_fps_tick_last = 0;
	
	st_fps++;
	sceRtcGetCurrentTick( &st_fps_tick_now );
	if( (st_fps_tick_now - st_fps_tick_last) >= 1000000 ){
		st_fps_tick_last = st_fps_tick_now;
		fps = st_fps;
		st_fps = 0;
	}
		
	pbPrintf( pbOffsetChar( 5 ), pbOffsetLine( 2 ), 0xff00ffff, MF_COLOR_TEXT_BG,
		"FPS:%d T:%d M:%d (KH:%d U:%d KL:%d V:%d)",
		fps,
		sceKernelTotalFreeMemSize(),
		sceKernelMaxFreeMemSize(),
		sceKernelPartitionMaxFreeMemSize( 1 ),
		sceKernelPartitionMaxFreeMemSize( 2 ),
		sceKernelPartitionMaxFreeMemSize( 4 ),
		sceKernelPartitionMaxFreeMemSize( 5 )
	);
	pbPrintf( pbOffsetChar( 1 ), pbOffsetLine( 0 ), 0xff0000ff, MF_COLOR_TEXT_BG, "Development build - %s", __DATE__ );
}
#endif

static bool mfmenu_init_params( struct MfMenuParams *params, SceCtrlData *pad, MfHprmKey *hk )
{
	int ox_swap;
	
	/* メニュースタックを確保 */
	if( ! mfmenu_stack( &(__mfmenu__gParams->Stack), MF_MENU_STACK_CREATE, NULL, NULL ) ) return false;
	
	/* ボタンシンボルリスト生成 */
	__mfmenu__gParams->ButtonSymbolTable = padutilCreateButtonSymbols();
	if( ! __mfmenu__gParams->ButtonSymbolTable ){
		dbgprint( "Failed to create buttons symbol table" );
		return false;
	}
	
	/* ダイアログ初期化 */
	mfDialogInit( __mfmenu__gPref.Remap );
	__mfmenu__gParams->LastDialogType = mfDialogCurrentType();
	
	/* コントロール */
	__mfmenu__gParams->Ctrl.Pad     = pad;
	__mfmenu__gParams->Ctrl.HprmKey = hk;
	__mfmenu__gParams->Ctrl.Uid     = padctrlNew();
	if( __mfmenu__gParams->Ctrl.Uid < 0 ){
		dbgprintf( "Failed to get PadctrlUID: %x", __mfmenu__gParams->Ctrl.Uid );
		__mfmenu__gParams->Ctrl.Uid = 0;
		return false;
	}
	
	/* ○×ボタン入れ替えチェック */
	if( sceUtilityGetSystemParamInt( PSP_SYSTEMPARAM_ID_INT_UNKNOWN, &ox_swap ) == PSP_SYSTEMPARAM_RETVAL_FAIL ){
		ox_swap = PSP_UTILITY_ACCEPT_CIRCLE;
	}
	if( ox_swap == PSP_UTILITY_ACCEPT_CROSS ){
		__mfmenu__gParams->Ctrl.Accept.Button = PSP_CTRL_CROSS;
		__mfmenu__gParams->Ctrl.Cancel.Button = PSP_CTRL_CIRCLE;
		__mfmenu__gParams->Ctrl.Accept.Symbol = PB_SYM_PSP_CROSS;
		__mfmenu__gParams->Ctrl.Cancel.Symbol = PB_SYM_PSP_CIRCLE;
		
		/* ×ボタンが決定の場合、ダイアログの環境フラグをセット */
		cdialogEnable( CDIALOG_ACCEPT_CROSS );
	} else{
		__mfmenu__gParams->Ctrl.Accept.Button = PSP_CTRL_CIRCLE;
		__mfmenu__gParams->Ctrl.Cancel.Button = PSP_CTRL_CROSS;
		__mfmenu__gParams->Ctrl.Accept.Symbol = PB_SYM_PSP_CIRCLE;
		__mfmenu__gParams->Ctrl.Cancel.Symbol = PB_SYM_PSP_CROSS;
	}
	/* リピートボタンを設定 */
	padctrlSetRepeatButtons( __mfmenu__gParams->Ctrl.Uid,
		PSP_CTRL_UP | PSP_CTRL_RIGHT | PSP_CTRL_DOWN | PSP_CTRL_LEFT |
		__mfmenu__gParams->Ctrl.Accept.Button | PSP_CTRL_SQUARE | PSP_CTRL_RTRIGGER | PSP_CTRL_LTRIGGER
	);
	
	/* メッセージを初期化 */
	__mfmenu__gParams->Message.Main   = MF_MM_PROC; /* 最初に送るメッセージ */
	__mfmenu__gParams->Message.Sub    = 0;
	__mfmenu__gParams->Message.Signal = 0;
	
	return true;
}

static bool mfmenu_init_graphics( void )
{
	/* pbライブラリ初期化 */
	dbgprint( "Initializing pb..." );
	pbInit();
	pbEnable( PB_NO_CACHE | PB_BLEND | PB_STEAL_DISPLAY );
	
	/* フレームバッファとして使用するメモリを確保 */
	dbgprint( "Allocating memory for frame buffers..." );
	if( ! mfmenu_alloc_buffers( &(__mfmenu__gParams->FrameBuffer), ! __mfmenu__gPref.LowMemoryMode ) ) return false;
	
	/* ダブルバッファリングを有効化 */
	if( __mfmenu__gParams->FrameBuffer.Frame[MF_MENU_FB_DRAW].Addr ){
		pbSetDrawBuffer( MF_MENU_FB_PIXELFORMAT, __mfmenu__gParams->FrameBuffer.Frame[MF_MENU_FB_DRAW].Addr, MF_MENU_FB_WIDTH );
		pbEnable( PB_DOUBLE_BUFFER );
	}
	
	pbSetDisplayBuffer( MF_MENU_FB_PIXELFORMAT, __mfmenu__gParams->FrameBuffer.Frame[MF_MENU_FB_DISP].Addr, MF_MENU_FB_WIDTH );
	
	/* 設定したフレームバッファで描画準備 */
	pbApply();
	if( CG_IS_ERROR( pbSyncDisplay( PB_BUFSYNC_NEXTFRAME ) ) ) return false;
	sceDisplayWaitVblankStart();
	
	/* 表示バッファの初期化 */
	if( __mfmenu__gParams->FrameBuffer.Frame[MF_MENU_FB_CLEAR].Addr ){
		sceDmacMemcpy( __mfmenu__gParams->FrameBuffer.Frame[MF_MENU_FB_DISP].Addr, __mfmenu__gParams->FrameBuffer.Frame[MF_MENU_FB_CLEAR].Addr, __mfmenu__gParams->FrameBuffer.Size );
	} else{
		memset( __mfmenu__gParams->FrameBuffer.Frame[MF_MENU_FB_DISP].Addr, 0, __mfmenu__gParams->FrameBuffer.Size );
	}
	
	return true;
}


static void mfmenu_mute( int ( *audio )( int ) )
{
	unsigned int k1 = pspSdkSetK1( 0 );
	( audio )( 0 );
	pspSdkSetK1( k1 );
}

static void mfmenu_indicator( void )
{
	static char lst_working_indicator[] = "|/-\\";
	static int  lst_working_indicator_index = 0;
	
	if( lst_working_indicator[lst_working_indicator_index] == '\0' ) lst_working_indicator_index = 0;
	pbPutChar( pbOffsetChar( 1 ), pbOffsetLine( 1 ), MF_COLOR_TITLE, MF_COLOR_TRANSPARENT, lst_working_indicator[lst_working_indicator_index++] );
}

static bool mfmenu_stack( struct MfMenuStack *stack, enum MfMenuStackAction ctrl, void *menu, struct MfMenuPosition *pos )
{
	switch( ctrl ){
		case MF_MENU_STACK_CREATE:
			stack->Index = 0;
			stack->Size  = MF_MENU_STACK_SIZE( MF_MENU_STACK_NUM );
			stack->Data  = (struct MfMenuStackData *)mfMenuMemoryAlloc( stack->Size );
			dbgprintf( "Allocating memory for menu-stack: %p: %d bytes", stack->Data, stack->Size );
			return stack->Data ? true : false;
		case MF_MENU_STACK_PUSH:
			if( stack->Index >= MF_MENU_STACK_NUM ){
				void *old_stack       = stack->Data;
				unsigned int old_size = stack->Size;
				stack->Size += MF_MENU_STACK_SIZE( MF_MENU_STACK_NUM );
				stack->Data = (struct MfMenuStackData *)mfMenuMemoryAlloc( stack->Size );
				dbgprintf( "Re-allocating memory for menu-stack: %p: %d bytes", stack->Data, stack->Size );
				if( ! stack->Data ){
					dbgprint( "Failed to re-allocate memory" );
					stack->Data = old_stack;
					return false;
				} else{
					memcpy( stack->Data, old_stack, old_size );
					mfMenuMemoryFree( old_stack );
				}
			}
			stack->Data[stack->Index].Proc = menu;
			if( pos ) stack->Data[stack->Index].Pos = *pos;
			dbgprintf( "Menu address push to menu-stack: %p", stack->Data[stack->Index].Proc );
			stack->Index++;
			return true;
		case MF_MENU_STACK_POP:
			if( stack->Index ){
				stack->Index--;
				dbgprintf( "Menu address pop from menu-stack: %p", stack->Data[stack->Index].Proc );
				*((void **)menu) = stack->Data[stack->Index].Proc;
				if( pos ) *pos = stack->Data[stack->Index].Pos;
				return true;
			} else{
				*((void **)menu) = NULL;
				if( pos ) memset( pos, 0, sizeof( struct MfMenuPosition ) );
				return false;
			}
		case MF_MENU_STACK_DESTROY:
			mfMenuMemoryFree( stack->Data );
			return true;
	}
	return false;
}

static bool mfmenu_get_target_threads( SceUID *thread_id_list, int *thread_id_count )
{
	int i, j;
	SceUID notifier;
	
	if( ( notifier = mfNotificationThreadId() ) ) mfNotificationPrintTerm();
	
	if( sceKernelGetThreadmanIdList( SCE_KERNEL_TMID_Thread, thread_id_list, MF_MENU_MAX_NUMBER_OF_THREADS, thread_id_count ) < 0 ){
		dbgprint( "Failed to get a thread-id list" );
		return false;
	}
	
	for( i = 0; i < *thread_id_count; i++ ){
		for( j = 0; j < __mfmenu__gSysThreads.Count; j++ ){
			if( thread_id_list[i] == __mfmenu__gSysThreads.List[j] || thread_id_list[i] == notifier ) thread_id_list[i] = 0;
		}
	}
	
	return true;
}

static void mfmenu_change_threads_stat( int ( *func )( SceUID ), SceUID *thread_id_list, int thread_id_count )
{
	int i;
	
	if( ! func || ! thread_id_list || ! thread_id_count ) return;
	
	dbgprint( "Controling all-threads without notification thread..." );
	for( i = 0; i < thread_id_count; i++ ){
		if( thread_id_list[i] ) ( func )( thread_id_list[i] );
	}
}

static bool mfmenu_alloc_buffers( struct MfMenuFrameBuffer *fb, bool graphics_mem )
{
	void *vram_start = (void *)( (uintptr_t)sceGeEdramGetAddr() | PB_DISABLE_CACHE );
	void *vram_end   = (void *)( (uintptr_t)vram_start + sceGeEdramGetSize() );
	
	unsigned int frame_num = 0;
	void *cur_fb_addr;
	unsigned int cur_fb_size;
	int cur_fb_width, cur_fb_format;
	
	/* 現在のフレームバッファ情報を取得 */
	if( sceDisplayGetFrameBuf( &cur_fb_addr, &cur_fb_width, &cur_fb_format, PSP_DISPLAY_SETBUF_IMMEDIATE ) != 0 || ! cur_fb_addr || ! cur_fb_width ) return false;
	cur_fb_addr = (void *)( (uintptr_t)cur_fb_addr | PB_DISABLE_CACHE );
	dbgprintf( "Detect current display buffer addr: %p", cur_fb_addr );
	
	/* 現在のフレームのサイズを取得 */
	cur_fb_size = pbGetFrameBufferDataSize( cur_fb_format, cur_fb_width );
	
	/* メニューの1フレームのサイズを取得 */
	fb->Size = pbGetFrameBufferDataSize( MF_MENU_FB_PIXELFORMAT, MF_MENU_FB_WIDTH );
	dbgprintf( "Frame size: %d bytes", fb->Size );
	
	/* 空きメモリから確保可能なフレーム数を取得 */
	if( graphics_mem ){
		frame_num = sceKernelMaxFreeMemSize() / fb->Size;
		if( frame_num > 3 ) frame_num = 3;
	}
	
	dbgprintf( "Frame buffer allocated for backup: %d bytes", fb->Size * frame_num );
	if( frame_num && ! ( fb->Backup = mfMemoryAllocHi( fb->Size * frame_num ) ) ){
		frame_num = 0;
	}
	
	/* フレームバッファとして使うVRAMアドレスを仮定 */
	fb->Frame[MF_MENU_FB_DISP].Addr  = cur_fb_addr;
	fb->Frame[MF_MENU_FB_DRAW].Addr  = (void *)( (uintptr_t)fb->Frame[MF_MENU_FB_DISP].Addr + fb->Size );
	fb->Frame[MF_MENU_FB_CLEAR].Addr = (void *)( (uintptr_t)fb->Frame[MF_MENU_FB_DRAW].Addr + fb->Size );
	if( fb->Frame[MF_MENU_FB_CLEAR].Addr >= vram_end ){
		fb->Frame[MF_MENU_FB_DRAW].Addr = vram_start;
		fb->Frame[MF_MENU_FB_CLEAR].Addr = (void *)( (uintptr_t)fb->Frame[MF_MENU_FB_DRAW].Addr + fb->Size );
	} else if( (void *)( (uintptr_t)fb->Frame[MF_MENU_FB_CLEAR].Addr + fb->Size ) >= vram_end ){
		fb->Frame[MF_MENU_FB_CLEAR].Addr = vram_start;
	}
	
	/* バックアップ可能なフレーム数に応じて調整 */
	if( ! frame_num ){
		if( cur_fb_format != PB_PXF_8888 ) fb->Frame[MF_MENU_FB_DRAW].Addr = NULL;
		fb->Frame[MF_MENU_FB_CLEAR].Addr = NULL;
	} else{
		uintptr_t backup_offset = (uintptr_t)fb->Backup;
		
		/* breakせずに順番にフレームバッファのリストアデータを設定 */
		switch( frame_num ){
			case 3:
				fb->Frame[MF_MENU_FB_DISP].Restore = (void *)backup_offset;
				backup_offset += fb->Size;
			case 2:
				if( cur_fb_format != PB_PXF_8888 ){
					fb->Frame[MF_MENU_FB_CLEAR].Restore = (void *)backup_offset;
				} else{
					fb->Frame[MF_MENU_FB_DRAW].Restore = (void *)backup_offset;
				}
				backup_offset += fb->Size;
			case 1:
				if( cur_fb_format != PB_PXF_8888 ){
					fb->Frame[MF_MENU_FB_DRAW].Restore = (void *)backup_offset;
				} else{
					fb->Frame[MF_MENU_FB_CLEAR].Restore = (void *)backup_offset;
				}
		}
	}
	
	dbgprintf( "Display buffer address: %p, %p", fb->Frame[MF_MENU_FB_DISP].Addr, fb->Frame[MF_MENU_FB_DISP].Restore );
	dbgprintf( "Draw buffer address: %p, %p", fb->Frame[MF_MENU_FB_DRAW].Addr, fb->Frame[MF_MENU_FB_DRAW].Restore );
	dbgprintf( "Clear buffer address: %p, %p", fb->Frame[MF_MENU_FB_CLEAR].Addr, fb->Frame[MF_MENU_FB_CLEAR].Restore );
	
	/* バックアップ */
	if( fb->Frame[MF_MENU_FB_DISP].Restore  ) sceDmacMemcpy( fb->Frame[MF_MENU_FB_DISP].Restore, fb->Frame[MF_MENU_FB_DISP].Addr, fb->Size );
	if( fb->Frame[MF_MENU_FB_DRAW].Restore  ) sceDmacMemcpy( fb->Frame[MF_MENU_FB_DRAW].Restore, fb->Frame[MF_MENU_FB_DRAW].Addr, fb->Size );
	if( fb->Frame[MF_MENU_FB_CLEAR].Restore ){
		PbColor color;
		void ( *ccv )( PbColor*, unsigned int );
		int i, pxcount = cur_fb_width * SCR_HEIGHT;
		
		sceDmacMemcpy( fb->Frame[MF_MENU_FB_CLEAR].Restore, fb->Frame[MF_MENU_FB_CLEAR].Addr, fb->Size );
		
		switch( cur_fb_format ){
			case PB_PXF_5650: ccv = pbColorParse5650; break;
			case PB_PXF_5551: ccv = pbColorParse5551; break;
			case PB_PXF_4444: ccv = pbColorParse4444; break;
			default:          ccv = pbColorParse8888; break;
		}
		
		for( i = 0; i < pxcount; i++ ){
			if( cur_fb_format == PB_PXF_8888 ){
				( ccv )( &color, ((uint32_t *)cur_fb_addr)[i] );
			} else{
				( ccv )( &color, ((uint16_t *)cur_fb_addr)[i] );
			}
			((uint16_t *)fb->Frame[MF_MENU_FB_CLEAR].Addr)[i] = pbColorGet5650( &color );
		}
		
		/* クリア用バッファに枠を描画しておく */
		pbSetDisplayBuffer( MF_MENU_FB_PIXELFORMAT, __mfmenu__gParams->FrameBuffer.Frame[MF_MENU_FB_CLEAR].Addr, MF_MENU_FB_WIDTH );
		pbApply();
		pbFillRect( 0, 0, SCR_WIDTH, SCR_HEIGHT, MF_COLOR_BG );
		mfmenu_draw_frame();
	} else{
		fb->Frame[MF_MENU_FB_CLEAR].Addr = NULL;
	}
	
	return true;
}

static void mfmenu_free_buffers( struct MfMenuFrameBuffer *fb )
{
	if( fb->Backup ){
		pbReturnDisplay( PB_BUFSYNC_NEXTFRAME );
		sceDisplayWaitVblankStart();
		if( fb->Frame[MF_MENU_FB_DISP].Restore ) sceDmacMemcpy( fb->Frame[MF_MENU_FB_DISP].Addr, fb->Frame[MF_MENU_FB_DISP].Restore, fb->Size );
		if( fb->Frame[MF_MENU_FB_DRAW].Restore ) sceDmacMemcpy( fb->Frame[MF_MENU_FB_DRAW].Addr, fb->Frame[MF_MENU_FB_DRAW].Restore, fb->Size );
		if( fb->Frame[MF_MENU_FB_CLEAR].Restore ) sceDmacMemcpy( fb->Frame[MF_MENU_FB_CLEAR].Addr, fb->Frame[MF_MENU_FB_CLEAR].Restore, fb->Size );
		mfMemoryFree( fb->Backup );
	} else{
		pbReturnDisplay( PB_BUFSYNC_NEXTFRAME );
	}
}

static void mfmenu_draw_frame( void )
{
	pbFillRect( 0, 0, SCR_WIDTH, pbOffsetLine( 3 ), MF_COLOR_TITLE_BAR );
	pbFillRect( 0, SCR_HEIGHT - pbOffsetLine( 3 ), SCR_WIDTH, SCR_HEIGHT, MF_COLOR_INFO_BAR );
	pbPrintf( pbOffsetChar( 3 ), pbOffsetLine( 1 ), MF_COLOR_TEXT_FG, MF_COLOR_TEXT_BG, MF_TITLE, MF_VERSION );
	pbPrint( pbOffsetChar( 47 ), SCR_HEIGHT - pbOffsetLine( 1 ), ( MF_COLOR_TEXT_FG & 0x00ffffff ) | 0x33000000, MF_COLOR_TEXT_BG, MF_AUTHOR );
	
	pbPrintf( pbOffsetChar( 36 ), pbOffsetLine( 1 ), MF_COLOR_TEXT_FG, MF_COLOR_TEXT_BG, "[ID: %s]", mfGetGameId() );
	
#ifdef MF_WITH_EXCEPTION_HANDLER
		pbPrint( SCR_WIDTH - pbOffsetChar( 12 ), 0, MF_COLOR_EX4, MF_COLOR_TEXT_BG, "<for DEBUG>" );
#endif
}

static int mfmenu_root_menu( MfMenuMessage message )
{
	static MfMenuTable *menu;
	static struct MfMenuMainPref *main_pref;
	
	switch( message ){
		case MF_MM_INIT:
			dbgprint( "Initializing root-menu" );
			menu = mfMenuCreateTables(
				3,
				1, 1,
				4, 1,
				gMftabEntryCount, 1
			);
			if( menu ){
				unsigned int row;
				void *menuproc;
				MfCtrlDefGetButtonsPref *hotkey_pref = mfMenuMemoryCalloc( sizeof( MfCtrlDefGetButtonsPref ) );
				main_pref = mfMenuMemoryCalloc( sizeof( struct MfMenuMainPref ) );
				
				if( ! hotkey_pref || ! main_pref || ! mfMenuRegisterTempMemory( mfmenu_root_menu, 3, menu, hotkey_pref, main_pref ) ){
					mfMenuDestroyTables( menu );
					if( hotkey_pref ) mfMenuMemoryFree( hotkey_pref );
					if( main_pref   ) mfMenuMemoryFree( main_pref );
					return CG_ERROR_NOT_ENOUGH_MEMORY;
				}
				
				main_pref->engine             = mfIsEnabled();
				main_pref->menu               = mfGetMenuButtons();
				main_pref->toggle             = mfGetToggleButtons();
				main_pref->statusNotification = mfNotificationThreadId() ? true : false;
				
				hotkey_pref->availButtons = MF_HOTKEY_BUTTONS;
				
				mfMenuSetTablePosition( menu, 1, pbOffsetChar( 5 ), pbOffsetLine( 4 ) + 2 );
				mfMenuSetTableEntry( menu, 1, 1, 1, MF_STR_HOME_MACROFIRE_ENGINE, mfmenu_control_engine, &(main_pref->engine), NULL );
				
				mfMenuSetTablePosition( menu, 2, pbOffsetChar( 5 ), pbOffsetLine( 6 ) + 2 );
				mfMenuSetTableLabel( menu, 2, MF_STR_HOME_MACROFIRE_PREFERENCE );
				mfMenuSetTableEntry( menu, 2, 1, 1, MF_STR_HOME_SET_MENU_BUTTON, mfCtrlDefGetButtons, &(main_pref->menu), hotkey_pref );
				mfMenuSetTableEntry( menu, 2, 2, 1, MF_STR_HOME_SET_TOGGLE_BUTTON, mfCtrlDefGetButtons, &(main_pref->toggle), hotkey_pref );
				mfMenuSetTableEntry( menu, 2, 3, 1, MF_STR_HOME_STAT_NOTIFICATION, mfCtrlDefBool, &(main_pref->statusNotification), NULL );
				mfMenuSetTableEntry( menu, 2, 4, 1, MF_STR_HOME_ANALOG_STICK_ADJUSTMENT, mfCtrlDefCallback, mfAnalogStickMenu, NULL );
				
				mfMenuSetTablePosition( menu, 3, pbOffsetChar( 5 ), pbOffsetLine( 12 ) + 2 );
				mfMenuSetTableLabel( menu, 3, MF_STR_HOME_FUNCTION );
				for( row = 0; row < gMftabEntryCount; row++ ){
					if( gMftab[row].proc && ( menuproc = ( gMftab[row].proc )( MF_MS_MENU ) ) ){
						mfMenuSetTableEntry( menu, 3, row + 1, 1, gMftab[row].name, mfCtrlDefCallback, menuproc, NULL );
					} else{
						mfMenuSetTableEntry( menu, 3, row + 1, 1, gMftab[row].name, NULL, NULL, NULL );
					}
				}
				mfMenuInitTables( menu, 3 );
				break;
			}
			return CG_ERROR_NOT_ENOUGH_MEMORY;
		case MF_MM_TERM:
			dbgprint( "Terminating root-menu" );
			mfSetMenuButtons( main_pref->menu );
			mfSetToggleButtons( main_pref->toggle );
			
			if( mfNotificationThreadId() && ! main_pref->statusNotification ){
				dbgprint( "Notification thread is shutting down..." );
				mfNotificationShutdownStart();
			} else if( ! mfNotificationThreadId() && main_pref->statusNotification ){
				dbgprint( "Notification thread is starting..." );
				mfNotificationStart();
			}
			break;
		default:
			return mfMenuDrawTables( menu, 3, MF_MENU_NO_OPTIONS );
	}
	return MF_MENU_OK;
}

static int mfmenu_control_engine( MfMessage message, const char *label, void *var, void *unused, void *ex )
{
	bool prev_stat = *((bool *)var);
	int ret = mfCtrlDefBool( message, label, var, unused, ex );
	
	if( prev_stat != *((bool *)var) ){
		if( *((bool *)var) ){
			mfEnable();
		} else{
			mfDisable();
		}
	}
	
	return ret;
}
