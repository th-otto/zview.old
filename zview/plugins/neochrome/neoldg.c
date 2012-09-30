#include "zview.h"

boolean CDECL reader_init( const char *name, IMGINFO info);
boolean CDECL reader_read( IMGINFO info, uint8 *buffer);
void    CDECL reader_get_txt( IMGINFO info, txt_data *txtdata);
void    CDECL reader_quit( IMGINFO info);
void	CDECL init( void);


PROC NEOFunc[] = 
{
	{ "plugin_init", 	"", init},
	{ "reader_init", 	"", reader_init},
	{ "reader_get_txt", "", reader_get_txt},
	{ "reader_read", 	"", reader_read},
	{ "reader_quit", 	"", reader_quit}
};


LDGLIB neo_plugin =
{
	0x0200, 				/* Plugin version */
	5,						/* Number of plugin's functions */
	NEOFunc,				/* List of functions */
	"NEO",					/* File's type Handled */
	LDG_NOT_SHARED, 		/* The flags NOT_SHARED is used here.. even if zview plugins are reentrant 
						       and are shareable, we must use this flags because we don't know if the 
							   user has ldg.prg deamon installed on his computer */	
	libshare_exit,				/* Function called when the plugin is unloaded */
	1L						/* Howmany file type are supported by this plugin */
};

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
 * boolean CDECL reader_get_txt														*
 *		This function , like other CDECL function mus be always present.			*
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
	NEOHDR		*neo	= ( NEOHDR*)info->_priv_ptr;

	free( neo);
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
	libshare_init();
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
	ldg_init( &neo_plugin);
	return( 0);
}
