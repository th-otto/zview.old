#include "zview.h"
#include "img.h"

extern int32 plane2packed24( int32 no_words, int32 plane_length, int16 no_planes, void *src, void *dst, COLOR_MAP *palette); 

boolean CDECL reader_init( const char *name, IMGINFO info);
void 	CDECL reader_get_txt( IMGINFO info, txt_data *txtdata);
boolean CDECL reader_read( IMGINFO info, uint8 *buffer);
void    CDECL reader_quit( IMGINFO info);
void	CDECL init( void);


PROC IMGFunc[] = 
{
	{ "plugin_init", 	"", init},
	{ "reader_init", 	"", reader_init},
	{ "reader_get_txt", "", reader_get_txt},
	{ "reader_read", 	"", reader_read},
	{ "reader_quit", 	"", reader_quit}
};


LDGLIB img_plugin =
{
	0x0200, 			/* Plugin version */
	5,					/* Number of plugin's functions */
	IMGFunc,			/* List of functions */
	"IMG",				/* File's type Handled */
	LDG_NOT_SHARED, 	/* The flags NOT_SHARED is used here.. even if zview plugins are reentrant 
						   and are shareable, we must use this flags because we don't know if the 
						   user has ldg.prg deamon installed on his computer */
	libshare_exit,				/* Function called when the plugin is unloaded */
	1L					/* Howmany file type are supported by this plugin */
};



boolean CDECL reader_init( const char *name, IMGINFO info)
{
	IMG_REF 	*pic		= NULL;
	IMGHDR		*head		= NULL;
	XIMG_DATA	*imginfo	= NULL;
	int16		handle, line_width;
	int8		identify[6];
	
	if ( ( handle = ( int16) Fopen( name, 0)) < 0)
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
	imginfo->img_buf 		= ( uint8*)malloc( imginfo->img_buf_len + 256L);	

	if ( imginfo->img_buf == NULL)		
	{
		free( pic);
		Fclose( handle);
		return FALSE;
	}

	/* Normally, here, it's the size of the line that we allocate but we prevent buffer
	   Overflow with some picture saved with french application	*/
	imginfo->line_buffer	= ( uint8*)malloc( imginfo->img_buf_len + 256L);
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
	info->colors	  				= 1L << ( uint32)pic->planes;
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
		register int32	pal_len;
		register int16	i;

		pal_len = MIN ((( head->length << 1) - 6 - sizeof( IMGHDR)), ( sizeof( RGB1000) << 8));

		Fread( handle, pal_len, palette);

		for ( i = 0; i < info->colors; i++)
		{
			register int32 intensity = ( int32)palette[i].blue;
			info->palette[i].blue  = ( uint8) (((( intensity  << 8) - intensity)  + 500) / 1000L);
			intensity =  ( int32)palette[i].green;
			info->palette[i].green = ( uint8) (((( intensity  << 8) - intensity)  + 500) / 1000L);
			intensity =  ( int32)palette[i].red;
			info->palette[i].red   = ( uint8) (((( intensity  << 8) - intensity)  + 500) / 1000L);
		}
	}
	return TRUE;
}


void fill_img_buf( IMG_REF *pic)
{
	XIMG_DATA	*info = &pic->info;
	int32		read  = info->img_buf_len;							

	if ( read > info->rest_length)						
		read = info->rest_length;	

	info->img_buf_offset = 0;								
	info->img_buf_used 	 = Fread( pic->file_handle, read, info->img_buf );
	info->rest_length 	-= info->img_buf_used;					
}


void	refill_img_buf( IMG_REF *pic, int32 read)
{	
	XIMG_DATA	*info = &pic->info;

	info->img_buf_used -= read;									
	info->img_buf_offset += read;								
	
	if ( info->img_buf_offset >= ( info->img_buf_len >> 1))	
	{
		if ( info->img_buf_used > 0)		
			memcpy( info->img_buf, info->img_buf + info->img_buf_offset, info->img_buf_used);	
		
		read = info->img_buf_offset;								

		if ( read > info->rest_length )								
			read = info->rest_length;

		read = Fread( pic->file_handle, read, info->img_buf + info->img_buf_used );
	
		info->img_buf_used += read;									
		info->img_buf_offset = 0;										
		info->rest_length -= read;									
	}
}



uint8 *unpack_line2( uint8 *img, uint8 *des, int16 pat_len, int16 len)
{
	register int16	i, cnt;

	while ( len > 0)												
	{
		register uint8	tag = *img++;
		
		if ( tag == 0)												
		{
			cnt = *img++;											

			for ( i = 0; i < cnt; i++)
			{
				register int16	j;
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


uint8 *unpack_line1( uint8 *img, uint8 *des, int16 pat_len, int16 len)
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


void unpack_line( IMG_REF *pic, uint8 *des)
{
	XIMG_DATA	*info = &pic->info;
	uint8		*img_line;
	int32		read;

	img_line = info->img_buf + info->img_buf_offset;

	read = ( int32) ( unpack_line1( img_line, des, info->pat_len, info->line_len * pic->planes ) - img_line );

	refill_img_buf( pic, read);							
}


boolean CDECL reader_read( IMGINFO info, uint8 *buffer)
{
	int16		even_len;
	IMG_REF 	*pic 		= ( IMG_REF *)info->_priv_ptr;
	XIMG_DATA	*img_info	= &pic->info;
	IMGHDR		*head		= &pic->img;


	if ( img_info->img_buf_valid == 0)		
	{
		Fseek( head->length << 1, pic->file_handle, 0);			
		img_info->rest_length = img_info->file_length - ( head->length << 1);
		fill_img_buf( pic);
		img_info->img_buf_valid = 1;
	}										

	if( info->planes > 8)
	{
		unpack_line( pic, buffer);
		return TRUE;
	}

	even_len = ( img_info->line_len + 1) & ~1;

	unpack_line( pic, img_info->line_buffer);

	if ( img_info->line_len & 1)								
	{
		uint8	*odd, *even;
		int16	i;

		odd = img_info->line_buffer + ( img_info->line_len * head->planes);	
		even = odd + head->planes;							

		for ( i = 0; i < head->planes; i++)
		{
			*( --even) = 0;				
			odd -= img_info->line_len;
			even -= img_info->line_len;
			memcpy( even, odd, img_info->line_len);
		}
	}

	if( !plane2packed24( even_len >> 1, even_len, head->planes, img_info->line_buffer, buffer, info->palette))
		return FALSE;

	return TRUE;
}


void CDECL reader_get_txt( IMGINFO info, txt_data *txtdata)
{
	return;
}


void CDECL reader_quit( IMGINFO info)
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


void CDECL init( void)
{
	libshare_init();
}


int main( void)
{
	ldg_init( &img_plugin);
	return( 0);
}
