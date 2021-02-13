/* mfMenuRegisterTempMemory.c */

#include "mfmenu_types.h"

bool mfMenuRegisterTempMemory( void *proc, unsigned int count, ... )
{
	va_list ap;
	
	dbgprintf( "Register temporary memory from %p", proc );
	
	if( ! __mfmenu__gParams->Tmem ){
		__mfmenu__gParams->Tmem = mfMenuMemoryAlloc( sizeof( struct MfMenuTempMemory ) + sizeof( void* ) * count + 1 );
		if( ! __mfmenu__gParams->Tmem ) return false;
		__mfmenu__gParams->Tmem->prev = NULL;
	/* }else if( __mfmenu__gParams->Tmem->procAddr == (uintptr_t)proc ){
		 追加するコードだが、そういう使い方をしないようにすればいい?
		return false; */
	} else{
		__mfmenu__gParams->Tmem->next = mfMenuMemoryAlloc( sizeof( struct MfMenuTempMemory ) + sizeof( void* ) * count + 1 );
		if( ! __mfmenu__gParams->Tmem->next ) return false;
		__mfmenu__gParams->Tmem->next->prev = __mfmenu__gParams->Tmem;
		__mfmenu__gParams->Tmem = __mfmenu__gParams->Tmem->next;
	}
	__mfmenu__gParams->Tmem->next = NULL;
	
	__mfmenu__gParams->Tmem->procAddr = (uintptr_t)proc;
	__mfmenu__gParams->Tmem->memList  = (void **)( (uintptr_t)(__mfmenu__gParams->Tmem) + sizeof( struct MfMenuTempMemory ) );
	
	va_start( ap, count );
	
	__mfmenu__gParams->Tmem->memList[count] = NULL;
	while( count-- ){
		__mfmenu__gParams->Tmem->memList[count] = va_arg( ap, void* );
	}
	
	return true;
}

void __mfmenu_clear_temp_memory( void *proc )
{
	dbgprintf( "Unregister temporary memory for %p", proc );
	
	if( __mfmenu__gParams->Tmem && __mfmenu__gParams->Tmem->procAddr == (uintptr_t)proc ){
		struct MfMenuTempMemory *tmem = __mfmenu__gParams->Tmem;
		void *ptr;
		
		dbgprintf( "Proc matched. release target memories" );
		__mfmenu__gParams->Tmem = __mfmenu__gParams->Tmem->prev;
		if( __mfmenu__gParams->Tmem ) __mfmenu__gParams->Tmem->next = NULL;
		
		for( ptr = *(tmem->memList); ptr; ptr = *(++(tmem->memList)) ){
			mfMenuMemoryFree( ptr );
		}
		
		mfMenuMemoryFree( tmem );
	}
}
