/* mfMenuMain.c */

#include "mfmenu_types.h"

/*-----------------------------------------------
	�萔
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
	�}�N��
-----------------------------------------------*/

/*-----------------------------------------------
	���[�J���֐��v���g�^�C�v
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
	�f�o�b�O�p
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
		
		/* ���[�U���������烁�j���[�p�̍�ƈ���m�� */
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
	
	/* �T�X�y���h/���W���[������X���b�h�̈ꗗ���쐬���A�T�X�y���h���� */
	if( ! mfmenu_get_target_threads( __mfmenu__gParams->Thread.List, &(__mfmenu__gParams->Thread.Count) ) ) goto DESTROY;
	mfmenu_change_threads_stat( MF_MENU_SUSPEND_THREADS, __mfmenu__gParams->Thread.List, __mfmenu__gParams->Thread.Count );
	
	/* �����Ɏ~�܂�Ȃ��X���b�h������? */
	sceDisplayWaitVblankStart();
	
	/* �T�E���h���~���[�g�ɂ��� */
	mfmenu_mute( MF_MENU_MUTE_ENABLE );
	
	/* API���t�b�N����Ă���ꍇ�A�ꎞ�I�Ɍ��ɖ߂� */
	if( mfIsEnabled() ) mfUnhook();
	
	/* params�������� */
	if( ! mfmenu_init_params( __mfmenu__gParams, pad, hk ) ) goto DESTROY;
	
	/* ���j���[�t���O�̏����� */
	mfMenuEnable( MF_MENU_SCREEN_UPDATE | MF_MENU_EXIT );
	
	/* ���[�g���j���[�ݒ� */
	__mfmenu__gParams->Proc = (MfFuncMenu)mfmenu_root_menu;
	( __mfmenu__gParams->Proc )( MF_MM_INIT );
	
	/* �O���t�B�b�N�������� */
	if( ! mfmenu_init_graphics() ) goto DESTROY;
	
	dbgprint( "Starting menu-loop" );
	while( gRunning ){
		/* �w�i��`�� */
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
		// �L�[���s�[�g���K�v sceHprmPeekCurrentKey( __mfmenu__gParams->Ctrl.HprmKey );
		padutilRemap( __mfmenu__gPref.Remap, padutilSetPad( mfMenuGetCurrentButtons() ), __mfmenu__gParams->Ctrl.Pad, __mfmenu__gParams->Ctrl.HprmKey, true );
		
#ifdef DEBUG_ENABLED
		mfmenu_debug();
#endif
		
		/*
			MF_MENU_FORCED_QUIT���Z�b�g���ꂽ���A
			MF_MENU_EXIT���L���ȏ�Ԃ�START/HOME�������������A
			���̃v���V�[�W�������݂��Ȃ���΃��j���[�𔲂���B
		*/
		if(
			( __mfmenu__gParams->Flags & MF_MENU_FORCED_QUIT ) ||
			( ( __mfmenu__gParams->Flags & MF_MENU_EXIT ) && ( __mfmenu__gParams->Ctrl.Pad->Buttons & ( PSP_CTRL_START | PSP_CTRL_HOME ) ) ) ||
			! __mfmenu__gParams->Proc
		) break;
		
		/* ���o�[���N���A */
		__mfmenu__gParams->Info.Text[0] = '\0';
		__mfmenu__gParams->Info.Options = 0;
		
		/* ���̃v���V�[�W���͍���Ɠ����Ɖ��� */
		__mfmenu__gParams->NextProc = __mfmenu__gParams->Proc;
		
		/* �_�C�A���O�󋵎擾 */
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
		
		/* �v���V�[�W�����s */
		__mfmenu__gParams->ProcRetv = ( __mfmenu__gParams->Proc )( __mfmenu__gParams->Message.Main | __mfmenu__gParams->Message.Sub );
		if( __mfmenu__gParams->ProcRetv != MF_MENU_OK ){
			if( CG_IS_ERROR( __mfmenu__gParams->ProcRetv ) ) mfMenuError( __mfmenu__gParams->ProcRetv, NULL );
			mfMenuProc( NULL );
		}
		
		/* ���j���[���b�Z�[�W���m�� */
		if( __mfmenu__gParams->Extra.Proc || mfDialogCurrentType() != MFDIALOG_NONE ){
			__mfmenu__gParams->Message.Main = MF_MM_WAIT;
		} else if( __mfmenu__gParams->Message.Main == MF_MM_WAIT ){
			__mfmenu__gParams->Message.Main = MF_MM_CONTINUE;
		} else{
			__mfmenu__gParams->Message.Main = MF_MM_PROC;
		}
		
		/* �T�u���b�Z�[�W���N���A���ăV�O�i����ݒ� */
		__mfmenu__gParams->Message.Sub  = __mfmenu__gParams->Message.Signal;
		__mfmenu__gParams->Message.Signal = 0;
		
		/* ���̃��j���[�v���V�[�W�����ύX����Ă���ΐݒ� */
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
		
		/* �_�C�A���O���s */
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
		
		/* ���o�[�`�� */
		if( __mfmenu__gParams->Info.Text[0] != '\0' ){
			pbPrint(
				pbOffsetChar( 3 ),
				pbOffsetLine( 32 ) - ( __mfmenu__gParams->Info.Options & MF_MENU_INFOTEXT_SET_MIDDLE_LINE ? 0 : ( pbOffsetLine( 1 ) >> 1 ) ),
				__mfmenu__gParams->Info.Options & MF_MENU_INFOTEXT_ERROR ? MF_COLOR_TEXT_FC : MF_COLOR_TEXT_FG,
				MF_COLOR_TEXT_BG,
				__mfmenu__gParams->Info.Text
			);
		}
		
		/* �\���o�b�t�@�ƕ`��o�b�t�@��؂�ւ��� */
		sceDisplayWaitVblankStart();
		pbSwapBuffers( PB_BUFSYNC_IMMEDIATE );
		
		/* �w�莞�ԑ҂� */
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
	
	/* ���j���[�X�^�b�N�ɋl�܂�Ă��郁�j���[�S�Ă�MF_MM_TERM�𑗂� */
	dbgprint( "Terminating menu..." );
	while( __mfmenu__gParams->Proc ){
		( __mfmenu__gParams->Proc )( MF_MM_TERM );
		mfmenu_stack( &(__mfmenu__gParams->Stack), MF_MENU_STACK_POP, &(__mfmenu__gParams->Proc), &(__mfmenu__gParams->Menu) );
	}
	
	dbgprint( "Quitting menu-loop" );
	
	/* �_�C�A���O���I�� */
	mfDialogFinish();
	
	goto DESTROY;
	
	DESTROY:
		dbgprint( "Destroying menu stack..." );
		mfmenu_stack( &(__mfmenu__gParams->Stack), MF_MENU_STACK_DESTROY, NULL, NULL );
		
		dbgprint( "Memory block returns to the system..." );
		padctrlDestroy( __mfmenu__gParams->Ctrl.Uid );
		padutilDestroyButtonSymbols();
		
		/* ���j���[�ƃQ�[���̕� */
		sceKernelDelayThread( 300000 );
		
		/* �t���[���o�b�t�@��߂��ĉ�� */
		mfmenu_free_buffers( &(__mfmenu__gParams->FrameBuffer) );
		
		/* �T�E���h��߂� */
		mfmenu_mute( MF_MENU_MUTE_DISABLE );
		
		/* ���j���[�N���O��API���t�b�N���Ă����ꍇ�́A�t�b�N���Ȃ��� */
		if( mfIsEnabled() ) mfHook();
		
		/* ���̃X���b�h�����W���[�� */
		dbgprint( "Resuming threads..." );
		sceDisplayWaitVblankStart();
		mfmenu_change_threads_stat( MF_MENU_RESUME_THREADS, __mfmenu__gParams->Thread.List, __mfmenu__gParams->Thread.Count );
		
#ifdef DEBUG_ENABLED
		dbgprintf( "Memory leaking for menu: %d", __mfmenu__g_memalloc_count );
#endif
		/* ���j���[�p�̃���������� */
		dbgprint( "Menu finish" );
		dmemDestroy( __mfmenu__gParams->MemoryUid );
		
		/* �f�B�X�v���C��_�������� */
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
	
	/* ���j���[�X�^�b�N���m�� */
	if( ! mfmenu_stack( &(__mfmenu__gParams->Stack), MF_MENU_STACK_CREATE, NULL, NULL ) ) return false;
	
	/* �{�^���V���{�����X�g���� */
	__mfmenu__gParams->ButtonSymbolTable = padutilCreateButtonSymbols();
	if( ! __mfmenu__gParams->ButtonSymbolTable ){
		dbgprint( "Failed to create buttons symbol table" );
		return false;
	}
	
	/* �_�C�A���O������ */
	mfDialogInit( __mfmenu__gPref.Remap );
	__mfmenu__gParams->LastDialogType = mfDialogCurrentType();
	
	/* �R���g���[�� */
	__mfmenu__gParams->Ctrl.Pad     = pad;
	__mfmenu__gParams->Ctrl.HprmKey = hk;
	__mfmenu__gParams->Ctrl.Uid     = padctrlNew();
	if( __mfmenu__gParams->Ctrl.Uid < 0 ){
		dbgprintf( "Failed to get PadctrlUID: %x", __mfmenu__gParams->Ctrl.Uid );
		__mfmenu__gParams->Ctrl.Uid = 0;
		return false;
	}
	
	/* ���~�{�^������ւ��`�F�b�N */
	if( sceUtilityGetSystemParamInt( PSP_SYSTEMPARAM_ID_INT_UNKNOWN, &ox_swap ) == PSP_SYSTEMPARAM_RETVAL_FAIL ){
		ox_swap = PSP_UTILITY_ACCEPT_CIRCLE;
	}
	if( ox_swap == PSP_UTILITY_ACCEPT_CROSS ){
		__mfmenu__gParams->Ctrl.Accept.Button = PSP_CTRL_CROSS;
		__mfmenu__gParams->Ctrl.Cancel.Button = PSP_CTRL_CIRCLE;
		__mfmenu__gParams->Ctrl.Accept.Symbol = PB_SYM_PSP_CROSS;
		__mfmenu__gParams->Ctrl.Cancel.Symbol = PB_SYM_PSP_CIRCLE;
		
		/* �~�{�^��������̏ꍇ�A�_�C�A���O�̊��t���O���Z�b�g */
		cdialogEnable( CDIALOG_ACCEPT_CROSS );
	} else{
		__mfmenu__gParams->Ctrl.Accept.Button = PSP_CTRL_CIRCLE;
		__mfmenu__gParams->Ctrl.Cancel.Button = PSP_CTRL_CROSS;
		__mfmenu__gParams->Ctrl.Accept.Symbol = PB_SYM_PSP_CIRCLE;
		__mfmenu__gParams->Ctrl.Cancel.Symbol = PB_SYM_PSP_CROSS;
	}
	/* ���s�[�g�{�^����ݒ� */
	padctrlSetRepeatButtons( __mfmenu__gParams->Ctrl.Uid,
		PSP_CTRL_UP | PSP_CTRL_RIGHT | PSP_CTRL_DOWN | PSP_CTRL_LEFT |
		__mfmenu__gParams->Ctrl.Accept.Button | PSP_CTRL_SQUARE | PSP_CTRL_RTRIGGER | PSP_CTRL_LTRIGGER
	);
	
	/* ���b�Z�[�W�������� */
	__mfmenu__gParams->Message.Main   = MF_MM_PROC; /* �ŏ��ɑ��郁�b�Z�[�W */
	__mfmenu__gParams->Message.Sub    = 0;
	__mfmenu__gParams->Message.Signal = 0;
	
	return true;
}

static bool mfmenu_init_graphics( void )
{
	/* pb���C�u���������� */
	dbgprint( "Initializing pb..." );
	pbInit();
	pbEnable( PB_NO_CACHE | PB_BLEND | PB_STEAL_DISPLAY );
	
	/* �t���[���o�b�t�@�Ƃ��Ďg�p���郁�������m�� */
	dbgprint( "Allocating memory for frame buffers..." );
	if( ! mfmenu_alloc_buffers( &(__mfmenu__gParams->FrameBuffer), ! __mfmenu__gPref.LowMemoryMode ) ) return false;
	
	/* �_�u���o�b�t�@�����O��L���� */
	if( __mfmenu__gParams->FrameBuffer.Frame[MF_MENU_FB_DRAW].Addr ){
		pbSetDrawBuffer( MF_MENU_FB_PIXELFORMAT, __mfmenu__gParams->FrameBuffer.Frame[MF_MENU_FB_DRAW].Addr, MF_MENU_FB_WIDTH );
		pbEnable( PB_DOUBLE_BUFFER );
	}
	
	pbSetDisplayBuffer( MF_MENU_FB_PIXELFORMAT, __mfmenu__gParams->FrameBuffer.Frame[MF_MENU_FB_DISP].Addr, MF_MENU_FB_WIDTH );
	
	/* �ݒ肵���t���[���o�b�t�@�ŕ`�揀�� */
	pbApply();
	if( CG_IS_ERROR( pbSyncDisplay( PB_BUFSYNC_NEXTFRAME ) ) ) return false;
	sceDisplayWaitVblankStart();
	
	/* �\���o�b�t�@�̏����� */
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
	
	/* ���݂̃t���[���o�b�t�@�����擾 */
	if( sceDisplayGetFrameBuf( &cur_fb_addr, &cur_fb_width, &cur_fb_format, PSP_DISPLAY_SETBUF_IMMEDIATE ) != 0 || ! cur_fb_addr || ! cur_fb_width ) return false;
	cur_fb_addr = (void *)( (uintptr_t)cur_fb_addr | PB_DISABLE_CACHE );
	dbgprintf( "Detect current display buffer addr: %p", cur_fb_addr );
	
	/* ���݂̃t���[���̃T�C�Y���擾 */
	cur_fb_size = pbGetFrameBufferDataSize( cur_fb_format, cur_fb_width );
	
	/* ���j���[��1�t���[���̃T�C�Y���擾 */
	fb->Size = pbGetFrameBufferDataSize( MF_MENU_FB_PIXELFORMAT, MF_MENU_FB_WIDTH );
	dbgprintf( "Frame size: %d bytes", fb->Size );
	
	/* �󂫃���������m�ۉ\�ȃt���[�������擾 */
	if( graphics_mem ){
		frame_num = sceKernelMaxFreeMemSize() / fb->Size;
		if( frame_num > 3 ) frame_num = 3;
	}
	
	dbgprintf( "Frame buffer allocated for backup: %d bytes", fb->Size * frame_num );
	if( frame_num && ! ( fb->Backup = mfMemoryAllocHi( fb->Size * frame_num ) ) ){
		frame_num = 0;
	}
	
	/* �t���[���o�b�t�@�Ƃ��Ďg��VRAM�A�h���X������ */
	fb->Frame[MF_MENU_FB_DISP].Addr  = cur_fb_addr;
	fb->Frame[MF_MENU_FB_DRAW].Addr  = (void *)( (uintptr_t)fb->Frame[MF_MENU_FB_DISP].Addr + fb->Size );
	fb->Frame[MF_MENU_FB_CLEAR].Addr = (void *)( (uintptr_t)fb->Frame[MF_MENU_FB_DRAW].Addr + fb->Size );
	if( fb->Frame[MF_MENU_FB_CLEAR].Addr >= vram_end ){
		fb->Frame[MF_MENU_FB_DRAW].Addr = vram_start;
		fb->Frame[MF_MENU_FB_CLEAR].Addr = (void *)( (uintptr_t)fb->Frame[MF_MENU_FB_DRAW].Addr + fb->Size );
	} else if( (void *)( (uintptr_t)fb->Frame[MF_MENU_FB_CLEAR].Addr + fb->Size ) >= vram_end ){
		fb->Frame[MF_MENU_FB_CLEAR].Addr = vram_start;
	}
	
	/* �o�b�N�A�b�v�\�ȃt���[�����ɉ����Ē��� */
	if( ! frame_num ){
		if( cur_fb_format != PB_PXF_8888 ) fb->Frame[MF_MENU_FB_DRAW].Addr = NULL;
		fb->Frame[MF_MENU_FB_CLEAR].Addr = NULL;
	} else{
		uintptr_t backup_offset = (uintptr_t)fb->Backup;
		
		/* break�����ɏ��ԂɃt���[���o�b�t�@�̃��X�g�A�f�[�^��ݒ� */
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
	
	/* �o�b�N�A�b�v */
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
		
		/* �N���A�p�o�b�t�@�ɘg��`�悵�Ă��� */
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
