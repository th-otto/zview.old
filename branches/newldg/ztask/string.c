#include "general.h"
#include "font_8.h"

void trim_start( char *name)
{
	while( name[0] == ' ')
		// memcpy( name, name + 1, strlen( name));
		strcpy( name, name + 1);
}


void trim_end( char *name, int16 len)
{
	while( name[len] == ' ')
		name[len--] = '\0';
}



/*==================================================================================*
 * zstrncpy:																		*
 * 		Copy <n> bytes from <src> to <dst>, including the terminating \0'-byte.		* 																	*
 *		If <src> is longer than <n-1>, it will be truncated with a '\0'-byte at		* 
 * 		position n-1.																* 
 *		Example: zstrncpy(b, "string", 1) would put a '\0'-byte at b[0] and 		*
 *		return. 																	*
 *		Basically, think of the <n> as a limiter on the number of bytes at <dst> 	*
 *		we're allowed to touch, and	then fit a string-copying idea on top of that.	*
 *		Works for me, and a	lot better than the standard strncpy(), too.			*
 *----------------------------------------------------------------------------------*
 * returns: <dst>																	*
 *----------------------------------------------------------------------------------*
 * 	dst:			Pointer to destination text.									*
 * 	src:			Pointer to source text.											*
 *	n:				Limiter on the number of bytes at <dst>							*
 *==================================================================================*/
char *zstrncpy( char *dst, const char *src, size_t n)
{
	register const char *source 	= src;
	register char *dest			 	= dst;
	register size_t count		 	= n;

	if( dest && !source && count)
		*dest = '\0';
	else if( dest && source && (count >= 1))
	{
		for( count--; count > 0 && *source != '\0'; count--)
			*dest++ = *source++;
		*dest = '\0';
	}
	return dst;
}



inline void draw_text_to_buffer( int16 xf, int16 yf, int32 src_line_octets, const char *str, MFDB *bm)
{
	int16 c, character_pixel_skip = 0;
	int32 src_line_word = src_line_octets >> 1;

	while ( (c = *str++) != 0) 
	{
		int xpos = xf + character_pixel_skip; 
		int xpos_octet  = xpos >> 3;
		register int pixel_shift = xpos - ( xpos_octet << 3);
		register char *character = (char*)&my_font_8[(c - 1) << 2];
		register int i;
				
		if( pixel_shift == 0)
		{
			register char *dest = bm->fd_addr + ( yf * src_line_octets) + xpos_octet;
					
			for( i = 0; i < 8; i++)
			{
				*dest = *character++;
				dest += src_line_octets;
			}	
		}
		else
		{
			register short *dest = ( short*)( bm->fd_addr + ( yf * src_line_octets) + xpos_octet);
							
			for( i = 0; i < 8; i++)
			{
				*dest |= (( *character++) << 8) >> pixel_shift;
				dest  += src_line_word;				
			}
		}			

		character_pixel_skip += ofwf[c];			
	}
}


void draw_text( int16 vdih, int16 xf, int16 yf, int16 color, const char *str)
{
	MFDB pic, screen = {0};
	short fx_mem[16], sys_pxy[8] = { 0, 0, 0, 7, xf, yf, 0, yf + 7}, c, char_width, tcolor[2] = { color, WHITE};
	
	pic.fd_addr = fx_mem;
	pic.fd_w = 16;
	pic.fd_h = 8;
	pic.fd_wdwidth = 1;
	pic.fd_stand = 0;
	pic.fd_nplanes = 1;
	
	while ( (c = *str++) != 0) 
	{
		register char *model = (char*)&my_font_8[(c - 1) << 2] ;
		register char *dest  = (char*)fx_mem;
		
		char_width = ofwf[c];
		
		memset( fx_mem, 0, 20);
		
		*dest = *model++; 
		dest += 2;
		*dest = *model++; 
		dest += 2;
		*dest = *model++; 
		dest += 2;
		*dest = *model++; 
		dest += 2;
		*dest = *model++; 
		dest += 2;
		*dest = *model++; 
		dest += 2;
		*dest = *model++; 
		dest += 2;
		*dest = *model;
		
		sys_pxy[2] = char_width - 1;
		sys_pxy[6] = sys_pxy[4] + sys_pxy[2];
		
		/*vdi*/
		vrt_cpyfm( vdih, 2, sys_pxy, &pic, &screen, tcolor);
		
		sys_pxy[4] += char_width;
	}
}



inline int16 get_text_width( const char *str)
{
	register int16 c, text_width = 0;
	
	while ( (c = *str++) != 0) 
	{
		text_width += ofwf[c];
	}

	return text_width;
}	


int16 name_shorter( int16 max_size, char *str)
{
	int16 len, current_len;

	len = ( int16) strlen( str) - 1;
	
	do
	{
		str[len - 3] 	= '.';
		str[len - 2] 	= '.';
		str[len - 1] 	= '.';
		str[len] 		= '\0';

		len--;		

		current_len = get_text_width( str);
		
	} while ( ( current_len >= max_size) && ( len > 0));

	return current_len;
}



