/*=========================================================
	
	inimgr.h
	
	INIファイルマネージャ。
	
=========================================================*/
#ifndef INIMGR_H
#define INIMGR_H

#include <pspkernel.h>
#include <stdbool.h>

/*=========================================================
	定数/マクロ
=========================================================*/
#define INIMGR_ENTRY_LENGTH   255
#define INIMGR_SECTION_LENGTH 32

/*=========================================================
	オプション
=========================================================*/
#define INIMGR_CREATE ( FH_O_RDONLY )
#define INIMGR_SAVE ( FH_O_RDWR | FH_O_CREAT )

/*=========================================================
	型宣言
=========================================================*/
typedef intptr_t InimgrUID;
typedef intptr_t InimgrContext;
typedef uint32_t InimgrVersion;

typedef enum {
	INIMGR_OP_LOAD,
	INIMGR_OP_SAVE
} InimgrOpMode;

typedef int ( *InimgrCallback )( InimgrContext, void* );

/*=========================================================
	関数
=========================================================*/
/*-----------------------------------------------
	inimgrNew
	
	新しいINIマネージャを生成する。
	
	@return: InimgrUID
		他の関数で使うUID。
-----------------------------------------------*/
InimgrUID inimgrNew( void );

/*-----------------------------------------------
	inimgrDestroy
	
	INIマネージャを削除する。
	
	@param: InimgrUID uid
		inimgrNew()で得たUID。
	
	@return: int
		= 0: 成功
		< 0: エラー
-----------------------------------------------*/
int inimgrDestroy( InimgrUID uid );

/*-----------------------------------------------
	inimgrMakeVersion
	
	inimgrLoad()やinimgrSave()、inimgrMerge()で使うバージョン番号を生成する。
	これで生成したInimgrVersionは、単純に == != < =< > >= といった演算子で比較可能。
	
	
-----------------------------------------------*/
InimgrVersion inimgrMakeVersion( unsigned char major, unsigned char minor, unsigned char rev );

/*-----------------------------------------------
	inimgrRegisterCallback
	
	inimgrLoadSection()/inimgrSave()/inimgrMerge()で呼び出されるコールバック関数。
	セクション内に大量のデータがある場合などに使用し、ファイルから直接データを呼んでメモリを節約する。
	
	コールバックセクション内であっても key = value の形式以外で読み書きすることはできない。
	
	inimgrLoadSection()時に、コールバックが設定されたセクションを読み出そうとすると、コールバックされる。
	inimgrSave()/inimgrMerge()では、コールバックが設定されたセクションを書き出そうとしたときにコールバックされる。
	
	コールバックされた関数は、inimgrReadEntry()/inimgrWriteEntry()を使ってデータを読み出したり書き出したりする。
	
	対象となるセクション名の指定にはワイルドカード( ? * )が使用できる。
	マッチするセクションでは、全てコールバックされる。
	
	@param: InimgrUID uid
		inimgrNew()で得たUID。
	
	@param: const char *wildcard
		コールバックさせるセクション名。
		ワイルドカードを使用可能。
	
	@param: InimgrCallback cb
		コールバック関数。
	
	@param: void *arg
		コールバック関数に渡される引数。
		セットされた値がそのまま渡される。
	
	@return: int
		= 0: 成功
		< 0: エラー
-----------------------------------------------*/
int inimgrRegisterCallback( InimgrUID uid, const char *wildcard, InimgrCallback cb, void *arg );

/*-----------------------------------------------
	inimgrLoad
	
	INIファイルをロードする準備を行う。
	この関数でINIファイルが完全にロードされるわけではない。
	これにより各セクションのファイル内でのオフセットを取得する。
	
	実際にデータを使用するには、さらにinimgrLoadSection()で必要なセクションを読み出さなければいけない。
	
	@param: InimgrUID uid
		inimgrNew()で得たUID。
	
	@param: const char *path
		INIファイルのパス。
	
	@param: const char *sig
		チェックするシグネチャ。
		一致しなければ、CG_ERROR_INI_SIGNATURE_MISMATCH を返す。
	
	@param: InimgrVersion version
		inimgrMakeVersion()の返り値。
		有効なバージョンを指定する。
		指定したバージョンよりも、INIファイルのバージョンが古ければ
		CG_ERROR_INI_VERSION_NOT_SUPPORTED を返す。
	
	@return int
		= 0: 成功
		< 0: エラー
-----------------------------------------------*/
int inimgrLoad( InimgrUID uid, const char *path, const char *sig, InimgrVersion version );

/*-----------------------------------------------
	inimgrLoadSection
	
	指定したセクションをロードして、読み書き可能な状態にする。
	inimgrLoad()は、単にセクションのオフセットを記録しているだけなので、
	この関数でエントリを読み出さなければ inimgrGet* / inimgrSet* は行えない。
-----------------------------------------------*/
int inimgrLoadSection( InimgrUID uid, const char *name );

/*-----------------------------------------------
	inimgrLoadAllSections
	
	inimgrLoadSection()を存在するすべてのセクションに対して行う。
-----------------------------------------------*/
int inimgrLoadAllSections( InimgrUID uid );

/*-----------------------------------------------
	inimgrAddSection
	
	新しいセクションを作成する。
	inimgrSet*は、存在しないセクションへ値をセットすることは出来ない。
	自動的にセクションは作成されないため、自分で作成する必要がある。
	
	すでに存在しているセクションを作成しようとした場合は何もせず成功を返す。
-----------------------------------------------*/
int inimgrAddSection( InimgrUID uid, const char *name );

/*-----------------------------------------------
	inimgrSave
	
	メモリ上のデータをINIファイルとして書き出す。
	指定したパスへ、メモリ上のデータを単に上書きする。
	読み出されなかったセクションは空になってしまう。
-----------------------------------------------*/
int inimgrSave( InimgrUID uid, const char *path, const char *sig, InimgrVersion version );

/*-----------------------------------------------
	inimgrMerge (未実装)
	
	メモリ上のデータと、既存のINIファイルをマージする。
	メモリ上のデータが優先される。
	
	マージするには、一度一時ファイルに書き出す必要があるため、一時ファイルのパスを引数に必要とする。
-----------------------------------------------*/
int inimgrMerge( InimgrUID uid, const char *path, const char *tempfile, const char *sig, InimgrVersion version );

/*-----------------------------------------------
	inimgrReadEntry
	
	エントリを一つ読み出す。
-----------------------------------------------*/
bool inimgrReadEntry( InimgrContext context, char **key, char **value );

/*-----------------------------------------------
	inimgrWriteEntry
	
	エントリを一つ書き出す。
-----------------------------------------------*/
bool inimgrWriteEntry( InimgrContext context, const char *key, const char *value );

/*-----------------------------------------------
	inimgrGetCurrentUID
	
	現在のInimgrUIDを返す。
-----------------------------------------------*/
InimgrUID inimgrGetCurrentUID( InimgrContext context );

/*-----------------------------------------------
	inimgrGetCurrentOpMode
	
	現在のオプモードを返す。
-----------------------------------------------*/
InimgrOpMode inimgrGetCurrentOpMode( InimgrContext context );

int inimgrGetInt( InimgrUID uid, const char *name, const char *key, int *res );
int inimgrGetString( InimgrUID uid, const char *name, const char *key, char *res, size_t reslen );
int inimgrGetBool( InimgrUID uid, const char *name, const char *key, bool *res );
int inimgrSetInt( InimgrUID uid, const char *name, const char *key, int32_t integer );
int inimgrSetString( InimgrUID uid, const char *name, const char *key, const char *str );
int inimgrSetBool( InimgrUID uid, const char *name, const char *key, bool bl );


#endif
