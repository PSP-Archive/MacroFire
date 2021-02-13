/*=========================================================
	
	lang_en.h
	
	Strings for dialogs.
	
=========================================================*/
#ifndef CDIALOG_STRINGS
#define CDIALOG_STRINGS

/* common */
#define CDIALOG_STR_HELP_LABEL  "Help"
#define CDIALOG_STR_HELP_ACCEPT "Accept"
#define CDIALOG_STR_HELP_CANCEL "Cancel"
#define CDIALOG_STR_HELP_MOVE   "Move"

#define CDIALOG_STR_MESSAGE_YES "Yes"
#define CDIALOG_STR_MESSAGE_NO  "No"
#define CDIALOG_STR_MESSAGE_OK  "OK"

/* detect buttons */
#define CDIALOG_STR_DETECTBUTTONS_DESC            "Please choose one."
#define CDIALOG_STR_DETECTBUTTONS_START           "Starting to detect"
#define CDIALOG_STR_DETECTBUTTONS_CHOOSE          "Choose from list"
#define CDIALOG_STR_DETECTBUTTONS_CLEAR           "Clear current"
#define CDIALOG_STR_DETECTBUTTONS_CANCEL          "Cancel"
#define CDIALOG_STR_DETECTBUTTONS_ACCEPT          "Accept"
#define CDIALOG_STR_DETECTBUTTONS_CURRENT_BUTTONS "Current buttons:"
#define CDIALOG_STR_DETECTBUTTONS_DETECT_DESC     "Please press the button combination.\n\nWhen you want to exit the dialog,\npress the already pressed button."
#define CDIALOG_STR_DETECTBUTTONS_NOW_DETECTING   "Now detecting..."
#define CDIALOG_STR_DETECTBUTTONS_CHOOSE_EXIT     "Exit"

/* get filename */
#define CDIALOG_STR_GETFILENAME_SAVE_IN                 "Save in"
#define CDIALOG_STR_GETFILENAME_LOOK_IN                 "Look in"
#define CDIALOG_STR_GETFILENAME_FILENAME                "Filename:"
#define CDIALOG_STR_GETFILENAME_CONFIRM_CREATE_LABEL    "File not found"
#define CDIALOG_STR_GETFILENAME_CONFIRM_CREATE          "File does not exist.\nDo you want to create it?"
#define CDIALOG_STR_GETFILENAME_CONFIRM_OVERWRITE_LABEL "Overwrite"
#define CDIALOG_STR_GETFILENAME_CONFIRM_OVERWRITE       "File already exist.\nAre you sure you want to overwrite it?"
#define CDIALOG_STR_GETFILENAME_ERROR_INVALID_FILENAME  "Invalid filename"
#define CDIALOG_STR_GETFILENAME_ERROR_ILLEGAL_CHAR      "Filename contains illegal characters\n\nIllegal characters: \\/:?\"<>|"
#define CDIALOG_STR_GETFILENAME_ERROR_EMPTY             "You must provide a filename."
#define CDIALOG_STR_GETFILENAME_ERROR_SAME_AS_DIR_NAME  "Filename already exist as directory name in the current directory."
#define CDIALOG_STR_GETFILENAME_ERROR_NOT_FOUND         "File does not exist."
#define CDIALOG_STR_GETFILENAME_INPUT                   "Input filename"
#define CDIALOG_STR_GETFILENAME_EXTRA_MENU_LABEL        "Extra menu"
#define CDIALOG_STR_GETFILENAME_EXTRA_MENU              "Sorry. Not implemented yet."
#define CDIALOG_GETFILENAME_HELP_WIDTH                  41
#define CDIALOG_STR_GETFILENAME_HELP_COMMON_MOVEFOCUS   "MoveFocus"
#define CDIALOG_STR_GETFILENAME_HELP_FILELIST_LABEL     "Filelist"
#define CDIALOG_STR_GETFILENAME_HELP_FILELIST_MOVE      CDIALOG_STR_HELP_MOVE
#define CDIALOG_STR_GETFILENAME_HELP_FILELIST_MOVEPAGE  "MovePage"
#define CDIALOG_STR_GETFILENAME_HELP_FILELIST_ENTER     "Open"
#define CDIALOG_STR_GETFILENAME_HELP_FILELIST_PARENTDIR "Parent directroy"
#define CDIALOG_STR_GETFILENAME_HELP_FILELIST_EXTRAMENU CDIALOG_STR_GETFILENAME_EXTRA_MENU_LABEL " (Not implemented yet)"
#define CDIALOG_STR_GETFILENAME_HELP_FILENAME_LABEL     "Filename"
#define CDIALOG_STR_GETFILENAME_HELP_FILENAME_INPUT     "Input"

/* num edit */
#define CDIALOG_NUMEDIT_HELP_WIDTH           16
#define CDIALOG_STR_NUMEDIT_HELP_MOVE        CDIALOG_STR_HELP_MOVE
#define CDIALOG_STR_NUMEDIT_HELP_CHANGEVALUE "Change value"

/* sosk */
#define CDIALOG_STR_SOSK_CANCEL            "Are you sure you want to cancel?"
#define CDIALOG_SOSK_HELP_WIDTH            20
#define CDIALOG_STR_SOSK_HELP_MOVE         CDIALOG_STR_HELP_MOVE
#define CDIALOG_STR_SOSK_HELP_MOVECURSOR   "MoveCursor"
#define CDIALOG_STR_SOSK_HELP_INPUT        "Input"
#define CDIALOG_STR_SOSK_HELP_WHITESPACE   "Space"
#define CDIALOG_STR_SOSK_HELP_BACKSPACE    "Backspace"
#define CDIALOG_STR_SOSK_HELP_CHANGELAYOUT "Change layout"

#endif
