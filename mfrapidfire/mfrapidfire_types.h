/* mfrapidfire_types.h */

#include "../mfrapidfire.h"

struct mf_rapidfire_progress {
	unsigned int button;
	unsigned int mode;
};

enum mfrapidfire_flags_hold {
	MF_RAPIDFIRE_FLAGS_HOLD_ENABLE     = 0x00000001,
	MF_RAPIDFIRE_FLAGS_HOLD_ISREALHOLD = 0x00000002
};

enum mfrapidfire_flags_rapid {
	MF_RAPIDFIRE_FLAGS_RAPID_PRESS = 0x00000001,
};
