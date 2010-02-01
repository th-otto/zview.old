#include "zview.h"
#include "colormap.h"

boolean CDECL reader_init( const char *name, IMGINFO info);
boolean CDECL reader_read( IMGINFO info, uint8 *buffer);
void    CDECL reader_get_txt( IMGINFO info, txt_data *txtdata);
void    CDECL reader_quit( IMGINFO info);
void	CDECL init( void);


PROC RAWFunc[] = 
{
	{ "plugin_init", 	"", init},
	{ "reader_init", 	"", reader_init},
	{ "reader_get_txt", "", reader_get_txt},
	{ "reader_read", 	"", reader_read},
	{ "reader_quit", 	"", reader_quit}
};


LDGLIB raw_plugin =
{
	0x0200, 			/* Plugin version */
	5,					/* Number of plugin's functions */
	RAWFunc,			/* List of functions */
	"RAW",				/* File's type Handled */
	LDG_NOT_SHARED, 	/* The flags NOT_SHARED is used here.. even if zview plugins are reentrant 
					   	   and are shareable, we must use this flags because we don't know if the 
					   	   user has ldg.prg deamon installed on his computer */
#ifdef __PUREC__
	NULL,				/* Function called when the plugin is unloaded */
#else
	libshare_exit,		/* Function called when the plugin is unloaded */
#endif
	1L					/* Howmany file type are supported by this plugin */
};


void swap( uint8 *a, uint8 *b)
{
	uint8 temp;

	temp = *a;

	*a = *b;

	*b = temp;
}

/*==================================================================================*
 * boolean CDECL reader_init:														*
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
boolean CDECL reader_init( const char *name, IMGINFO info)
{
	int16 		handle;
	int32		file_size, palette[768];
	uint8     	header[4], *img_buffer;

	if ( ( handle = ( int16)Fopen( name, 0)) < 0)
		return FALSE;

	file_size = Fseek( 0L, handle, 2) - 4L;

	Fseek( 0L, handle, 0);

	if ( Fread( handle, 4, header) != 4)
	{
		Fclose( handle);
		return FALSE;	
	}

	info->planes 				= 8;
	info->width 				= *((uint16*)&header[0]);
	info->height 				= *((uint16*)&header[2]);
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
	info->_priv_var				= ( uint32)handle;

	if( ( info->width * info->height) == file_size)
	{
		info->indexed_color	= FALSE;
	}
	else
	{
		/* 3072 is the palette's size */
		file_size -= 3072;

		if( ( info->width * info->height) != file_size)
		{
			Fclose( handle);
			return FALSE;	
		}

		info->indexed_color	= TRUE;
	}

	strcpy( info->info, "Eureka RAW Picture");	
	strcpy( info->compression, "None");	

	if( info->indexed_color	== TRUE)
	{
		register int16 i;

		Fread( handle, 3072, palette);

		for ( i = 0; i < info->colors; i++)
		{
			info->palette[i].red   = ( uint8)(( palette[i] >> 24) & 0xffL);
			info->palette[i].green = ( uint8)(( palette[i] >> 16) & 0xffL);
			info->palette[i].blue  = ( uint8)(( palette[i] >> 8 ) & 0xffL);
		}
	}
	else
	{
		register int16 i;

		info->indexed_color	= TRUE;

		for ( i = 0; i < info->colors; i++)
		{
			info->palette[i].red   = ( uint8)((( uint32)red[i] * 255L) / 1000L);
			info->palette[i].green = ( uint8)((( uint32)red[i] * 255L) / 1000L);
			info->palette[i].blue  = ( uint8)((( uint32)red[i] * 255L) / 1000L);
		}
	}

	img_buffer 				= ( uint8*)malloc( file_size);
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
		register uint16 i, j, w = info->width >> 1;

		for( i = 0; i < info->height; i++)
			for( j = 0; j < w; j++)
				swap( &img_buffer[(uint32)info->width * (uint32)i + (uint32)j], &img_buffer[(uint32)info->width * (uint32)i + (uint32)info->width - 1L - (uint32)j]);
	}

	return TRUE;
}


/*==================================================================================*
 * boolean CDECL reader_read:														*
 *		This function fits the buffer with image data								*
 *----------------------------------------------------------------------------------*
 * input:																			*
 *		buffer		->	The destination buffer.										*
 *		info		->	The IMGINFO struct. to fit.									*
 *----------------------------------------------------------------------------------*
 * return:	 																		*
 *      TRUE if all ok else FALSE.													*
 *==================================================================================*/
boolean CDECL reader_read( IMGINFO info, uint8 *buffer)
{
	memcpy( buffer, ( uint8*)info->_priv_ptr_more, info->width);
	( uint8*)info->_priv_ptr_more += info->width;

	return TRUE;
}


/*==================================================================================*
 * boolean CDECL reader_get_txt														*
 *		This function , like other CDECL function must be always present.			*
 *		It fills txtdata struct. with the text present in the picture ( if any).	*
 *----------------------------------------------------------------------------------*
 * input:																			*
 *		txtdata		->	The destination text buffer.								*
 *		info		->	The IMGINFO struct. to fit.									*
 *----------------------------------------------------------------------------------*
 * return:	 																		*
 *      --																			*
 *==================================================================================*/
void CDECL reader_get_txt( IMGINFO info, txt_data *txtdata)
{
	return;
}


/*==================================================================================*
 * boolean CDECL reader_quit:														*
 *		This function makes the last job like close the file handler and free the	*
 *		allocated memory.															*
 *----------------------------------------------------------------------------------*
 * input:																			*
 *		info		->	The IMGINFO struct. to fit.									*
 *----------------------------------------------------------------------------------*
 * return:	 																		*
 *      --																			*
 *==================================================================================*/
void CDECL reader_quit( IMGINFO info)
{
	free( ( uint8*)info->_priv_ptr);
	Fclose( ( int16)info->_priv_var);
}


/*==================================================================================*
 * boolean CDECL init:																*
 *		First function called from zview, in this one, you can make some internal	*
 *		initialisation.																*
 *----------------------------------------------------------------------------------*
 * input:																			*
 *		--																			*
 *----------------------------------------------------------------------------------*
 * return:	 																		*
 *      --																			*
 *==================================================================================*/
void CDECL init( void)
{
#ifdef __PUREC__
	/* Make the compiler happy*/
	return;	
#else
	libshare_init();	
#endif
}


/*==================================================================================*
 * int main:																		*
 *		Main function, his job is to call ldg_init function.						*
 *----------------------------------------------------------------------------------*
 * input:																			*
 *		--																			*
 *----------------------------------------------------------------------------------*
 * return:	 																		*
 *      0																			*
 *==================================================================================*/
int main( void)
{
	ldg_init( &raw_plugin);
	return( 0);
}
