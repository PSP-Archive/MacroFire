/*=========================================================

	mfmain.h

	MacroFire ���C�����[�v

=========================================================*/
#ifndef MFMAIN_H
#define MFMAIN_H

#define MF_FIRST_INCLUDE
#include "macrofire.h"
#undef MF_FIRST_INCLUDE

/*=========================================================
	�f�t�H���g�ݒ�l
=========================================================*/
#define MF_INI_STARTUP             false
#define MF_INI_MENU_BUTTONS        ( PSP_CTRL_VOLUP | PSP_CTRL_VOLDOWN )
#define MF_INI_TOGGLE_BUTTONS      ( 0 )
#define MF_INI_STATUS_NOTIFICATION false

/*=========================================================
	�^�錾
=========================================================*/
enum MfPbpOffset {
	PARAM_SFO = 0,
	ICON0_PNG,
	ICON1_PMF,
	ICON1_PNG,
	PIC1_PNG,
	SND0_AT3,
	DATA_PSP,
	DATA_PSAR
};

struct MfPbpHeader {
	char Signature[4]; // "\0PBP"
	uint32_t Version;
	uint32_t Offset[8];
};

struct MfPsfHeader {
	char Signature[4]; // "\0PSF"
	uint32_t Version;
	uint32_t KeyTableOffset;
	uint32_t ValueTableOffset;
	uint32_t KeysCount;
};

struct MfPsfSection {
	uint16_t KeyOffset;
	uint8_t  Unk; // ���4?
	uint8_t  ValueType;
	uint32_t ValueLength;
	uint32_t ValueMaxLength;
	uint32_t ValueOffset;
};

/*=========================================================
	�v���g�^�C�v
=========================================================*/
int mfHprmPeekCurrentKey( u32 *key );
int mfHprmPeekLatch( u32 *latch );
int mfHprmReadLatch( u32 *latch );
void mfSysconSetDebugHandlers( SceSysconDebugHandlers *handlers );

/*=======================================================*/

#include "hooktable.h"

#endif
