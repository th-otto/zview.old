#include "plugin.h"
#include "zvplugin.h"
#include "libtarga.h"

#define VERSION 0x200
#define AUTHOR "Zorro"

#ifdef PLUGIN_SLB
long __CDECL get_option(zv_int_t which)
{
	switch (which)
	{
	case OPTION_CAPABILITIES:
		return CAN_DECODE;
	case OPTION_EXTENSIONS:
		return (long)("TGA\0");
	}
	return -ENOSYS;
}
#endif

#define alpha_composite( composite, fg, alpha) {						\
    uint16_t temp = (( uint16_t)( fg) * ( uint16_t)( alpha) + ( uint16_t)128);	\
    ( composite) = ( uint8_t)(( temp + ( temp >> 8)) >> 8);				\
}


/*==================================================================================*
 * void tga_write_pixel_to_mem:														*
 *		Write the pixel to the data regarding how the header says the data 			*
 *		is ordered.																	*
 *----------------------------------------------------------------------------------*
 * input:																			*
 *		dst				->	the pixel is wrote here.								*
 *		orientation 	->	the pixel orientation ( from TGA header).				*
 *		pixel_position	->	the pixel position in the x axis.						*
 *		width			->	the image's width.										*
 *		pixel			->	the 24 bits pixel to write.								*
 *----------------------------------------------------------------------------------*
 * return:	 																		*
 *      --																			*
 *==================================================================================*/
static inline void tga_write_pixel_to_mem( uint8_t *dst, uint8_t orientation, uint16_t pixel_position, uint16_t width, uint8_t *rgb) 
{
    register uint16_t x, addy;

    switch( orientation) 
	{
	    case TGA_UPPER_LEFT:
	    case TGA_LOWER_LEFT:
	    default:
	        x = pixel_position % width;
			break;

	    case TGA_LOWER_RIGHT:
	    case TGA_UPPER_RIGHT:
	        x = width - 1 - ( pixel_position % width);
	        break;
    }

    addy = x * 3;

	dst[addy++] = rgb[0];
    dst[addy++] = rgb[1];
    dst[addy] 	= rgb[2];
}


/*==================================================================================*
 * uint32_t tga_convert_color:														*
 *		Write the pixel to the data regarding how the header says the data 			*
 *		is ordered.																	*
 *----------------------------------------------------------------------------------*
 * input:																			*
 *		rgb				->	bgra value.												*
 *		bpp_in			->	original image bitplanes.								*
 *		alphabits		->	Alpha bit... 1 if present else 0.						*
 *----------------------------------------------------------------------------------*
 * return:	 																		*
 *      the converted pixel.														*
 *==================================================================================*/
static inline void tga_convert_color( uint8_t *bgra, uint8_t *rgb, uint8_t bpp_in, uint8_t alphabits)
{
    uint8_t r = 0, g = 0, b = 0, a = 0;

    switch( bpp_in) 
	{
	    case 32:
	    case 24:
			if( alphabits)
			{
				/* not premultiplied alpha -- multiply. */
				a =  bgra[3];
				alpha_composite( r, bgra[2], a);
				alpha_composite( g, bgra[1], a);
				alpha_composite( b, bgra[0], a);
		    }
			else
			{
				r = bgra[2];
				g = bgra[1];
				b = bgra[0];
		    }		
	        break;
 
	    case 16:
		case 15:
			{
		        /* 16-bit to 32-bit; (force alpha to full) */
				register uint16_t src16 = (( ( uint16_t)bgra[1] << 8) | bgra[0]);
		        b = (( src16) 		& 0x001F) << 3;
		        g = (( src16 >> 5)  & 0x001F) << 3;
		        r = (( src16 >> 10) & 0x001F) << 3; 
		        break;
		    }
	}

	rgb[0] = r;
	rgb[1] = g;
	rgb[2] = b;
}


/*==================================================================================*
 * boolean __CDECL reader_init:														*
 *		Open the file "name", fit the "info" struct. ( see zview.h) and make others	*
 *		things needed by the decoder.												*
 *----------------------------------------------------------------------------------*
 * input:																			*
 *		name		->	The file to open.											*
 *		info		->	The IMGINFO struct. to fit.									*
 *----------------------------------------------------------------------------------*
 * return:	 																		*
 *      TRUE if all ok else FALSE.													*
 *==================================================================================*/
boolean __CDECL reader_init( const char *name, IMGINFO info)
{
    uint32_t		num_pixels, file_length;
	int16_t		handle, format = TGA_TRUECOLOR_24;
	uint8_t		header_buf[HDR_LENGTH];
    tga_pic		*tga_struct;

	if ( ( handle = ( int16_t)Fopen( name, 0)) < 0)
		return FALSE;

	if( Fread( handle, HDR_LENGTH, &header_buf) != HDR_LENGTH)
	{
		Fclose( handle);
		return FALSE;
	}

	tga_struct = ( tga_pic*)malloc( sizeof( tga_pic));

	if( tga_struct == NULL)
	{
		Fclose( handle);
		return FALSE;	
	}

    /* byte order is important here. */
	tga_struct->tga.idlen				= header_buf[0];
	tga_struct->tga.cmap_type			= header_buf[1];
	tga_struct->tga.image_type			= header_buf[2];
    tga_struct->tga.cmap_first			= (uint16_t)header_buf[3]  + ((uint16_t)header_buf[4] << 8);
    tga_struct->tga.cmap_length			= (uint16_t)header_buf[5]  + ((uint16_t)header_buf[6] << 8);
	tga_struct->tga.cmap_entry_size		= header_buf[7];
    tga_struct->tga.img_spec_xorig		= (uint16_t)header_buf[8]  + ((uint16_t)header_buf[9]  << 8);
    tga_struct->tga.img_spec_yorig		= (uint16_t)header_buf[10] + ((uint16_t)header_buf[11] << 8);
    tga_struct->tga.img_spec_width		= (uint16_t)header_buf[12] + ((uint16_t)header_buf[13] << 8);
    tga_struct->tga.img_spec_height		= (uint16_t)header_buf[14] + ((uint16_t)header_buf[15] << 8);
	tga_struct->tga.img_spec_pix_depth	= header_buf[16];
	tga_struct->tga.img_spec_img_desc	= header_buf[17];

    num_pixels = tga_struct->tga.img_spec_width * tga_struct->tga.img_spec_height;

    if( num_pixels == 0) 
	{
		free( ( void*)tga_struct);
		Fclose( handle);
		return FALSE;	
	}
	
	file_length	= Fseek( 0L, handle, 2);

    tga_struct->alphabits	= tga_struct->tga.img_spec_img_desc & 0x0F;
	tga_struct->handle		= handle;
    
    /* seek past the image id, if there is one */
	if( Fseek( tga_struct->tga.idlen + HDR_LENGTH, handle, 0) < 0 ) 
	{
		free( ( void*)tga_struct);
		Fclose( handle);
		return FALSE;	
	}

    /* if the image type is not supported, just jump out. */
	if( tga_struct->tga.image_type != TGA_IMG_UNC_TRUECOLOR && tga_struct->tga.image_type != TGA_IMG_RLE_TRUECOLOR)
	{
		free( ( void*)tga_struct);
		Fclose( handle);
		return FALSE;	
	}

    /* compute number of bytes in an image data unit (either index or BGR triple) */
    if( tga_struct->tga.img_spec_pix_depth & 0x07 )
		tga_struct->bytes_per_pix = ((( 8 - ( tga_struct->tga.img_spec_pix_depth & 0x07)) + tga_struct->tga.img_spec_pix_depth) >> 3);
    else
        tga_struct->bytes_per_pix = ( tga_struct->tga.img_spec_pix_depth >> 3);

    /* assume that there's one byte per pixel */
    if( tga_struct->bytes_per_pix == 0 )
        tga_struct->bytes_per_pix = 1;

    tga_struct->line_size = tga_struct->tga.img_spec_width * tga_struct->bytes_per_pix;
	tga_struct->img_buf_len = tga_struct->line_size << 1;

	tga_struct->img_buf  = ( uint8_t *)malloc( tga_struct->img_buf_len + 256L);

	if( tga_struct->img_buf == NULL)
	{
		free( ( void*)tga_struct);
		Fclose( handle);
		return FALSE;	
	}

	tga_struct->orientation	= ( tga_struct->tga.img_spec_img_desc & 0x30) >> 4;
	tga_struct->rest_length = file_length - ( tga_struct->tga.idlen + HDR_LENGTH);

	if ( tga_struct->img_buf_len > tga_struct->rest_length)						
		tga_struct->img_buf_len = tga_struct->rest_length;	

	tga_struct->img_buf_offset = 0;	
	tga_struct->img_buf_used 	 = Fread( handle, tga_struct->img_buf_len, tga_struct->img_buf);
	tga_struct->rest_length 	-= tga_struct->img_buf_used;

	info->width   				= tga_struct->tga.img_spec_width;
	info->height  				= tga_struct->tga.img_spec_height;
	info->real_width			= info->width;
	info->real_height			= info->height;
	info->memory_alloc 			= TT_RAM;	
	info->components			= format;
	info->planes   				= tga_struct->tga.img_spec_pix_depth;
	info->colors  				= 1L << ( uint32_t)info->planes;
	info->page	 				= 1;
	info->delay					= 0;
	info->num_comments			= 0;
	info->max_comments_length	= 0;
	info->indexed_color 		= FALSE;
	info->_priv_ptr				= ( void *)tga_struct;			

	if( tga_struct->orientation == TGA_UPPER_LEFT || tga_struct->orientation == TGA_UPPER_RIGHT)
	{
		info->orientation = UP_TO_DOWN;
	}
	else
	{
		info->orientation = DOWN_TO_UP;
	}

	strcpy( info->info, "Truevision Graphics Adapter");	

	if( tga_struct->tga.image_type == TGA_IMG_UNC_TRUECOLOR) 
	{
		strcpy( info->compression, "None");
	}
	else
	{
		strcpy( info->compression, "RLE");
	}

	return TRUE;
}


/*==================================================================================*
 * int16_t fill_img_buf:															*
 *		fill the buffer "tga_struct->img_buf" with a line from file.	 			*
 *		the buffer can contain 2 lines, this function search also where write the	*
 *		new line from the file.														*
 *----------------------------------------------------------------------------------*
 * input:																			*
 *		tga_struct		->	tga_pic struct. with all the wanted information.		*
 *		nread		 	->	internal counter.										*
 *----------------------------------------------------------------------------------*
 * return:	 																		*
 *      Always 1 ( need to code something more secure)								*
 *==================================================================================*/
static int16_t fill_img_buf( tga_pic *tga_struct, int32_t nread)
{	
	tga_struct->img_buf_used -= nread;									
	tga_struct->img_buf_offset += nread;								
	
	if ( tga_struct->img_buf_offset >= ( tga_struct->img_buf_len >> 1))	
	{
		if ( tga_struct->img_buf_used > 0)
			memcpy( tga_struct->img_buf, tga_struct->img_buf + tga_struct->img_buf_offset, tga_struct->img_buf_used);	
		
		nread = tga_struct->img_buf_offset;								

		if ( nread > tga_struct->rest_length )								
			nread = tga_struct->rest_length;

		nread = Fread( tga_struct->handle, nread, tga_struct->img_buf + tga_struct->img_buf_used);

		tga_struct->img_buf_used += nread;									
		tga_struct->img_buf_offset = 0;										
		tga_struct->rest_length -= nread;
	}

	return( 1);
}


/*==================================================================================*
 * uint8_t *unpack_line1:															*
 *		depack ( for RLE image) and copy data from source to destination. 			*
 *----------------------------------------------------------------------------------*
 * input:																			*
 *		tga_struct	->	tga_pic struct. with all the wanted information.			*
 *		src		 	->	the source buffer.											*
 *		dst		 	->	the destination buffer.										*
 *		line_width	->	the image width.											*
 *----------------------------------------------------------------------------------*
 * return:	 																		*
 *      Howmany source buffer position after that the job is done.					*
 *==================================================================================*/
static uint8_t *unpack_line1( tga_pic *tga_struct, uint8_t *src, uint8_t *dst, uint16_t line_width)
{
	register uint16_t i, j, bytes_per_pix = ( int16_t)tga_struct->bytes_per_pix;
	uint8_t	bgra[4], rgb[3], packet_header, repcount;

    if( tga_struct->tga.image_type == TGA_IMG_UNC_TRUECOLOR) 
	{
		for ( i = 0; i < line_width; i++)
		{
			for ( j = 0; j < bytes_per_pix; j++)
				bgra[j] = *src++;

 	       	tga_convert_color( &bgra[0], &rgb[0], tga_struct->tga.img_spec_pix_depth, tga_struct->alphabits);
           	tga_write_pixel_to_mem( dst, tga_struct->orientation, i, line_width, &rgb[0]);
		}
	}
	else
	{
		register uint16_t l;

        for( i = 0; i < line_width; ) 
		{
			packet_header = *src++;

            if( packet_header & 0x80 ) 
			{

				for ( j = 0; j < bytes_per_pix; j++)
					bgra[j] = *src++;

				tga_convert_color( &bgra[0], &rgb[0], tga_struct->tga.img_spec_pix_depth, tga_struct->alphabits);

                repcount = (packet_header & 0x7F) + 1;
                
                /* write all the data out */
                for( j = 0; j < repcount; j++ ) 
                    tga_write_pixel_to_mem( dst, tga_struct->orientation, i + j, line_width, &rgb[0]);

                i += repcount;
            } 
			else 
			{
                repcount = ( packet_header & 0x7F) + 1;
               
                for( l = 0; l < repcount; l++) 
				{
					for ( j = 0; j < bytes_per_pix; j++)
						bgra[j] = *src++;

					tga_convert_color( &bgra[0], &rgb[0], tga_struct->tga.img_spec_pix_depth, tga_struct->alphabits);

					tga_write_pixel_to_mem( dst, tga_struct->orientation, i + l, line_width, &rgb[0]);
                }

                i += repcount;
        	}
		}
	}

	return( src);
}


/*==================================================================================*
 * int16_t unpack_line:																*
 *		Call the "source's buffer to destination's buffer" and						* 
 *		"image to source's buffer" functions. 										*
 *----------------------------------------------------------------------------------*
 * input:																			*
 *		tga_struct	->	tga_pic struct. with all the wanted information.			*
 *		dst		 	->	the destination buffer.										*
 *----------------------------------------------------------------------------------*
 * return:	 																		*
 *      1 if ok else 0.																*
 *==================================================================================*/
static int16_t unpack_line( tga_pic *tga_struct, uint8_t *dst)
{
	uint8_t		*line_begin;
	int32_t		nread;

	line_begin = tga_struct->img_buf + tga_struct->img_buf_offset;

	nread = ( int32_t)( unpack_line1( tga_struct, line_begin, dst, ( uint16_t)tga_struct->tga.img_spec_width) - line_begin);

	if( fill_img_buf( tga_struct, nread))
		return ( 1);
	else
		return ( 0);							
}


/*==================================================================================*
 * boolean __CDECL reader_read:														*
 *		This function fits the buffer with image data								*
 *----------------------------------------------------------------------------------*
 * input:																			*
 *		buffer		->	The destination buffer.										*
 *		info		->	The IMGINFO struct. to fit.									*
 *----------------------------------------------------------------------------------*
 * return:	 																		*
 *      TRUE if all ok else FALSE.													*
 *==================================================================================*/
boolean __CDECL reader_read( IMGINFO info, uint8_t *buffer)
{
	tga_pic *tga_struct = ( tga_pic*)info->_priv_ptr;

	if( unpack_line( tga_struct, buffer))
		return TRUE;
	else
		return FALSE;
}


/*==================================================================================*
 * boolean __CDECL reader_get_txt													*
 *		This function , like other function mus be always present.					*
 *		It fills txtdata struct. with the text present in the picture ( if any).	*
 *----------------------------------------------------------------------------------*
 * input:																			*
 *		txtdata		->	The destination text buffer.								*
 *		info		->	The IMGINFO struct. to fit.									*
 *----------------------------------------------------------------------------------*
 * return:	 																		*
 *      --																			*
 *==================================================================================*/
void __CDECL reader_get_txt( IMGINFO info, txt_data *txtdata)
{
	(void)info;
	(void)txtdata;
}


/*==================================================================================*
 * boolean __CDECL reader_quit:														*
 *		This function makes the last job like close the file handler and free the	*
 *		allocated memory.															*
 *----------------------------------------------------------------------------------*
 * input:																			*
 *		info		->	The IMGINFO struct. to fit.									*
 *----------------------------------------------------------------------------------*
 * return:	 																		*
 *      --																			*
 *==================================================================================*/
void __CDECL reader_quit( IMGINFO info)
{
	tga_pic *tga_struct = ( tga_pic*)info->_priv_ptr;

	Fclose( tga_struct->handle);
	free( tga_struct->img_buf);
	free( ( void*)tga_struct);
}
