/* inimgrLoad.c */

#include "inimgr_types.h"

static int inimgr_check_sigver( char *sigver_line, const char *sig, InimgrVersion reqver );
static char *inimgr_line_is_section( char *line );

int inimgrLoad( InimgrUID uid, const char *path, const char *sig, InimgrVersion version )
{
	struct inimgr_params  *params = (struct inimgr_params *)uid;
	struct inimgr_section *current_section = params->section;
	char *line_start;
	
	params->ini = fiomgrOpen( path, FH_O_RDWR | FH_O_ALLOC_HIGH, 0777 );
	if( ! CG_IS_VALID_UID( params->ini ) ) return params->ini;
	
	if( sig ){
		int ret;
		while( ( line_start = __inimgr_read_line( params->ini, params->buffer, sizeof( params->buffer ) ) ) ){
			if( line_start && *line_start != ';' && *line_start != '\0' ) break;
		}
		
		if( ( ret = inimgr_check_sigver( line_start, sig, version ) ) != CG_ERROR_OK ){
			fiomgrClose( params->ini );
			params->ini = 0;
			return ret;
		}
		
		/* セクション行の次の行がデフォルトセクションの開始位置 */
		params->section->offsetStart = fiomgrTell( params->ini );
	}
	
	/* 各セクションのオフセットと長さを取得 */
	while( ( line_start = __inimgr_read_line( params->ini, params->buffer, sizeof( params->buffer ) ) ) ){
		if( ( line_start = inimgr_line_is_section( line_start ) ) ){
			current_section->next = __inimgr_create_section( params, line_start );
			if( ! current_section->next ){
				fiomgrClose( params->ini );
				params->ini = 0;
				return CG_ERROR_NOT_ENOUGH_MEMORY;
			}
			current_section->next->prev = current_section;
			current_section->next->next = NULL;
			current_section = current_section->next;
			current_section->offsetStart = fiomgrTell( params->ini );
		} else if( current_section ){
			current_section->offsetEnd = fiomgrTell( params->ini );
		}
	}
	
	params->last = params->section;
	
	return CG_ERROR_OK;
}

static int inimgr_check_sigver( char *sigver_line, const char *sig, InimgrVersion reqver )
{
	char *ver_start;
	
	if( sig[0] == '\0' || strchr( sig, '\x20' ) ) return CG_ERROR_INVALID_ARGUMENT;
	
	ver_start = strpbrk( sigver_line, "\x20\t" );
	if( ver_start ){
		*ver_start++ = '\0';
		ver_start = strutilCounterPbrk( ver_start, "\x20\t" );
	}
	
	if( strcasecmp( sigver_line, sig ) != 0 ) return CG_ERROR_INI_SIGNATURE_MISMATCH;
	
	if( reqver ){
		unsigned char major, minor, rev;
		char *num, *saveptr;
		
		if( ! ver_start ) return CG_ERROR_INI_VERSION_NOT_SUPPORTED;
		strutilTrim( ver_start, "\x20\t" );
		
		num = strtok_r( ver_start, ".", &saveptr );
		if( num ){
			major = (unsigned char)( num ? strtoul( num, NULL, 10 ) : 0 );
			num   = strtok_r( NULL, ".", &saveptr );
			minor = (unsigned char)( num ? strtoul( num, NULL, 10 ) : 0 );
			num   = strtok_r( NULL, ".", &saveptr );
			rev   = (unsigned char)( num ? strtoul( num, NULL, 10 ) : 0 );
		} else{
			major = (unsigned char)( num ? strtoul( ver_start, NULL, 10 ) : 0 );
			minor = 0;
			rev   = 0;
		}
		if( inimgrMakeVersion( major, minor, rev ) < reqver ) return CG_ERROR_INI_VERSION_NOT_SUPPORTED;
	}
	
	return CG_ERROR_OK;
}

static char *inimgr_line_is_section( char *line )
{
	char *line_end;
	if( *line == '[' && isalnum( *(line + 1) ) ){
		if( ( line_end = strchr( line, ']' ) ) ){
			*line_end = '\0';
			return line + 1;
		}
	}
	return NULL;
}

