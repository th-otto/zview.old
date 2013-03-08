#include "zview.h"

boolean CDECL reader_init( const char *name, IMGINFO info);
boolean CDECL reader_read( IMGINFO info, uint8 *buffer);
void    CDECL reader_get_txt( IMGINFO info, txt_data *txtdata);
void    CDECL reader_quit( IMGINFO info);
void	CDECL init( void);


PROC DEGASFunc[] = 
{
	{ "plugin_init", 	"", init},
	{ "reader_init", 	"", reader_init},
	{ "reader_get_txt", "", reader_get_txt},
	{ "reader_read", 	"", reader_read},
	{ "reader_quit", 	"", reader_quit}
};


LDGLIB degas_plugin =
{
	0x0200, 			/* Plugin version */
	5,					/* Number of plugin's functions */
	DEGASFunc,			/* List of functions */
	"PI1PI2PI3PC1PC2PC3",		/* File's type Handled */
	LDG_NOT_SHARED, 	/* The flags NOT_SHARED is used here.. even if zview plugins are reentrant 
					   	   and are shareable, we must use this flags because we don't know if the 
					   	   user has ldg.prg deamon installed on his computer */
	libshare_exit,				/* Function called when the plugin is unloaded */
	6L					/* Howmany file type are supported by this plugin */
};

typedef struct 
{
	uint16 compression  : 1;
	uint16 reserved0	: 13;
	uint16 resolution	: 2;
	uint16 palette[16];
} DEGASHDR;



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
	int32		position, file_size;
	DEGASHDR	*degas;

	if ( ( handle = ( int16)Fopen( name, 0)) < 0)
		return FALSE;

	file_size = Fseek( 0L, handle, 2);

	if( file_size > 33000L)
	{
		Fclose( handle);
		return FALSE;	
	}		

	Fseek( 0L, handle, 0);

	degas = ( DEGASHDR *)malloc( file_size);

	if ( degas == NULL)
	{
		Fclose( handle);
		return FALSE;	
	}

	if ( Fread( handle, file_size, degas) != file_size)
	{
		free( degas);
		Fclose( handle);
		return FALSE;	
	}

	Fclose( handle);

	info->planes = 1 << ( 2 - degas->resolution);

	switch( info->planes)
	{
		case 4:
			info->width 		= 320;
			info->height 		= 200;
			info->components	= 3;
			info->indexed_color	= TRUE;
			info->colors  		= 16;
			break;

		case 2:
			info->width 		= 640;
			info->height 		= 200;
			info->components	= 3;
			info->indexed_color	= TRUE;
			info->colors  		= 4;
			break;

		case 1:
			info->width 		= 640;
			info->height 		= 400;
			info->components	= 1;
			info->indexed_color	= FALSE;
			info->colors  		= 2;
			break;

		default:
			free( degas);
			return FALSE;		
	}

	position = sizeof( DEGASHDR);

	info->real_width			= info->width;
	info->real_height			= info->height;
	info->memory_alloc 			= TT_RAM;
	info->page	 				= 1;
	info->delay					= 0;
	info->orientation			= UP_TO_DOWN;
	info->num_comments			= 0;
	info->max_comments_length 	= 0;
	info->_priv_ptr				= ( void*)degas;
	info->_priv_var_more		= position;		

	strcpy( info->info, "Degas");	

	if( degas->compression)
	{	
		uint8 *compress_buffer	= malloc( 320L);
		
		if( compress_buffer == NULL)
		{
			free( degas);
			return FALSE;
		}

		info->_priv_ptr_more	= ( void*)compress_buffer;		

		strcpy( info->compression, "Yes");	
	}
	else
	{
		strcpy( info->compression, "None");
		info->_priv_ptr_more	= NULL;
	}


	if( info->indexed_color)
	{
		register int16 i;

		for ( i = 0; i < info->colors; i++)
		{
			info->palette[i].red   = ((( degas->palette[i] >> 7) & 0xE) + (( degas->palette[i] >> 11) & 0x1)) * 0x11;
			info->palette[i].green = ((( degas->palette[i] >> 3) & 0xE) + (( degas->palette[i] >> 7) & 0x1))  * 0x11;
			info->palette[i].blue  = ((( degas->palette[i] << 1) & 0xE) + (( degas->palette[i] >> 3) & 0x1))  * 0x11;
		}
	}

	return TRUE;
}


static int16 read_line( int16 bpl, uint8 *src, uint8 *compress_buffer)
{
    int16	x, n, c;
	int16	count = 0;

    /* decompress the line */
    for( x = 0; x < bpl;)
    {
        n = src[count++];

        if( n <= 127)           /* (n+1) literal bytes */
        {
            n++;

			if( x + n > bpl)
				return -1;

			do 
			{
				compress_buffer[x++] = src[count++];
			}while ( --n > 0);
        }
        else                    /* next byte is repeated (257-n) times */
        {
			n = 257 - n;

			if( x + n > bpl)
				return -1;

			c = src[count++];

            do {
                compress_buffer[x++] = c;
            } while (--n > 0);
        }
    }

    return count;
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
	DEGASHDR	*degas	= ( DEGASHDR*)info->_priv_ptr;
	uint8 		*l, *s = ( uint8*)degas + info->_priv_var_more;
	uint8		*compress_buffer = ( uint8*)info->_priv_ptr_more;
	uint16		x, c, p0, p1, p2, p3;
	int16		count;

	if( degas->compression)
	{	
		count =  read_line(( info->width * info->planes) >> 3, s, compress_buffer);

		if( count == -1)
			return FALSE;

		info->_priv_var_more += count;

		switch( info->planes)
		{	
			case 4:
				for( x = 0, l = compress_buffer; x < info->width; l += 2)
				{
            		p0 = (l[  0] << 8) | l[  1];
            		p1 = (l[ 40] << 8) | l[ 41];
            		p2 = (l[ 80] << 8) | l[ 81];
            		p3 = (l[120] << 8) | l[121];

			        for ( c = 0; c < 16; c++)
			        {
			     	   buffer[x++] = ((p0 >> 15) & 1) | ((p1 >> 14) & 2) | ((p2 >> 13) & 4) | ((p3 >> 12) & 8);
			           p0 <<= 1;
			           p1 <<= 1;
			           p2 <<= 1;
			           p3 <<= 1;
			        }
				}
				break;

			case 2:
				for ( x = 0, l = compress_buffer; x < info->width; l += 2)
			    {
			    	p0 = (l[0] << 8) | l[1];
			    	p1 = (l[80] << 8) | l[81];

			    	for (c = 0; c < 16; c++)
			        {
			    	    buffer[x++] = ((p0 >> 15) & 1) | ((p1 >> 14) & 2);
			            p0 <<= 1;
			            p1 <<= 1;
			        }
			    }
				break;			

			case 1:
		        for (x = 0, l = compress_buffer; x < info->width; l += 2)
		        {
		            p0 = ( l[0] << 8) | l[1];
		
		            for ( c = 0; c < 16; c++)
		            {
		                buffer[x++] = ((p0 >> 15) & 1);
		                p0 <<= 1;
		            }
		        }
				break;
		}
		
		return TRUE;
	}

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
	DEGASHDR	*degas			 = ( DEGASHDR*)info->_priv_ptr;
	uint8		*compress_buffer = ( uint8*)info->_priv_ptr_more;

	free( degas);

	if( compress_buffer)
		free( compress_buffer);
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
	ldg_init( &degas_plugin);
	return( 0);
}
