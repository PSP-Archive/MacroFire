/* iscntrl.c */

int iscntrl( int c ) 
{
	if( c < 0x20 || c == 0x7F ) return 1;
	return 0;
}
