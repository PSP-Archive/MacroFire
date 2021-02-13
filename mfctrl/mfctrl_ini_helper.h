/* mfctrl_ini_helper.h */

#define MF_CTRL_DEF_INI_SIG_ACCEPT MF_SM_1
#define MF_CTRL_DEF_INI_SIG_FINISH MF_SM_2

struct __mf_ctrl_ini_helper_params {
	MfCtrlDefIniProc proc;
	MfCtrlDefIniPref *pref;
	MfCtrlDefIniAction action;
	char *path;
};

void __mf_ctrl_ini_helper_handler( MfMessage message, void *arg );
