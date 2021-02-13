/* mfMenuCreateTables.c */

#include "mfmenu_types.h"

MfMenuTable *mfMenuCreateTables( unsigned int tables, ... )
{
	/*
		指定されたテーブルを全てメモリに一度に割り当てる。
		
		可変引数は、必ず二つで一組で、「テーブルの行/テーブルの列」を渡す。
		    mfMenuCreateTable(
		    	2,
		    	3, 1,
		    	5, 2
		    )；
		これは、二つのテーブルが存在し、その二つのテーブルはそれぞれ行/列が以下のようになるように割り当てられる。
		    テーブル1: 行3 列1
		    テーブル2: 行5 列2
		テーブル数が一致しない、引数を二つ一組で渡さない場合の動作は予測できない。
	*/
	
	va_list ap;
	MfMenuTable *table;
	SceSize allocsize;
	unsigned int i, j, k;
	struct {
		unsigned int rows;
		unsigned int cols;
	} tablecells[16];
	uintptr_t offset;
	
	if( tables >= 16 ) return NULL;
	
	allocsize = tables * sizeof( MfMenuTable );
	
	va_start( ap, tables );
	for( i = 0; i < tables; i++ ){
		tablecells[i].rows = va_arg( ap, int );
		tablecells[i].cols = va_arg( ap, int );
		
		dbgprintf( "Table size: %d, %d", tablecells[i].rows, tablecells[i].cols );
		
		allocsize += sizeof( MfMenuEntry * ) * tablecells[i].rows + sizeof( MfMenuEntry ) * tablecells[i].rows * tablecells[i].cols;
		if( tablecells[i].cols > 1 ) allocsize += sizeof( unsigned int ) * tablecells[i].cols;
	}
	va_end( ap );
	
	dbgprintf( "Allocating memory for menu-tables: %d bytes", allocsize );
	table = (MfMenuTable *)mfMenuMemoryAlloc( allocsize );
	if( ! table ){
		dbgprint( "Failed to allocate memory" );
		return NULL;
	}
	
	offset = (uintptr_t)table + sizeof( MfMenuTable ) * tables;
	for( i = 0; i < tables; i++ ){
		table[i].label = NULL;
		table[i].x     = 0;
		table[i].y     = 0;
		table[i].rows  = tablecells[i].rows;
		table[i].cols  = tablecells[i].cols;
		if( table[i].cols > 1 ){
			unsigned int size = sizeof( unsigned int ) * table[i].cols;
			table[i].colsWidth = (unsigned int *)offset;
			memset( table[i].colsWidth, 0, size );
			offset += size;
		} else{
			table[i].colsWidth = NULL;
		}
		
		table[i].entry = (MfMenuEntry **)offset;
		offset += sizeof( MfMenuEntry * ) * table[i].rows;
		for( j = 0; j < table[i].rows; j++ ){
			table[i].entry[j] = (MfMenuEntry *)offset;
			for( k = 0; k < table[i].cols; k++ ){
				table[i].entry[j][k].active = false;
				table[i].entry[j][k].ctrl   = NULL;
				table[i].entry[j][k].label  = "";
				table[i].entry[j][k].var    = NULL;
				table[i].entry[j][k].arg    = NULL;
				offset += sizeof( MfMenuEntry );
			}
		}
	}
	return table;
}
