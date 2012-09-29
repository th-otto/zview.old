#include "zview.h"

#define fill4B(a)	( ( 4 - ( (a) % 4 ) ) & 0x03)

boolean CDECL reader_init( const char *name, IMGINFO info);
boolean CDECL reader_read( IMGINFO info, uint8 *buffer);
void    CDECL reader_get_txt( IMGINFO info, txt_data *txtdata);
void    CDECL reader_quit( IMGINFO info);
void	CDECL init( void);

PROC BMPFunc[] = 
{
	{ "plugin_init", 	"", init},
	{ "reader_init", 	"", reader_init},
	{ "reader_get_txt", "", reader_get_txt},
	{ "reader_read", 	"", reader_read},
	{ "reader_quit", 	"", reader_quit}
};


LDGLIB bmp_plugin =
{
	0x0200, 			/* Plugin version */
	5,					/* Number of plugin's functions */
	BMPFunc,			/* List of functions */
	"BMP",				/* File's type Handled */
	LDG_NOT_SHARED, 	/* The flags NOT_SHARED is used here.. even if zview plugins are reentrant 
					   	   and are shareable, we must use this flags because we don't know if the 
					   	   user has ldg.prg deamon installed on his computer */
	NULL,				/* Function called when the plugin is unloaded */
	1L					/* Howmany file type are supported by this plugin */
};


/*==================================================================================*
 * uint32 ToL:																		*
 *		Transform a unsigned long from little to big endian.						*
 *----------------------------------------------------------------------------------*
 * input:																			*
 *		puffer		->	pointer to the little endian long to convert.				*
 *----------------------------------------------------------------------------------*
 * return:	 																		*
 *      --																			*
 *==================================================================================*/
uint32 ToL( uint8 * puffer)
{
  return ( puffer[0] | ( puffer[1] << 8) | ( puffer[2] << 16) | ( puffer[3] << 24));
}


/*==================================================================================*
 * uint16 ToS:																		*
 *		Transform a unsigned word from little to big endian.						*
 *----------------------------------------------------------------------------------*
 * input:																			*
 *		puffer		->	pointer to the little endian word to convert.				*
 *----------------------------------------------------------------------------------*
 * return:	 																		*
 *      --																			*
 *==================================================================================*/
uint16 ToS( uint8 *puffer)
{
  return ( puffer[0] | ( puffer[1] << 8));
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
	uint8		buff[4], header_buffer[50], *line_buffer;
	int16		handle, compressed;
	uint32		bisize, line_size, skip;

	if ( ( handle = ( int16) Fopen( name, 0)) < 0)
		return FALSE;

	if ( Fread( handle, 2, buff) != 2)
	{	
		Fclose( handle);
		return FALSE;	
	}

	if ( buff[0] != 'B' || buff[1] != 'M' )
	{	
		Fclose( handle);
		return FALSE;	
	}

	if ( Fread( handle, 0x10, header_buffer) != 0x10)
	{	
		Fclose( handle);
		return FALSE;	
	}

	bisize = ToL( &header_buffer[12]);

	if( bisize != 40uL) 
	{
		/* OS/2 BMP No supported */
		Fclose( handle);
		return FALSE;	
	}
	else
	{
		/* Window */
		Fread( handle, 36L, header_buffer);
		info->width  	= (uint16)ToL (&header_buffer[0x00]);
		info->height 	= (uint16)ToL (&header_buffer[0x04]);
		info->planes 	= ToS (&header_buffer[0x0A]);	
		compressed		= (int16)ToL (&header_buffer[0x0C]);
	}

	if( compressed)
	{	
		Fclose( handle);
		return FALSE;	
	}

	switch( info->planes)
	{
		case 1:
			line_size = ( info->width >> 3) + ( info->width % 8 ? 1 : 0);
			break;
		case 4:
			line_size = ( info->width >> 1) + info->width % 2;
			break; 
		case 8:
			line_size = info->width;
			break;
		case 24:
			line_size = info->width * 3;
			break;
		default:
			Fclose( handle);
			return FALSE;
	}
	
	skip = fill4B( line_size);
	line_size += skip;
	line_buffer = malloc( line_size + 256L);

	if( line_buffer == NULL)
	{	
		Fclose( handle);
		return FALSE;	
	}

	info->real_width			= info->width;
	info->real_height			= info->height;
	info->components			= ( info->planes == 1 ? 1 : 3);
	info->indexed_color 		= FALSE;
	info->memory_alloc 			= TT_RAM;
	info->page	 				= 1;
	info->delay					= 0;
	info->orientation			= DOWN_TO_UP;
	info->num_comments			= 0;
	info->max_comments_length 	= 0;
	info->colors  				= 1uL << (uint32)info->planes;
	info->_priv_ptr 			= ( void*)line_buffer;
	info->_priv_var 			= ( int32)handle;
	info->_priv_var_more		= ( int32)line_size;

	strcpy( info->info, 		"Window BitMaP");
	strcpy( info->compression, 	"None");	

	if( info->planes <= 8)
	{
		register int16 i;

		for ( i = 0; i < info->colors; i++)
		{
			Fread( handle, 4L, buff);
			info->palette[i].blue  = buff[0];
			info->palette[i].green = buff[1];
			info->palette[i].red   = buff[2];
		}
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
	int16 width, j, k, i, handle = ( int16)info->_priv_var;
	uint8 buff[2], *line_buffer = ( uint8*)info->_priv_ptr;
	uint32 line_size = info->_priv_var_more;

	if( Fread( handle, line_size, line_buffer) != line_size)
		return FALSE;

	switch( info->planes)
	{
		case 1:
			width = info->width >> 3;

			for ( j = 0; j < width; j++) 
			{
				buff[0] = line_buffer[j];

				for ( k = 0; k < 8; k++) 
				{
					if (buff[0] & 0x80) 
						*buffer++ = 0xff;
					else 
						*buffer++ = 0x00;

					buff[0] = buff[0] << 1;
				}
					
			}
			if ( info->width % 8) 
			{
				buff[0] = line_buffer[j+1];

				for ( k = 0;  k < info->width % 8; k++) 
				{
					if (buff[0] & 0x80)
						*buffer++ = 0xff;
					else
						*buffer++ = 0x00;
						
					buff[0] = buff[0] << 1;
				}
			}
			break;

		case 4:
			width = info->width >> 1;

			for ( j = 0; j < width; j++) 
			{
				buff[1] = line_buffer[j] >> 4;
				buff[2] = line_buffer[j] & 0x0f;
				*buffer++ = info->palette[buff[1]].red;
				*buffer++ = info->palette[buff[1]].green;
				*buffer++ = info->palette[buff[1]].blue;
				*buffer++ = info->palette[buff[2]].red;
				*buffer++ = info->palette[buff[2]].green;
				*buffer++ = info->palette[buff[2]].blue;
			}
			if ( info->width % 2) 
			{
				buff[1] = line_buffer[j+1] >> 4;
				*buffer++ = info->palette[buff[1]].red;
				*buffer++ = info->palette[buff[1]].green;
				*buffer++ = info->palette[buff[1]].blue;
			}
			break;

		case 8:
			for ( j = 0; j < info->width; j++) 
			{
				*buffer++ = info->palette[line_buffer[j]].red;
				*buffer++ = info->palette[line_buffer[j]].green;
				*buffer++ = info->palette[line_buffer[j]].blue;
			}
			break;

		case 24:
			for ( i = j = 0; j < info->width; j++, i += 3) 
			{
				*buffer++ = line_buffer[i+2];
				*buffer++ = line_buffer[i+1];
				*buffer++ = line_buffer[i];
			}
			break;

		default:
			return FALSE;
	}

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
	free( info->_priv_ptr);
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
	ldg_init( &bmp_plugin);
	return( 0);
}
