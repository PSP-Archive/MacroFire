/*=========================================================

	hooktable.h

	APIフックテーブル

=========================================================*/
#ifndef HOOKTABLE_H
#define HOOKTABLE_H

#include "util/hook.h"

#define MF_HPRM_PEEK_CURRENT_KEY        0
#define MF_HPRM_PEEK_LATCH              1
#define MF_HPRM_READ_LATCH              2

typedef struct {
	struct{
		void **addr;
		void *api;
	} restore;
	char *module;
	char *library;
	unsigned int nid;
	void *hook;
} MfHookEntry;

MfHookEntry hooktable[] = {
	{ { NULL }, "sceHP_Remote_Driver", "sceHprm", 0x1910B327, mfHprmPeekCurrentKey },
	{ { NULL }, "sceHP_Remote_Driver", "sceHprm", 0x2BCEC83E, mfHprmPeekLatch },
	{ { NULL }, "sceHP_Remote_Driver", "sceHprm", 0x40D2F9F0, mfHprmReadLatch },
};

unsigned int hooktableEntryCount = sizeof( hooktable ) / sizeof( MfHookEntry );

#endif
