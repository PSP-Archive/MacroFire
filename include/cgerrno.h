/*=========================================================
	
	cgerrno.h
	
	ClassGエラーコード
	
=========================================================*/
#ifndef CGERRNO_H
#define CGERRNO_H

/*-----------------------------------------------
	成功
-----------------------------------------------*/
#define CG_ERROR_OK 0
#define CG_OK       CG_ERROR_OK

/*-----------------------------------------------
	エラーマクロ
-----------------------------------------------*/
/* 成功したか調べる */
#define CG_IS_OK( err ) ( ( err ) >= CG_OK )

/* エラーかどうか調べる */
#define CG_IS_ERROR( err ) ( ( err ) < CG_OK )

/* エラーコードがSCE APIからのエラーかどうか調べる */
#define CG_IS_SCE_ERROR( err ) ( (( err ) & 0xF0000000 ) == 0x80000000 )

/* エラーがCGERRNOであるか調べる */
#define CG_IS_CG_ERROR( err )  ( (( err ) & 0xF0000000 ) == 0xE0000000 )

/* 各種ライブラリが返す*UID、およびSCEのAPIが返すSceUIDがが有効な値かどうか調べる */
#define CG_IS_VALID_UID( uid ) ( (int)( uid ) > CG_OK )

/*-----------------------------------------------
	汎用エラー
-----------------------------------------------*/
/* 何らかのエラー */
#define CG_ERROR_UNKNOWN                 0xE001FFFF

/* 不正な引数 */
#define CG_ERROR_INVALID_ARGUMENT        0xE0010001

/* メモリが不足している */
#define CG_ERROR_NOT_ENOUGH_MEMORY       0xE0010002

/* バッファが不足している */
#define CG_ERROR_OUT_OF_BUFFER           0xE0010003

/* 許可されていない操作を行った */
#define CG_ERROR_OPERATION_NOT_PERMITTED 0xE0010004

/* 存在していない */
#define CG_ERROR_NOT_FOUND               0xE0010005

/* 存在している */
#define CG_ERROR_ALREADY_EXIST           0xE0010006

/* 読み込み禁止 */
#define CG_ERROR_NOT_READABLE            0xE0010009

/* 書き込み禁止 */
#define CG_ERROR_NOT_WRITABLE            0xE001000A

/* ディレクトリである */
#define CG_ERROR_IS_DIRECTORY            0xE001000B

/* ディレクトリではない */
#define CG_ERROR_NOT_DIRECTORY           0xE001000C

/* 既に実行されている */
#define CG_ERROR_ALREADY_RUNNING         0xE001000D

/* まだ実行されていない */
#define CG_ERROR_NOT_RUNNING_YET         0xE001000E

/* 既に初期化されている */
#define CG_ERROR_ALREADY_INITIALIZED     0xE001000F

/* まだ初期化されていない */
#define CG_ERROR_NOT_INITIALIZED_YET     0xE0010010

/* 既に開いている */
#define CG_ERROR_ALREADY_OPENED          0xE0010011

/* まだ開いていない */
#define CG_ERROR_NOT_OPENED_YET          0xE0010012

/* 機能は未実装 */
#define CG_ERROR_NOT_IMPLEMENTED_YET     0xE0010013


/*-----------------------------------------------
	SCE API エラー
-----------------------------------------------*/
#define CG_ERROR_FAILED_TO_CREATE_SEMA 0xE0020001
#define CG_ERROR_FAILED_TO_GETSTAT     0xE0020002
#define CG_ERROR_FAILED_TO_OPEN        0xE0020004
#define CG_ERROR_FAILED_TO_READ        0xE0020005
#define CG_ERROR_FAILED_TO_WRITE       0xE0020006
#define CG_ERROR_FAILED_TO_SEEK        0xE0020007
#define CG_ERROR_FAILED_TO_DOPEN       0xE0020009
#define CG_ERROR_FAILED_TO_DREAD       0xE002000A

/*-----------------------------------------------
	Inimgrエラー
-----------------------------------------------*/
/* シグネチャが一致しない */
#define CG_ERROR_INI_SIGNATURE_MISMATCH    0xE0030001

/* サポートしていないバージョン */
#define CG_ERROR_INI_VERSION_NOT_SUPPORTED 0xE0030002
	
/* 指定されたセクションは存在しない */
#define CG_ERROR_INI_SECTION_NOT_FOUND     0xE0030003

/* 指定されたキーは存在しない */
#define CG_ERROR_INI_KEY_NOT_FOUND         0xE0030004

/* 指定されたキーのデータは、読み出そうとしたデータ型に適合していない */
#define CG_ERROR_INI_INVALID_VALUE         0xE0030005


#endif
