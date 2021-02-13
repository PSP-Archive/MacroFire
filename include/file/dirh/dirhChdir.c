/* dirhChdir.c */

#include "dirh_types.h"
#include "util/strutil.h"

#define DIRH_SCE_INVALID_ARGUMENT 0x80010016

static struct dirh_thread_dopen_params st_thdopen;

static int dirh_parse_dirent( struct dirh_data *data, SceUID evid );
static int dirh_thread_parse( struct dirh_thread_dopen_params *thdopen, struct dirh_data *data, SceUInt timeout );
static int dirh_dopen_thread_main( SceSize arglen, void *argp );

int dirhChdir( DirhUID uid, const char *dirpath, SceUInt timeout )
{
	struct dirh_params *params = (struct dirh_params *)uid;
	SceIoStat stat = { 0 };
	int ret;
	
	/* パスを展開 */
	strutilCopy( params->data.cwd.path, dirpath, params->data.cwd.max );
	makepath( "ms0:/", params->data.cwd.path, params->data.cwd.max );
	
	/*
		末尾が':'で終わっていない場合は、パスをチェック。
		sceIoGetstat()は、引数にドライブのルートを示す("ms0:/" のような)パス指定しても80010016(不正な引数?)が返る。
		
		ただし、存在しないドライブ名を渡した場合は、80020321(存在しないデバイス)が返るので、これだけチェックする。
	*/
	if( *(strchr( params->data.cwd.path, '\0' ) - 1) != ':' ){
		/* 指定されたパスの情報を取得 */
		if( ( ret = sceIoGetstat( params->data.cwd.path, &stat ) ) < 0 ){
			return ret;
		} else if( ! FIO_S_ISDIR( stat.st_mode ) ){
			return CG_ERROR_NOT_DIRECTORY;
		}
	} else if( ( ret = sceIoGetstat( params->data.cwd.path, &stat ) ) != DIRH_SCE_INVALID_ARGUMENT ){
		return ret;
	}
	
	__dirh_clear_entries( &(params->data.entry) );
	
	if( timeout ){
		int ret;
		sceKernelWaitSema( params->semaId, 1, 0 );
		ret = dirh_thread_parse( &st_thdopen, &(params->data), timeout );
		sceKernelSignalSema( params->semaId, 1 );
		return ret;
	} else{
		return dirh_parse_dirent( &(params->data), 0 );
	}
}

void __dirh_clear_entries( struct dirh_entry *entry )
{
	if( entry->list ){
		memoryFree( entry->list );
		entry->list  = NULL;
		entry->count = 0;
		entry->pos   = 0;
	}
}

static int dirh_parse_dirent( struct dirh_data *data, SceUID evid )
{
	SceUID fd;
	SceIoDirent dirent;
	size_t require_memsize;
	
	char *namebuf;
	unsigned int i;
	int ret;
	
	fd = sceIoDopen( data->cwd.path );
	if( ! fd ) return CG_ERROR_FAILED_TO_DOPEN;
	
	memset( &dirent, 0, sizeof( SceIoDirent ) );
	
	if( evid ){
		unsigned int evbits = 0;
		sceKernelPollEventFlag( evid, DIRH_EVENT_EXIT, PSP_EVENT_WAITOR, &evbits );
		if( evbits & DIRH_EVENT_EXIT ) return CG_ERROR_OK;
	}
	
	require_memsize = 0;
	while( ( ret = sceIoDread( fd, &dirent ) ) > 0 ){
		if( ( dirent.d_stat.st_attr & ( FIO_SO_IFREG | FIO_SO_IFDIR ) ) && strcmp( dirent.d_name, "." ) != 0 ){
			require_memsize += strlen( dirent.d_name ) + 1;
			data->entry.count++;
		}
	}
	sceIoDclose( fd );
	
	fd = sceIoDopen( data->cwd.path );
	if( ! fd ) return CG_ERROR_FAILED_TO_DOPEN;
	
	data->entry.list = (DirhFileInfo *)memoryAlloc( sizeof( DirhFileInfo ) * data->entry.count + require_memsize );
	if( ! data->entry.list ) return CG_ERROR_NOT_ENOUGH_MEMORY;
	namebuf     = (char *)((uintptr_t)(data->entry.list) + sizeof( DirhFileInfo ) * data->entry.count);
	
	/* ディレクトリリストを作成 */
	i = 0;
	while( ( ret = sceIoDread( fd, &dirent ) ) > 0 ){
		if( strcmp( dirent.d_name, "." ) != 0 && i < data->entry.count ){
			data->entry.list[i].type = dirent.d_stat.st_attr & ( DIRH_FILE | DIRH_DIR );
			if( ! data->entry.list[i].type ) continue;
			
			data->entry.list[i++].name = namebuf;
			
			strcpy( namebuf, dirent.d_name );
			namebuf += strlen( dirent.d_name ) + 1;
		}
	}
	
	sceIoDclose( fd );
	
	return CG_ERROR_OK;
}

static int dirh_thread_parse( struct dirh_thread_dopen_params *thdopen, struct dirh_data *data, SceUInt timeout )
{
	SceUID thid;
	int ret;
	
	if( CG_IS_VALID_UID( thdopen->eventId ) ) return CG_ERROR_ALREADY_RUNNING;
	
	thdopen->eventId = sceKernelCreateEventFlag( "DirhThEvId", 0, 0, NULL );
	if( CG_IS_ERROR( thdopen->eventId ) ) return thdopen->eventId;
	
	thdopen->data = data;
	thdopen->status = 0;
	
	thid = sceKernelCreateThread( "DirhThDopen", dirh_dopen_thread_main, 32, 0x600, 0, NULL );
	if( ! CG_IS_VALID_UID( thid ) ) return thid;
	
	ret = sceKernelStartThread( thid, sizeof( struct dirh_thread_dopen_params * ), &thdopen );
	if( CG_IS_ERROR( ret ) ) return ret;
	
	if( CG_IS_ERROR( ret = sceKernelWaitEventFlag( thdopen->eventId, DIRH_EVENT_OK, PSP_EVENT_WAITOR, NULL, &timeout ) ) ){
		ret = CG_ERROR_FAILED_TO_DOPEN;
	} else{
		ret = thdopen->status;
	}
	
	sceKernelSetEventFlag( thdopen->eventId, DIRH_EVENT_EXIT );
	
	return ret;
}

static int dirh_dopen_thread_main( SceSize arglen, void *argp )
{
	struct dirh_thread_dopen_params *thdopen = *(struct dirh_thread_dopen_params **)argp;
	
	thdopen->status = dirh_parse_dirent( thdopen->data, thdopen->eventId );
	sceKernelSetEventFlag( thdopen->eventId, DIRH_EVENT_OK );
	sceKernelWaitEventFlag( thdopen->eventId, DIRH_EVENT_EXIT, PSP_EVENT_WAITOR, NULL, 0 );
	sceKernelDeleteEventFlag( thdopen->eventId );
	
	thdopen->eventId = 0;
	
	return sceKernelExitDeleteThread( 0 );
}
