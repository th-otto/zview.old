#include "zview.h"
#include "imginfo.h"
#include "zvneo.h"

#ifdef PLUGIN_SLB
#include "plugin.h"

long __CDECL get_option(zv_int_t which)
{
	switch (which)
	{
	case OPTION_CAPABILITIES:
		return CAN_DECODE;
	case OPTION_EXTENSIONS:
		return (long)("NEO\0");
	}
	return -ENOSYS;
}
#endif

#define fill4B(a)	( ( 4 - ( (a) % 4 ) ) & 0x03)


typedef struct 
{
	int16 	flags;				/* Always 0										*/
	int16 	resolution;			/* 0 = low, 1 = medium, 3 = high 				*/
	uint16 	palette[16];		/* Picture palette in Atari ST hardware format 	*/
	int8   	filename[12];		/* The original file name						*/
	int16	color_anim_limit;	/* color animation limit						*/
	int16	color_anim_speed;	/* color animation speed and direction			*/
	int16	color_steps;		/* numbre of color step							*/
	int16	x_offset;			/* image offset									*/
	int16	y_offset;			/* "											*/
	int16	width;				
	int16	height;				
	int16	reserverd[33];		
} NEOHDR;


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
	int32 		position;
	int16		handle;
	NEOHDR		*neo;

	if ( ( handle = ( int16)Fopen( name, 0)) < 0)
		return FALSE;

	if( Fseek( 0L, handle, 2) != 32128L)
	{
		Fclose( handle);
		return FALSE;	
	}		

	Fseek( 0L, handle, 0);

	neo = ( NEOHDR *)malloc( 32128L);

	if ( neo == NULL)
	{
		Fclose( handle);
		return FALSE;	
	}

	if ( Fread( handle, 32128L, neo) != 32128L)
	{
		free( neo);
		Fclose( handle);
		return FALSE;	
	}

	Fclose( handle);

	switch( neo->resolution)
	{
		case 0:
			info->width 		= 320;
			info->height 		= 200;
			info->planes 		= 4;
			info->components	= 3;
			info->indexed_color	= TRUE;
			info->colors  		= 16;
			break;

		case 1:
			info->width 		= 640;
			info->height 		= 200;
			info->planes 		= 2;
			info->components	= 3;
			info->indexed_color	= TRUE;
			info->colors  		= 4;
			break;

		case 2:
			info->width 		= 640;
			info->height 		= 400;
			info->planes 		= 1;
			info->components	= 1;
			info->indexed_color	= FALSE;
			info->colors  		= 2;
			break;

		default:
			free( neo);
			return FALSE;		
	}

	position = sizeof( NEOHDR);

	info->real_width			= info->width;
	info->real_height			= info->height;
	info->memory_alloc 			= TT_RAM;
	info->page	 				= 1;
	info->delay					= 0;
	info->orientation			= UP_TO_DOWN;
	info->num_comments			= 0;
	info->max_comments_length 	= 0;
	info->_priv_ptr				= ( void*)neo;
	info->_priv_var_more		= position;		

	strcpy( info->info, "Neochrome");	
	strcpy( info->compression, "None");

	if( info->indexed_color)
	{
		register int16 i;

		for ( i = 0; i < info->colors; i++)
		{
			info->palette[i].red   = ((( neo->palette[i] >> 7) & 0xE) + (( neo->palette[i] >> 11) & 0x1)) * 0x11;
			info->palette[i].green = ((( neo->palette[i] >> 3) & 0xE) + (( neo->palette[i] >> 7) & 0x1))  * 0x11;
			info->palette[i].blue  = ((( neo->palette[i] << 1) & 0xE) + (( neo->palette[i] >> 3) & 0x1))  * 0x11;
		}
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
boolean __CDECL reader_read( IMGINFO info, uint8 *buffer)
{
	NEOHDR		*neo	= ( NEOHDR*)info->_priv_ptr;
	uint8 		*l, *s = ( uint8*)neo + info->_priv_var_more;
	uint16		x, c, p0, p1, p2, p3;

	switch( info->planes)
	{	
		case 4:
			for( x = 0, l = s; x < info->width; l += 8)
			{
				p0 = ( l[0] << 8) | l[1]; 
				p1 = ( l[2] << 8) | l[3];
				p2 = ( l[4] << 8) | l[5];
				p3 = ( l[6] << 8) | l[7];

		        for ( c = 0; c < 16; c++)
		        {
		     	   buffer[x++] = ((p0 >> 15) & 1) | ((p1 >> 14) & 2) | ((p2 >> 13) & 4) | ((p3 >> 12) & 8);
		           p0 <<= 1;
		           p1 <<= 1;
		           p2 <<= 1;
		           p3 <<= 1;
		        }
			}
			info->_priv_var_more += 160;
			break;

		case 2:
			for ( x = 0, l = s; x < info->width; l += 4)
		    {
		    	p0 = (l[0] << 8) | l[1];
		    	p1 = (l[2] << 8) | l[3];

		    	for (c = 0; c < 16; c++)
		        {
		    	    buffer[x++] = ((p0 >> 15) & 1) | ((p1 >> 14) & 2);
		            p0 <<= 1;
		            p1 <<= 1;
		        }
		    }
			info->_priv_var_more += 160;
			break;			

		case 1:
	        for (x = 0, l = s; x < info->width; l += 2)
	        {
	            p0 = ( l[0] << 8) | l[1];
	
	            for ( c = 0; c < 16; c++)
	            {
	                buffer[x++] = ((p0 >> 15) & 1);
	                p0 <<= 1;
	            }
	        }
			info->_priv_var_more += 80;
			break;
	}

	return TRUE;
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
	NEOHDR		*neo	= ( NEOHDR*)info->_priv_ptr;

	free( neo);
}
