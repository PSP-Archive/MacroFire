/* ovmsg_types.h                     */
/* 内部で使用。外から参照しない。 */

#include "../ovmsg.h"

#include <stdio.h>
#include <string.h>
#include "memory/memory.h"
#include "graphic/pb.h"
#include "cgerrno.h"

/*=========================================================
	型宣言
=========================================================*/

struct ovmsg_params {
	SceUID selfThreadId;
	int ctrlEvId, workEvId;
	
	unsigned int x, y, w, h;
	unsigned int fgcolor;
	unsigned int bgcolor;
	
	char *message;
	
	time_t displayStart;
	time_t displaySec;
	
	unsigned int flags;
	void *fb;
	int bufwidth;
	enum PspDisplayPixelFormats pxformat;
};

enum ovmsg_event_flag {
	OVMSG_EVENT_OK       = 0x00000001,
	OVMSG_EVENT_PRINT    = 0x00000002,
	OVMSG_EVENT_SUSPEND  = 0x00000004,
	OVMSG_EVENT_SHUTDOWN = 0x00000008
};
