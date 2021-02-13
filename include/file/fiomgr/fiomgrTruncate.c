/* fiomgrTruncate.c */

#if 0

/* truncate()‚ª‹@”\‚µ‚È‚¢‚Ì‚Å•Û—¯ */

#include "fiomgr_types.h"

#ifdef PSP_USE_KERNEL_LIBC
#include "sysclib.h"
#endif

int fiomgrTruncate( FiomgrHandle fh, SceOff length )
{
	struct fiomgr_params *params = (struct fiomgr_params *)fh;
	SceOff offset;
	int ret;
	
	if( ! ( params->flags & ( FH_O_WRONLY | FH_O_RDWR ) ) ) return CG_ERROR_NOT_WRITABLE;
	
	offset = fiomgrTell( fh );
	
	ret = __fiomgr_close( params );
	if( ret < 0 ) return ret;
	
	__fiomgr_cache_clear( params );
	
	ret = truncate( params->path, length );
	if( ret < 0 ) return ret;
	
	params->fd = sceIoOpen( params->path, params->flags, params->mode );
	if( params->fd < 0 ) return params->fd;
	
	if( offset > length ) offset = length;
	fiomgrSeek( fh, offset, FH_SEEK_SET );
	
	return CG_ERROR_OK;
}
#endif