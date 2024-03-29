/*
   notes: verified hyper paint with XPORTIMG.PRG VERSION 5.0 -> output iff
   ------------------------------------------------------------------------------
   1.00 1st release, added hyper paint support
        added 15-bit and 16-bit high-color support
   1.01 updated hyper paint support
   1.02 fixed 16-bit high-color decoding, added 32-bit true-color support
   1.03 removed printf() debug calls
   1.04 fixed hyper paint mono reveresed images
   1.05 minor changes to iff style decoding
   1.06 more changes to iff style decoding, fixed grayscale palettes
   1.07 added ventura publisher support
   1.08 added EGA 16 color palette, if palette is missing
   1.09 made arrays global, better error handling
   1.10 reworked debug info, firebee fix (patched purec library)
   1.11 fixed missing 256 color palette, added ventura 24-bit support
        fixed nosig 256 color ordering
 */

#define	VERSION	0x0111
#define AUTHOR "Lonny Pursell"

#include "zview.h"
#include "imginfo.h"
#include "img.h"
#include "zvimg.h"

/* globals */
typedef struct
{
	uint16_t vn;
	uint16_t len;							/* header len, usually 8 */
	uint16_t planes;
	uint16_t pl;
	uint16_t mw;
	uint16_t mh;
	uint16_t iw;
	uint16_t ih;
} HEADER;


#ifdef PLUGIN_SLB
#include "plugin.h"

long __CDECL get_option(zv_int_t which)
{
	switch (which)
	{
	case OPTION_CAPABILITIES:
		return CAN_DECODE;
	case OPTION_EXTENSIONS:
		return (long)("IMG\0");
	}
	return -ENOSYS;
}
#endif

extern int32_t plane2packed24( int32_t no_words, int32_t plane_length, int16_t no_planes, void *src, void *dst, COLOR_MAP *palette); 

boolean __CDECL reader_init( const char *name, IMGINFO info)
{
	IMG_REF 	*pic		= NULL;
	IMGHDR		*head		= NULL;
	XIMG_DATA	*imginfo	= NULL;
	int16_t		handle, line_width;
	int8_t		identify[6];
	
	if ( ( handle = ( int16_t) Fopen( name, 0)) < 0)
		return FALSE;

	pic  					= ( IMG_REF*)malloc( sizeof( IMG_REF));
	head 					= &pic->img;
	imginfo 				= &pic->info;

	if( pic == NULL)
	{
		Fclose( handle);
		return FALSE;	
	}

	if ( Fread( handle, sizeof( IMGHDR), head) != sizeof( IMGHDR))
	{
		free( pic);
		return FALSE;	
	}

	if (( head->planes != 1) && ( head->planes != 4) && ( head->planes != 8) && ( head->planes != 24)) 
	{
		free( pic);
		Fclose( handle);
		return FALSE;
	}

	pic->file_handle		= handle;
	pic->width  			= head->w;
	pic->height 			= head->h;
	pic->planes 			= head->planes;

	line_width 				= ((( head->w + 15) & 0xfff0) >> 3 ) * head->planes;

	imginfo->pat_len 		= head->pattern_length;
	imginfo->line_len 		= ( head->w + 7) >> 3;
	imginfo->file_length	= Fseek( 0L, handle, 2);
	imginfo->img_buf_len	= MAX( 8192, line_width << 1);
	imginfo->img_buf 		= ( uint8_t*)malloc( imginfo->img_buf_len + 256L);	

	if ( imginfo->img_buf == NULL)		
	{
		free( pic);
		Fclose( handle);
		return FALSE;
	}

	/* Normally, here, it's the size of the line that we allocate but we prevent buffer
	   Overflow with some picture saved with french application	*/
	imginfo->line_buffer	= ( uint8_t*)malloc( imginfo->img_buf_len + 256L);
	imginfo->img_buf_valid  = 0;

	if ( imginfo->line_buffer == NULL)		
	{
		free( imginfo->img_buf);
		free( pic);
		Fclose( handle);
		return FALSE;
	}

	info->real_width = info->width 	= pic->width;
	info->real_height = info->height= pic->height;
	info->components				= ( pic->planes == 1 ? 1 : 3);
	info->planes   					= pic->planes;
	info->orientation				= UP_TO_DOWN;
	info->colors	  				= 1L << ( uint32_t)pic->planes;
	info->indexed_color 			= FALSE; /* ( (( pic->planes == 1) || ( pic->planes > 8)) ? 0 : 1); */
	info->memory_alloc 				= TT_RAM;
	info->page	 					= 1;
	info->delay						= 0;
	info->num_comments	 			= 0;
	info->_priv_ptr					= ( void*)pic;	

	strcpy( info->info, 		"GEM Bitmap ");
	strcpy( info->compression, 	"RLE");	

	/* It a extended IMG ?, if not, return */
	if(( head->length <= ( sizeof( IMGHDR) >> 1)) || ( info->planes > 8))
		return TRUE;	

	strcat( info->info, "( eXtended)");

	Fseek( sizeof( IMGHDR), handle, 0);
	Fread( handle, 6, identify);

	if( strcmp ( identify, "XIMG") != 0)
	{
		free( imginfo->line_buffer);
		free( imginfo->img_buf);
		free( pic);
		Fclose( handle);
		return FALSE;
	}
	else
	{
		RGB1000			palette[256];
		register int32_t	pal_len;
		register int16_t	i;

		pal_len = MIN ((( head->length << 1) - 6 - sizeof( IMGHDR)), ( sizeof( RGB1000) << 8));

		Fread( handle, pal_len, palette);

		for ( i = 0; i < info->colors; i++)
		{
			register int32_t intensity = ( int32_t)palette[i].blue;
			info->palette[i].blue  = ( uint8_t) (((( intensity  << 8) - intensity)  + 500) / 1000L);
			intensity =  ( int32_t)palette[i].green;
			info->palette[i].green = ( uint8_t) (((( intensity  << 8) - intensity)  + 500) / 1000L);
			intensity =  ( int32_t)palette[i].red;
			info->palette[i].red   = ( uint8_t) (((( intensity  << 8) - intensity)  + 500) / 1000L);
		}
	}
	return TRUE;
}


static void fill_img_buf( IMG_REF *pic)
{
	XIMG_DATA	*info = &pic->info;
	int32_t		nread  = info->img_buf_len;							

	if ( nread > info->rest_length)						
		nread = info->rest_length;	

	info->img_buf_offset = 0;								
	info->img_buf_used 	 = Fread( pic->file_handle, nread, info->img_buf );
	info->rest_length 	-= info->img_buf_used;					
}


static void	refill_img_buf( IMG_REF *pic, int32_t nread)
{	
	XIMG_DATA	*info = &pic->info;

	info->img_buf_used -= nread;									
	info->img_buf_offset += nread;								
	
	if ( info->img_buf_offset >= ( info->img_buf_len >> 1))	
	{
		if ( info->img_buf_used > 0)		
			memcpy( info->img_buf, info->img_buf + info->img_buf_offset, info->img_buf_used);	
		
		nread = info->img_buf_offset;								

		if ( nread > info->rest_length )								
			nread = info->rest_length;

		nread = Fread( pic->file_handle, nread, info->img_buf + info->img_buf_used );
	
		info->img_buf_used += nread;									
		info->img_buf_offset = 0;										
		info->rest_length -= nread;									
	}
}



static uint8_t *unpack_line2( uint8_t *img, uint8_t *des, int16_t pat_len, int16_t len)
{
	register int16_t	i, cnt;

	while ( len > 0)												
	{
		register uint8_t	tag = *img++;
		
		if ( tag == 0)												
		{
			cnt = *img++;											

			for ( i = 0; i < cnt; i++)
			{
				register int16_t	j;
				for ( j = 0; j < pat_len; j++)
					*des++ = img[j];
			}

			img += pat_len;											
			cnt *= pat_len;											
		}
		else if ( tag == 0x80 )										
		{
			cnt = *img++;											
		
			for ( i = 0; i < cnt; i++)
				*des++ = *img++;
		}
		else if (( tag & 0x80 ) == 0)								
		{
			cnt = tag & 0x7f;										
			
			for ( i = 0; i < cnt; i++)
				*des++ = 0;
		}
		else														
		{
			cnt = tag & 0x7f;										
			
			for ( i = 0; i < cnt; i++)
				*des++ = 0xff;
		}	
		len -= cnt;													
	}
	return( img);
}


static uint8_t *unpack_line1( uint8_t *img, uint8_t *des, int16_t pat_len, int16_t len)
{
	if (( img[0] == 0 ) && ( img[1] == 0 ) && ( img[2] == 0xff))	
	{
		if ( img[3] > 1)											
		{
			unpack_line2( img + 4, des, pat_len, len);
			img[3] -= 1;											
			return( img);
		}
		else
			return( unpack_line2( img + 4, des, pat_len, len));
	}
	else
		return( unpack_line2( img, des, pat_len, len));
}


static void unpack_line( IMG_REF *pic, uint8_t *des)
{
	XIMG_DATA	*info = &pic->info;
	uint8_t		*img_line;
	int32_t		nread;

	img_line = info->img_buf + info->img_buf_offset;

	nread = ( int32_t) ( unpack_line1( img_line, des, info->pat_len, info->line_len * pic->planes ) - img_line );

	refill_img_buf( pic, nread);							
}


boolean __CDECL reader_read( IMGINFO info, uint8_t *buffer)
{
	int16_t		even_len;
	IMG_REF 	*pic 		= ( IMG_REF *)info->_priv_ptr;
	XIMG_DATA	*ximg_info	= &pic->info;
	IMGHDR		*head		= &pic->img;


	if ( ximg_info->img_buf_valid == 0)		
	{
		Fseek( head->length << 1, pic->file_handle, 0);			
		ximg_info->rest_length = ximg_info->file_length - ( head->length << 1);
		fill_img_buf( pic);
		ximg_info->img_buf_valid = 1;
	}										

	if( info->planes > 8)
	{
		unpack_line( pic, buffer);
		return TRUE;
	}

	even_len = ( ximg_info->line_len + 1) & ~1;

	unpack_line( pic, ximg_info->line_buffer);

	if ( ximg_info->line_len & 1)								
	{
		uint8_t	*odd, *even;
		int16_t	i;

		odd = ximg_info->line_buffer + ( ximg_info->line_len * head->planes);	
		even = odd + head->planes;							

		for ( i = 0; i < head->planes; i++)
		{
			*( --even) = 0;				
			odd -= ximg_info->line_len;
			even -= ximg_info->line_len;
			memcpy( even, odd, ximg_info->line_len);
		}
	}

	if( !plane2packed24( even_len >> 1, even_len, head->planes, ximg_info->line_buffer, buffer, info->palette))
		return FALSE;

	return TRUE;
}


void __CDECL reader_get_txt( IMGINFO info, txt_data *txtdata)
{
	(void)info;
	(void)txtdata;
}


void __CDECL reader_quit( IMGINFO info)
{
	IMG_REF *pic = ( IMG_REF *)info->_priv_ptr;

	if ( pic)
	{
		Fclose( pic->file_handle );

		free( pic->info.img_buf);										
		free( pic->info.line_buffer);									
		free( pic);
	}
}
