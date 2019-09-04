#include "plugin.h"
#include "zvplugin.h"
#include "colormap.h"

#define VERSION 0x203
#define NAME    "Eureka RAW"
#define AUTHOR  "Zorro"
#define DATE    __DATE__ " " __TIME__

#ifdef PLUGIN_SLB
long __CDECL get_option(zv_int_t which)
{
	switch (which)
	{
	case OPTION_CAPABILITIES:
		return CAN_DECODE;
	case OPTION_EXTENSIONS:
		return (long)("RAW\0");

	case INFO_NAME:
		return (long)NAME;
	case INFO_VERSION:
		return VERSION;
	case INFO_DATETIME:
		return (long)DATE;
	case INFO_AUTHOR:
		return (long)AUTHOR;
	}
	return -ENOSYS;
}
#endif


static void swap( uint8_t *a, uint8_t *b)
{
	uint8_t temp;

	temp = *a;
	*a = *b;
	*b = temp;
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
	int16_t 		handle;
	int32_t		file_size, palette[768];
	uint16_t header[2];
	uint8_t *img_buffer;
	
	if ( ( handle = ( int16_t)Fopen( name, 0)) < 0)
		return FALSE;

	file_size = Fseek( 0L, handle, 2) - 4L;

	Fseek( 0L, handle, 0);

	if ( Fread( handle, 4, header) != 4)
	{
		Fclose( handle);
		return FALSE;	
	}

	info->planes 				= 8;
	info->width 				= header[0];
	info->height 				= header[1];
	info->colors  				= 256;
	info->components			= 3;
	info->real_width			= info->width;
	info->real_height			= info->height;
	info->memory_alloc 			= TT_RAM;
	info->page	 				= 1;
	info->delay					= 0;
	info->orientation			= UP_TO_DOWN;
	info->num_comments			= 0;
	info->max_comments_length 	= 0;
	info->_priv_var				= ( uint32_t)handle;

	if( ((int32_t)info->width * (int32_t)info->height) == file_size)
	{
		info->indexed_color	= FALSE;
	}
	else
	{
		/* 3072 is the palette's size */
		file_size -= 3072;

		if( ( (int32_t)info->width * (int32_t)info->height) != file_size)
		{
			Fclose( handle);
			return FALSE;	
		}

		info->indexed_color	= TRUE;
	}

	strcpy( info->info, "Eureka RAW Picture");	
	strcpy( info->compression, "None");	

	if( info->indexed_color)
	{
		int16_t i;

		Fread( handle, 3072, palette);

		for ( i = 0; i < info->colors; i++)
		{
			info->palette[i].red   = ( uint8_t)(( palette[i] >> 24) & 0xffL);
			info->palette[i].green = ( uint8_t)(( palette[i] >> 16) & 0xffL);
			info->palette[i].blue  = ( uint8_t)(( palette[i] >> 8 ) & 0xffL);
		}
	}
	else
	{
		int16_t i;

		info->indexed_color	= TRUE;

		for ( i = 0; i < info->colors; i++)
		{
			info->palette[i].red   = ( uint8_t)((( uint32_t)red[i] * 255L) / 1000L);
			info->palette[i].green = ( uint8_t)((( uint32_t)green[i] * 255L) / 1000L);
			info->palette[i].blue  = ( uint8_t)((( uint32_t)blue[i] * 255L) / 1000L);
		}
	}

	img_buffer 				= ( uint8_t*)malloc( file_size);
	info->_priv_ptr			= ( void*)img_buffer;
	info->_priv_ptr_more	= ( void*)img_buffer;

	if( img_buffer == NULL)
	{
		Fclose( handle);
		return FALSE;	
	}		

	if( Fread( handle, file_size, img_buffer) != file_size)
	{
		free( img_buffer);
		Fclose( handle);
		return FALSE;	
	}		
	else
	{
		uint16_t i, j, w = info->width >> 1;

		for( i = 0; i < info->height; i++)
			for( j = 0; j < w; j++)
				swap( &img_buffer[(uint32_t)info->width * (uint32_t)i + (uint32_t)j], &img_buffer[(uint32_t)info->width * (uint32_t)i + (uint32_t)info->width - 1L - (uint32_t)j]);
	}

	return TRUE;
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
	memcpy( buffer, ( uint8_t*)info->_priv_ptr_more, info->width);
	info->_priv_ptr_more = ( uint8_t*)info->_priv_ptr_more + info->width;

	return TRUE;
}


/*==================================================================================*
 * boolean __CDECL reader_get_txt													*
 *		This function , like other function must be always present.					*
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
	free( ( uint8_t*)info->_priv_ptr);
	Fclose( ( int16_t)info->_priv_var);
}
