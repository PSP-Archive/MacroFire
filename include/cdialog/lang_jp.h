/*=========================================================
	
	lang_jp.h
	
	ダイアログ内で使用する文字列定数。
	
=========================================================*/
#ifndef CDIALOG_STRINGS
#define CDIALOG_STRINGS

/* 共通 */
#define CDIALOG_STR_HELP_LABEL  "ヘルプ"
#define CDIALOG_STR_HELP_ACCEPT "決定"
#define CDIALOG_STR_HELP_CANCEL "キャンセル"
#define CDIALOG_STR_HELP_MOVE   "移動"

#define CDIALOG_STR_MESSAGE_YES "はい"
#define CDIALOG_STR_MESSAGE_NO  "いいえ"
#define CDIALOG_STR_MESSAGE_OK  "OK"

/* ボタン検出 */
#define CDIALOG_STR_DETECTBUTTONS_DESC            "1つ選んでください"
#define CDIALOG_STR_DETECTBUTTONS_START           "ボタン検出開始"
#define CDIALOG_STR_DETECTBUTTONS_CHOOSE          "リストから選択"
#define CDIALOG_STR_DETECTBUTTONS_CLEAR           "現在のボタンをクリア"
#define CDIALOG_STR_DETECTBUTTONS_CANCEL          "キャンセル"
#define CDIALOG_STR_DETECTBUTTONS_ACCEPT          "決定"
#define CDIALOG_STR_DETECTBUTTONS_CURRENT_BUTTONS "現在のボタン:"
#define CDIALOG_STR_DETECTBUTTONS_DETECT_DESC     "組み合わせたいボタンを押してください。\n\n既に検出されているボタンをもう一度押すと終了します。"
#define CDIALOG_STR_DETECTBUTTONS_NOW_DETECTING   "検出中..."
#define CDIALOG_STR_DETECTBUTTONS_CHOOSE_EXIT     "終了"

/* ファイル名取得 */
#define CDIALOG_STR_GETFILENAME_SAVE_IN                 "セーブ"
#define CDIALOG_STR_GETFILENAME_LOOK_IN                 "ロード"
#define CDIALOG_STR_GETFILENAME_FILENAME                "ファイル名:"
#define CDIALOG_STR_GETFILENAME_CONFIRM_CREATE_LABEL    "作成確認"
#define CDIALOG_STR_GETFILENAME_CONFIRM_CREATE          "指定されたファイルが見つかりません。\n作成しますか?"
#define CDIALOG_STR_GETFILENAME_CONFIRM_OVERWRITE_LABEL "上書き確認"
#define CDIALOG_STR_GETFILENAME_CONFIRM_OVERWRITE       "指定されたファイル名は既に存在します。\n上書きしますか?"
#define CDIALOG_STR_GETFILENAME_ERROR_INVALID_FILENAME  "不正なファイル名"
#define CDIALOG_STR_GETFILENAME_ERROR_ILLEGAL_CHAR      "使用できない文字が含まれています。\n\n使用できない文字: \\/:?\"<>|"
#define CDIALOG_STR_GETFILENAME_ERROR_EMPTY             "ファイル名を入力してください。"
#define CDIALOG_STR_GETFILENAME_ERROR_SAME_AS_DIR_NAME  "指定されたファイル名は、既にディレクトリ名として使用されています。"
#define CDIALOG_STR_GETFILENAME_ERROR_NOT_FOUND         "ファイルが見つかりません。"
#define CDIALOG_STR_GETFILENAME_INPUT                   "ファイル名"
#define CDIALOG_STR_GETFILENAME_EXTRA_MENU_LABEL        "エクストラメニュー"
#define CDIALOG_STR_GETFILENAME_EXTRA_MENU              "未実装です。"
#define CDIALOG_GETFILENAME_HELP_WIDTH                  28
#define CDIALOG_STR_GETFILENAME_HELP_COMMON_MOVEFOCUS   "フォーカスの移動"
#define CDIALOG_STR_GETFILENAME_HELP_FILELIST_LABEL     "ファイルリスト"
#define CDIALOG_STR_GETFILENAME_HELP_FILELIST_MOVE      CDIALOG_STR_HELP_MOVE
#define CDIALOG_STR_GETFILENAME_HELP_FILELIST_MOVEPAGE  "ページ移動"
#define CDIALOG_STR_GETFILENAME_HELP_FILELIST_ENTER     "開く"
#define CDIALOG_STR_GETFILENAME_HELP_FILELIST_PARENTDIR "1つ上のディレクトリ"
#define CDIALOG_STR_GETFILENAME_HELP_FILELIST_EXTRAMENU CDIALOG_STR_GETFILENAME_EXTRA_MENU_LABEL " (未実装)"
#define CDIALOG_STR_GETFILENAME_HELP_FILENAME_LABEL     "ファイル名"
#define CDIALOG_STR_GETFILENAME_HELP_FILENAME_INPUT     "入力"

/* 数値編集 */
#define CDIALOG_NUMEDIT_HELP_WIDTH           16
#define CDIALOG_STR_NUMEDIT_HELP_MOVE        CDIALOG_STR_HELP_MOVE
#define CDIALOG_STR_NUMEDIT_HELP_CHANGEVALUE "値の変更"

/* 簡易OSK */
#define CDIALOG_STR_SOSK_CANCEL            "入力を中断しますか?"
#define CDIALOG_SOSK_HELP_WIDTH            20
#define CDIALOG_STR_SOSK_HELP_MOVE         CDIALOG_STR_HELP_MOVE
#define CDIALOG_STR_SOSK_HELP_MOVECURSOR   "カーソ\ルの移動"
#define CDIALOG_STR_SOSK_HELP_INPUT        "入力"
#define CDIALOG_STR_SOSK_HELP_WHITESPACE   "スペース"
#define CDIALOG_STR_SOSK_HELP_BACKSPACE    "削除"
#define CDIALOG_STR_SOSK_HELP_CHANGELAYOUT "文字レイアウト変更"

#endif
