/*=========================================================
	
	errno.c
	
	PSPSDK��libcglue.c����q�؁B
	����ł����̂����Ă����B
	
=========================================================*/

int errno;

/* TODO: Should this be made reentrant (wrapping interrupt disable/enable
   around it should do it)? */
int * __errno( void )
{
	return &errno;
}