#include <stdlib.h>
#include <gif_lib.h>
#include "zview.h"

/* Prototypes */
boolean CDECL reader_init( const char *name, IMGINFO info);
void    CDECL reader_get_txt( IMGINFO info, txt_data *txtdata);
boolean CDECL reader_read( IMGINFO info, uint8 *buffer);
uint8* 	CDECL reader_get_page_addr( IMGINFO info);
void    CDECL reader_quit( IMGINFO info);
void 	CDECL init( void);


PROC GIFFunc[] = 
{
	{ "plugin_init",    		"", init},	
	{ "reader_init",    		"", reader_init},
	{ "reader_get_txt", 		"", reader_get_txt},
	{ "reader_read",    		"", reader_read},
	{ "reader_quit",    		"", reader_quit}
};


LDGLIB gif_plugin =
{
	0x0200, 		/* Plugin version */
	5,				/* Number of plugin's functions */
	GIFFunc,		/* List of functions */
	"GIF",			/* File's type Handled */
	LDG_NOT_SHARED, /* The flags NOT_SHARED is used here.. even if zview plugins are reentrant 
					   and are shareable, we must use this flags because we don't know if the 
					   user has ldg.prg deamon installed on his computer */
	libshare_exit,			/* Function called when the plugin is unloaded */
	1L				/* Howmany file type are supported by this plugin */
};



/*==================================================================================*
 * void decodecolormap:																*
 *		Convert 8 bits + palette data to standard RGB								*
 *----------------------------------------------------------------------------------*
 * input:																			*
 *		src			->	The 8 bits source.											*
 *		dst			->	The destination where put the RGB data.						*
 *		cm			->	The source's colormap.										*
 *		width		->	picture's width.											*
 *----------------------------------------------------------------------------------*
 * return:	 																		*
 *     --																			*
 *==================================================================================*/
static inline void decodecolormap( uint8 *src, uint8 *dst, GifColorType *cm, uint16 width, int16 trans_index, uint32 transparent_color, int16 draw_trans)
{
	GifColorType *cmentry;
	register uint16 i;

	for( i = 0; i < width; i++)
	{
		if( src[i] == trans_index)
		{		
			if( draw_trans == FALSE)
			{
				dst += 3;
				continue;	
			}
				
			*(dst++) = ( ( transparent_color >> 16) & 0xFF);
			*(dst++) = ( ( transparent_color >> 8) & 0xFF);
			*(dst++) = ( ( transparent_color) & 0xFF);
		}
		else
		{
			cmentry		=	&cm[src[i]];
			*(dst++)	=	cmentry->Red;
			*(dst++)	=	cmentry->Green;
			*(dst++)	=	cmentry->Blue;
		}
	}
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
	static int16 	InterlacedOffset[] = { 0, 4, 2, 1 }, InterlacedJumps[] = { 8, 8, 4, 2 };  
	int16			i, j, error = 0, transpar = -1, interlace = FALSE, draw_trans = TRUE;
	uint16			delaycount	= 0;
	GifFileType		*gif		= NULL;
	GifRecordType 	rec;
	txt_data		comment		= { 0,};
	img_data		img			= { 0,};

	if ( ( gif = DGifOpenFileName ( name, NULL)) == NULL)
		return FALSE;

	img.imagecount = 0;	
		
	do 
	{
		if( DGifGetRecordType (gif, &rec) == GIF_ERROR) 
		{
			error = 1;
			break;
		}
			
		if ( rec == IMAGE_DESC_RECORD_TYPE) 
		{
			if ( DGifGetImageDesc( gif) == GIF_ERROR) 
			{
				error = 1;
				break;
			}					
			else 
			{
				GifImageDesc *dsc	= &gif->Image;
				ColorMapObject *map	= ( dsc->ColorMap ? dsc->ColorMap : gif->SColorMap);
				int Row, Col, Width, Height;
				int32 line_size;
				uint8 *line_buffer	= NULL, *img_buffer;

				if( map == NULL)
				{
					error = 1;
					break;
				}

				Row 	= dsc->Top; 	/* Image Position relative to Screen. */
				Col 	= dsc->Left;
				Width 	= dsc->Width;
				Height 	= dsc->Height;
				
				/* if ( Col + Width > gif->SWidth || Row + Height > gif->SWidth) 
				{
					error = 1;
					break;
				}
				*/				
				info->components	= ( map->BitsPerPixel == 1 ? 1 : 3);
				info->planes   		= map->BitsPerPixel;
				info->colors  		= map->ColorCount;
				info->width 		= ( uint16)gif->SWidth;
				info->height 		= ( uint16)gif->SHeight;
				line_size			= ( int32)gif->SWidth * ( int32)info->components;

				if( dsc->Interlace) 
					interlace = TRUE;
					
				img.image_buf[img.imagecount] = ( uint8*)malloc( line_size * ( gif->SHeight + 2));
				img_buffer	= img.image_buf[img.imagecount];
					
				if( img_buffer == NULL)
				{
					error = 1;
					break;
				}

				if( map->BitsPerPixel != 1)
				{
					line_buffer	= ( uint8*)malloc( line_size + 128); 

					if( line_buffer == NULL)
					{
						free( ( void*)img.image_buf[img.imagecount]);
						error = 1;
						break;
					}
				}

				if( img.imagecount > 0)
				{
					/* the transparency is the background picture */
					draw_trans = FALSE;

					if(( Row > 0 && Height < gif->SHeight) || ( Col > 0 && Width < gif->SWidth))
						memcpy( img_buffer, img.image_buf[img.imagecount - 1], line_size * gif->SHeight);
					else draw_trans = TRUE;
				}
												
				if( interlace == TRUE)
				{
					for( i = 0; i < 4; i++)
					{
						for ( j = Row + InterlacedOffset[i]; j < Row + Height; j += InterlacedJumps[i])
						{
							if( map->BitsPerPixel != 1)
							{
								if( DGifGetLine ( gif, line_buffer, Width) != GIF_OK)
								{
									error = 1;
									free( ( void*)img.image_buf[img.imagecount]);
									break;
								}

								decodecolormap( line_buffer, &img_buffer[(j * line_size) + (Col * 3)], map->Colors, Width, transpar, info->background_color, draw_trans);
							}
							else if( DGifGetLine ( gif, &img_buffer[(j * line_size) + Col], Width) != GIF_OK)
							{
								free( ( void*)img.image_buf[img.imagecount]);
								error = 1;
								break;
							}
						}
					}	
				}
				else
				{
					for( i = 0; i < Height; i++, Row++)
					{
						if( map->BitsPerPixel != 1)
						{
							if( DGifGetLine( gif, line_buffer, Width) != GIF_OK)
							{
								error = 1;
								free( ( void*)img.image_buf[img.imagecount]);
								break;
							}

							decodecolormap( line_buffer, &img_buffer[(Row * line_size) + (Col * 3)], map->Colors, Width, transpar, info->background_color, draw_trans);
						}
						else if( DGifGetLine( gif, &img_buffer[(Row * line_size) + Col], Width) != GIF_OK)
						{
							free( ( void*)img.image_buf[img.imagecount]);
							error = 1;
							break;
						}
					}
				}
									
				if( line_buffer)
				{
					free( ( void*)line_buffer);
					line_buffer = NULL;
				}	

				img.imagecount++;
			}
			
			if( info->thumbnail	== TRUE)		
				break;
		
		} 
		else if ( rec == EXTENSION_RECORD_TYPE) 
		{
			int           code;
			GifByteType * block;

			if ( DGifGetExtension (gif, &code, &block) == GIF_ERROR) 
			{
				error = 1;
				break;
			}		
			else while ( block != NULL) 
			{
				switch( code)
				{
				   	case COMMENT_EXT_FUNC_CODE: 
				   		if( comment.lines > 254)
				   			break;
				   	
						block[block[0]+1] = '\000';   /* Convert gif's pascal-like string */
						comment.txt[comment.lines] = ( int8*)malloc( block[0] + 1);

						if( comment.txt[comment.lines] == NULL)
							break;

						strcpy( comment.txt[comment.lines], (char*)block + 1);
						comment.max_lines_length = MAX( comment.max_lines_length, ( int16)strlen( comment.txt[comment.lines]) + 1);
						comment.lines++;
		    			break; 

				   	case GRAPHICS_EXT_FUNC_CODE: 
						if( block[1] & 1)
							transpar = ( uint16)block[4];
						else
				   			transpar = -1;
					   							   	
						img.delay[delaycount++] = ( block[2] + (block[3] << 8)) << 1;				

		    			break;
					/*
					In version 2.0 beta Netscape, introduce a special extention for
					set a maximum loop playback.. Netscape itself doesn't use it anymore
					and play the animation infinitly... so we don't care about it.

					case APPLICATION_EXT_FUNC_CODE:
						if( reading_netscape_ext == 0)
						{
							if( memcmp( block + 1, "NETSCAPE", 8) != 0) 
								break;

							if( block[0] != 11) 
								break;

							if( memcmp( block + 9, "2.0", 3) != 0)
								break;

	  						reading_netscape_ext = 1;
						}
						else
						{
							if (( block[1] & 7) == 1)
								info->max_loop_count = block[2] | ( block[3] << 8);
						}
					*/
					default:
						break;
				}

				if ( DGifGetExtensionNext (gif, &block) == GIF_ERROR) 
				{
					error = 1;
					break;
				}	
			}
		}
		else break;
	} while ( rec != TERMINATE_RECORD_TYPE);

	
	if( error) 
	{
		for ( i = 0; i < comment.lines; i++) 
		{
			if( comment.txt[i])
				free( ( void*)comment.txt[i]);
		}

		for ( i = 0; i < img.imagecount; i++) 
		{
			if( img.image_buf[i])
				free( ( void*)img.image_buf[i]);
		}	

		DGifCloseFile ( gif, NULL);
		return FALSE;
	}

	info->real_height  			= info->height;
	info->real_width 			= info->width;
	info->orientation			= UP_TO_DOWN;
	info->page	 				= img.imagecount;
	info->num_comments			= comment.lines;
	info->max_comments_length	= comment.max_lines_length;
	info->indexed_color 		= FALSE;
	info->memory_alloc			= TT_RAM;
	info->_priv_var				= 0L;	/* page line counter */
	info->_priv_var_more		= 0L;	/* current page returned */
	info->_priv_ptr				= ( void*)&comment;
	info->_priv_ptr_more		= ( void*)&img;

	strcpy( info->info, "GIF");

	/*if( gif->Version)
		strcat( info->info, "89a"); 
	else
		strcat( info->info, "87a"); 
	*/

	if( interlace)
		strcat( info->info, " (Interlaced)"); 

	strcpy( info->compression, "LZW");	

	DGifCloseFile( gif, NULL);
	
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
	register int16 i;
	txt_data *comment	= ( txt_data *)info->_priv_ptr;

	for ( i = 0; i < txtdata->lines; i++) 
		strcpy( txtdata->txt[i] , comment->txt[i]);
}


/*==================================================================================*
 * boolean CDECL reader_read:														*
 *		This function fits the buffer with image data								*
 *----------------------------------------------------------------------------------*
 * input:																			*
 *		buffer		->	The destination buffer.										*
 *		info		->	The IMGINFO struct.											*
 *----------------------------------------------------------------------------------*
 * return:	 																		*
 *      TRUE if all ok else FALSE.													*
 *==================================================================================*/
boolean CDECL reader_read( IMGINFO info, uint8 *buffer)
{
	img_data	*img		= ( img_data*)info->_priv_ptr_more;
	int32		line_size	= ( int32)info->width * ( int32)info->components;
	uint8		*line_src;

	if( ( int32)info->page_wanted != info->_priv_var_more)
	{
		info->_priv_var_more 	= ( int32)info->page_wanted;
		info->_priv_var 		= 0L;
	}

	line_src	= &img->image_buf[info->page_wanted][info->_priv_var];

	info->_priv_var += line_size;
	info->delay = img->delay[info->page_wanted];

	memcpy( buffer, line_src, line_size);

	return TRUE;
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
	txt_data 	*comment	= ( txt_data *)info->_priv_ptr;
	img_data	*img		= ( img_data *)info->_priv_ptr_more;	
	register int16 i;

	for ( i = 0; i < comment->lines; i++) 
	{
		if( comment->txt[i])
			free( ( void*)comment->txt[i]);
	}
	
	for ( i = 0; i < img->imagecount; i++) 
	{
		if( img->image_buf[i])
			free( ( void*)img->image_buf[i]);
	}	
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
	ldg_init( &gif_plugin);
	return( 0);
}
