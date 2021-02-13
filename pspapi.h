/*==========================================================

	pspapi.h

	PSPSDK�Ƀv���g�^�C�v���Ȃ�API�B

==========================================================*/
#ifndef PSPAPI_H
#define PSPAPI_H

#include <pspkernel.h>

/*-----------------------------------------------
	sceDisplayEnable
	sceDisplayDisable
	
	LCD�̓d����ON/OFF��؂�ւ���悤���H
-----------------------------------------------*/
void sceDisplayEnable( void );
//void sceDisplayDisable( void );

/*-----------------------------------------------
	sceDmacMemcpy
	sceDmacTryMemcpy (async(?))
	
	�w�i�̂悤�ȑ傫�ȃ������u���b�N�̓]���ɂ�DMA�R���g���[�����g�p�B
	�א؂�̃f�[�^�ɑ΂��Ă͋t�ɒx���Ȃ�Ƃ̏�񂪂������̂Ŕw�i�]���ɂ̂ݎg���B
	�{���ɒx���̂��ǂ����͒��ׂĂ��Ȃ��B
	
	�L���b�V���̊֌W�Ŗ��d�s�v�c�ȃo�O�ɔY�܂�邱�Ƃ�������ۂ��B
	�ʏ��memcpy()���g���A�f�[�^�����������Ă����̂Ȃ��w�i�]���ȂǂɎg������������H
-----------------------------------------------*/
int sceDmacMemcpy( void *dest, const void *src, SceSize size );
//int sceDmacTryMemcpy( void *dest, const void *src, SceSize size );

/*-----------------------------------------------
	sceSysregAudioIoEnable
	sceSysregAudioIoDisable
	
	�I�[�f�B�I���o�̗͂L����/�������B
	�ǂ����pspSdkSetK1( 0 )���s��łȂ��ƕs�v�c�ȓ��������B
-----------------------------------------------*/
int sceSysregAudioIoEnable( int unk );
int sceSysregAudioIoDisable( int unk );

/*-----------------------------------------------
	SceSysconPacket
	SceSysconDebugHandlers
	SceSysconCtrlButtons
	
	sceSysconSetDebugHandlers
	
	Booster����PSP VSH Extender / SYSCON override example 070910 by Booster���B
	
	http://www.maxconsole.net/showthread.php?78436-PSP-VSH-extender-SYSCON-override-example-070910-by-Booster
-----------------------------------------------*/
#define SCE_SYSCON_CMD_GET_CONTROLLER_DIGITAL 0x07
#define SCE_SYSCON_CMD_GET_CONTROLLER_ANALOG  0x08

typedef struct SceSysconPacket
{
	u8	unk00[4];		// +0x00 ?(0x00,0x00,0x00,0x00)
	u8	unk04[2];		// +0x04 ?(arg2)
	u8	status;			// +0x06
	u8	unk07;			// +0x07 ?(0x00)
	u8	unk08[4];		// +0x08 ?(0xff,0xff,0xff,0xff)
// transmit data
	u8	tx_cmd;			// +0x0C command code
	u8	tx_len;			// +0x0D number of transmit bytes
	u8	tx_data[14];	// +0x0E transmit parameters
// receive data
	u8	rx_sts;			// +0x1C generic status
	u8	rx_len;			// +0x1D receive length
	u8	rx_response;	// +0x1E response code(tx_cmd or status code)
	u8	rx_data[9];		// +0x1F receive parameters
// ?
	u32	unk28;			// +0x28
// user callback (when finish an access?)
	void (*callback)(struct SceSysconPacket *,u32);	// +0x2c
	u32	callback_r28;	// +0x30
	u32	callback_arg2;	// +0x34 arg2 of callback (arg4 of sceSycconCmdExec)

	u8	unk38[0x0d];	// +0x38
	u8	old_sts;		// +0x45 old     rx_sts
	u8	cur_sts;		// +0x46 current rx_sts
	u8	unk47[0x21];	// +0x47
// data size == 0x60 ??
} SceSysconPacket;

typedef void ( *SceSysconDebugHandler )( SceSysconPacket* );

typedef struct SceSysconDebugHandlers {
	SceSysconDebugHandler handler1;  // �Ă΂�Ă��Ȃ��͗l�BNULL�ł�����?
	SceSysconDebugHandler before_tx; // Syscon�R�}���h�����s�����O�ɌĂ΂��B
	SceSysconDebugHandler after_rx;  // Syscon�R�}���h�����s���ꂽ��ɌĂ΂��B
	SceSysconDebugHandler handler4;  // �Ă΂�Ă��Ȃ��͗l�BNULL�ł�����?
} SceSysconDebugHandlers;

enum SceSysconCtrlButtons {
	SCE_SYSCON_CTRL_SELECT       = 0x00000100,
	SCE_SYSCON_CTRL_START        = 0x00000800,
	
	SCE_SYSCON_CTRL_LTRIGGER     = 0x00000200,
	SCE_SYSCON_CTRL_RTRIGGER     = 0x00000400,
	
	SCE_SYSCON_CTRL_TRIANGLE     = 0x00000010,
	SCE_SYSCON_CTRL_CIRCLE       = 0x00000020,
	SCE_SYSCON_CTRL_CROSS        = 0x00000040,
	SCE_SYSCON_CTRL_SQUARE       = 0x00000080,
	
	SCE_SYSCON_CTRL_UP           = 0x00000001,
	SCE_SYSCON_CTRL_RIGHT        = 0x00000002,
	SCE_SYSCON_CTRL_DOWN         = 0x00000004,
	SCE_SYSCON_CTRL_LEFT         = 0x00000008,
	SCE_SYSCON_CTRL_HOME         = 0x00001000,
	SCE_SYSCON_CTRL_HOLD         = 0x00002000,
	SCE_SYSCON_CTRL_WLAN_UP      = 0x00004000,
	SCE_SYSCON_CTRL_VOLUP        = 0x00010000,
	SCE_SYSCON_CTRL_VOLDOWN      = 0x00020000,
	SCE_SYSCON_CTRL_SCREEN       = 0x00040000,
	SCE_SYSCON_CTRL_NOTE         = 0x00080000,
	
	SCE_SYSCON_CTRL_REMOTE_EJECT = 0x00008000,
	SCE_SYSCON_CTRL_DISC_EJECT   = 0x00100000,
	SCE_SYSCON_CTRL_MS_EJECT     = 0x00200000,
	
	/* ���܂�r�b�g�R�[�h�B���m�̃{�^���R�[�h���̂Ă�Ƃ܂������Ȃ̂ł���Œ��o���ăZ�b�g */
	SCE_SYSCON_CTRL_UNK_BUTTONS  = 0xFFC00000
} SceSysconCtrlButtons;

void sceSysconSetDebugHandlers( SceSysconDebugHandlers *handlers );

#endif
