/* pbMeasureString.c */

#include "pb_types.h"

unsigned int pbMeasureNString( const char *str, int n )
{
	unsigned int i;
	unsigned int len;
	
	for( i = 0, len = 0; str[i]; i++ ){
		if( n >= 0 ){
			if( n == 0 ){
				break;
			} else{
				n--;
			}
		}
		
#ifdef PB_SJIS_SUPPORT
		if( (unsigned char)str[i] < 0x80 ){
			len += PB_CHAR_WIDTH;
		} else if( (unsigned char)str[i] >= 0xA0 && (unsigned char)str[i] <= 0xDF ){
			len += PB_CHAR_WIDTH;
		} else if( n && str[i + 1] ){
			len += PB_WCHAR_WIDTH;
			i++;
			n--;
		}
#else
		len += PB_CHAR_WIDTH;
#endif
	}
	
	return len;
}
