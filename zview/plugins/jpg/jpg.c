#include <stdlib.h>
#include <stdio.h>
#if defined(PLUGIN_SLB) && defined(JPEG_SLB)
#include <slb/jpeg.h>
#else
#include <jpeglib.h>
#endif

#if defined(PLUGIN_SLB) && defined(EXIF_SLB)
#include <slb/exif.h>
#else
#include <libexif/exif-data.h>
#endif

#include "plugin.h"
#include "zvplugin.h"
#include "zvjpg.h"
#include <gem.h>
#include <mint/cookie.h>
#include "jpgdh.h"
#include "jpgdsp.h"

/* Options*/
int	quality = 90; /* quality 0 -> 100 % */
J_COLOR_SPACE color_space = JCS_RGB;
boolean progressive = FALSE;

#undef VERSION
#define VERSION 0x200
#define AUTHOR "Zorro"


#ifdef PLUGIN_SLB
long __CDECL get_option(zv_int_t which)
{
	switch (which)
	{
	case OPTION_CAPABILITIES:
		return CAN_DECODE | CAN_ENCODE;
	case OPTION_EXTENSIONS:
		return (long)("JPG\0JPE\0JPEG\0");
	case OPTION_QUALITY:
		return quality;
	case OPTION_COLOR_SPACE:
		return color_space;
	case OPTION_PROGRESSIVE:
		return progressive;
	}
	return -ENOSYS;
}


long __CDECL set_option(zv_int_t which, zv_int_t value)
{
	switch (which)
	{
	case OPTION_CAPABILITIES:
	case OPTION_EXTENSIONS:
		return -EACCES;
	case OPTION_QUALITY:
		if (value < 0 || value > 100)
			return -ERANGE;
		quality = value;
		return value;
	case OPTION_COLOR_SPACE:
		if (value < JCS_UNKNOWN || value >
#if JPEG_LIB_VERSION >= 90
			JCS_BG_YCC
#else
			JCS_YCCK
#endif
			)
			return -ERANGE;
		color_space = value;
		return value;
	case OPTION_PROGRESSIVE:
		progressive = value != 0;
		return value;
	}
	return -ENOSYS;
}
#endif


typedef struct
{
	struct	jpeg_source_mgr pub;   /* public fields */
	uint8_t	*buffer;
	int		size;
	JOCTET	terminal[2];
} my_source_mgr;

/* Brainstorm DSP driver cookie pointer */
static boolean dsp_decoding = FALSE;

typedef struct jpeg_decompress_struct 	*JPEG_DEC;
typedef struct jpeg_error_mgr         	*JPEG_ERR;
typedef struct jpeg_compress_struct		*JPEG_ENC;
typedef my_source_mgr *my_src_ptr;



typedef struct
{
	long key;
	long value;
} COOKIE;


static COOKIE *get_cookie_jar(void)
{
	return (COOKIE *) Setexc(360, (void (*)()) -1L);
}


static int get_cookie(long key, long *value)
{
	COOKIE *cookiejar = get_cookie_jar();

	*value = 0;
	if (cookiejar)
	{
		while (cookiejar->key)
		{
			if (cookiejar->key == key)
			{
				*value = cookiejar->value;
				return 1;
			}
			cookiejar++;
		}
	}
	return 0;
}



static void _jpeg_shutup( j_common_ptr cinfo, int msg_level)
{
	(void)cinfo;
	(void)msg_level;
}


static void _jpeg_errjmp( j_common_ptr cinfo)
{
	jmp_buf *escape = cinfo->client_data;
	
	if ( escape)
		longjmp( *escape, TRUE);
}

static void init_source (j_decompress_ptr cinfo)
{
}


static boolean fill_input_buffer (j_decompress_ptr cinfo)
{
	my_src_ptr src = (my_src_ptr) cinfo->src;

	src->pub.next_input_byte = src->terminal;
	src->pub.bytes_in_buffer = 2;
	src->terminal[0]         = (JOCTET) 0xFF;
	src->terminal[1]         = (JOCTET) JPEG_EOI;

	return TRUE;
}

static void skip_input_data( j_decompress_ptr cinfo, long num_bytes)
{
	my_src_ptr src = (my_src_ptr) cinfo->src;
 	src->pub.next_input_byte = src->pub.next_input_byte + num_bytes;
}

static void term_source (j_decompress_ptr cinfo)
{
}


static boolean decompress_thumbnail_image( void *source, uint32_t size, IMGINFO info)
{
	int 		header = 0;
	jmp_buf 	escape;
	JPEG_ERR 	jerr;
	JPEG_DEC 	jpeg;	
 	my_src_ptr  src;

 	jpeg 	= ( JPEG_DEC) malloc( sizeof( struct jpeg_decompress_struct));
	jerr 	= ( JPEG_ERR) malloc( sizeof( struct jpeg_error_mgr));

	if ( jpeg == NULL || jerr == NULL) 
	{		
		if ( jerr) 		free( jerr);
		if ( jpeg) 		free( jpeg);
		return FALSE;
	}
	
	 	  
	if( setjmp( escape))
	{						
		if ( header == 2)
			jpeg_abort_decompress( jpeg);
			
		jpeg_destroy_decompress( jpeg);
		if ( jerr) 		free( jerr);
		if ( jpeg) 		free( jpeg);
		return FALSE;
	}

	jpeg->err               = jpeg_std_error( jerr);
	jpeg->err->emit_message = _jpeg_shutup;
	jpeg->err->error_exit   = _jpeg_errjmp;
	jpeg->client_data   	= &escape;			
	
	jpeg_create_decompress( jpeg);

	if (jpeg->src == NULL)
    	jpeg->src = (struct jpeg_source_mgr *)(*jpeg->mem->alloc_small)((j_common_ptr)jpeg, JPOOL_PERMANENT, sizeof (my_source_mgr));

    src = (my_src_ptr)jpeg->src;
      	
	src->pub.init_source		= init_source;
	src->pub.fill_input_buffer	= fill_input_buffer;
	src->pub.skip_input_data	= skip_input_data;
	src->pub.resync_to_restart	= jpeg_resync_to_restart;
	src->pub.term_source		= term_source;
	src->pub.bytes_in_buffer	= size;
	src->pub.next_input_byte	= source;
	src->buffer 				= source;
	src->size					= size;

	jpeg_read_header( jpeg, TRUE);

	jpeg->dct_method 			= JDCT_IFAST;
	jpeg->do_fancy_upsampling 	= FALSE;

	header = 1;
	jpeg_start_decompress( jpeg);
	header = 2;

	switch( jpeg->out_color_space) 
	{
		case JCS_RGB:
			info->components = 3;
			break;
		case JCS_GRAYSCALE:
			info->components = 1;
			break;
		default: 
			jpeg->out_color_space = JCS_RGB;
			info->components = jpeg->out_color_components = 3;
			break;
	}
	
	info->width   				= jpeg->image_width;
	info->height  				= jpeg->image_height;
	info->planes   				= ( ( jpeg->out_color_space == JCS_RGB) ? 24 : 8);
	info->colors  				= 1 << info->planes;

	info->_priv_ptr				= ( void*)jpeg;			
	info->_priv_var_more		= 1;
	info->_priv_var				= -1;
	
	jpeg->client_data   		= NULL;		

	return TRUE;
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
	JPEG_ERR 				jerr;
	JPEG_DEC 				jpeg;		   
	jpeg_saved_marker_ptr 	mark;
	txt_data				*comment;
	jmp_buf 				escape;
	FILE* 					jpeg_file;
	int16_t 				header = 0;
	
	/* If Brainstorm cookie is used, we try to decode with it*/
	if( jpgdrv)
	{
		int16_t dsp_result = reader_dsp_init( name, info);

		switch( dsp_result)
		{
			case GLOBAL_ERROR:
				dsp_decoding = FALSE;
				return FALSE;
			
			case DSP_ERROR:
			case PROGRESSIVE_JPG:
				dsp_decoding = FALSE;
				break;

			case ALL_OK:
				dsp_decoding = TRUE;
				return TRUE;
		}
	}

	if ( ( jpeg_file = fopen( name, "rb")) == NULL)
		return FALSE;

	jpeg 	= ( JPEG_DEC) malloc( sizeof( struct jpeg_decompress_struct));
	jerr 	= ( JPEG_ERR) malloc( sizeof( struct jpeg_error_mgr));
	comment = ( txt_data*)malloc( sizeof( txt_data));

	if ( jpeg == NULL || jerr == NULL || comment == NULL) 
	{
		if ( comment) 	free( comment);		
		if ( jerr) 		free( jerr);
		if ( jpeg) 		free( jpeg);
		fclose( jpeg_file);
		return FALSE;
	}

	comment->lines 				= 0;
	comment->max_lines_length 	= 0;
	comment->txt[0] 			= NULL;

	jpeg->err               = jpeg_std_error( jerr);
	jpeg->err->emit_message = _jpeg_shutup;
	jpeg->err->error_exit   = _jpeg_errjmp;
	jpeg->client_data   	= &escape;		

	if ( setjmp( escape))
	{
		if ( header >= 2)
			jpeg_abort_decompress( jpeg);

		jpeg_destroy_decompress( jpeg);

		if ( comment)
		{	
			register int16_t i;			

			for ( i = 0; i < comment->lines; i++) 
			{
				if( comment->txt[i])
					free( comment->txt[i]);
			}

		 	free( comment);
			comment	= NULL;	
		}
	
		if ( jerr) 		free( jerr);
		if ( jpeg) 		free( jpeg);
		fclose( jpeg_file);
		return FALSE;
	}
			
	jpeg_create_decompress( jpeg);
	jpeg_stdio_src( jpeg, jpeg_file);
    jpeg_save_markers( jpeg, JPEG_COM,  0xffff); /* comment */
    jpeg_save_markers( jpeg, M_EXIF,    0xFFFF); /* EXIF */
	jpeg_read_header( jpeg, TRUE);

	
	/* faster that the default method ( 10%) for a very little output quality lost */
	jpeg->dct_method 			= JDCT_IFAST;
	jpeg->do_fancy_upsampling 	= FALSE;
	
	header = 1;
	jpeg_start_decompress( jpeg);
	header = 2;
		
	jpeg->client_data = NULL;
	
	switch( jpeg->out_color_space) 
	{
		case JCS_RGB:
			info->components = 3;
			break;
		case JCS_GRAYSCALE:
			info->components = 1;
			break;
		default: 
			jpeg->out_color_space = JCS_RGB;
			info->components = jpeg->out_color_components = 3;
			break;
	}

	info->real_width 	= jpeg->image_width;
	info->real_height	= jpeg->image_height;
	info->width   		= info->real_width;
	info->height  		= info->real_height;
	info->planes   		= ( ( jpeg->out_color_space == JCS_RGB) ? 24 : 8);
	info->orientation 	= UP_TO_DOWN;
	info->colors  		= 1 << info->planes;
	info->memory_alloc 	= TT_RAM;
	info->indexed_color = FALSE;
	info->page	 		= 1;
	info->delay		 	= 0;

	strcpy( info->info, "JPEG");	

	if( info->planes == 8)
		strcat( info->info, " Greyscale");

	strcpy( info->compression, "JPG");	
	
    for ( mark = jpeg->marker_list; mark; mark = mark->next)
	{
		if( mark->marker == JPEG_COM)
		{
			if (!mark->data || !mark->data_length)
	               continue;
		
			comment->txt[comment->lines] = ( int8_t*)malloc( mark->data_length + 1);

			if( comment->txt[comment->lines] == NULL)
				continue;

			memcpy( comment->txt[comment->lines], mark->data, mark->data_length);
			comment->max_lines_length = MAX( comment->max_lines_length, ( int16_t)mark->data_length + 1);
			comment->lines++;
		}
		else if( mark->marker == M_EXIF)
		{
			int i, l, length;
			char value[1024+256];
			ExifData *exifData = exif_data_new_from_data(( char*)mark->data, mark->data_length);

		    if( !exifData)
				continue;

#ifdef EXIF_SLB
			if (get_slb_funcs()->p_slb_open(LIB_EXIF) < 0)
			{
				exif_data_unref( exifData);
				continue;
			}
#endif
			for( i = 0; i < EXIF_IFD_COUNT; i++)
			{
				ExifContent* content = exifData->ifd[i];

				for ( l = 0; l < content->count; l++)
				{
					const char *tag;
					
					if( comment->lines > 253)
						break;

					ExifEntry *e = content->entries[l];

					tag = exif_tag_get_name (e->tag);
					if (tag)
						strcpy( value, tag);
					else
						sprintf(value, "0x%x", e->tag);

					strcat( value, ": ");

					length = strlen( value);

					exif_entry_get_value( e, &value[length], sizeof(value) - length);

					length = strlen( value);
					
					comment->txt[comment->lines] = ( int8_t*)malloc( length + 1);

					if( comment->txt[comment->lines] == NULL)
						break;

					strcpy( comment->txt[comment->lines], value);

					comment->max_lines_length = MAX( comment->max_lines_length, ( int16_t)length);					
					comment->lines++;
				}
			}

			if(( exifData->data) && ( exifData->size > 0) && info->thumbnail)
			{
				int32_t size = exifData->size;

				info->__priv_ptr_more = malloc( size + 1);
				memcpy( info->__priv_ptr_more, exifData->data, size);

				exif_data_unref( exifData);

				jpeg_finish_decompress( jpeg);
				jpeg_destroy_decompress( jpeg);
				free( jpeg->err);
				free( jpeg);
				fclose( jpeg_file);

				if( decompress_thumbnail_image( info->__priv_ptr_more, size, info))
				{
					info->num_comments			= comment->lines;
					info->max_comments_length	= comment->max_lines_length;
					info->_priv_ptr_more		= ( void*)comment;
					
#ifdef EXIF_SLB
					get_slb_funcs()->p_slb_close(LIB_EXIF);
#endif
					return TRUE;
				}
				else /* We can't extract the thumbnail :/ */
				{
					free( info->__priv_ptr_more);

					if ( comment)
					{
						for ( i = 0; i < comment->lines; i++)
						{
							if( comment->txt[i])
								free( comment->txt[i]);
						}

					 	free( comment);
						comment	= NULL;
					}
					
#ifdef EXIF_SLB
					get_slb_funcs()->p_slb_close(LIB_EXIF);
#endif
					return FALSE;
				}
			}

			exif_data_unref( exifData);
#ifdef EXIF_SLB
			get_slb_funcs()->p_slb_close(LIB_EXIF);
#endif
		}
    }

	info->num_comments			= comment->lines;
	info->max_comments_length	= comment->max_lines_length;

	
	info->_priv_ptr				= ( void*)jpeg;			
	info->_priv_ptr_more		= ( void*)comment;
	info->__priv_ptr_more		= NULL;
	info->_priv_var				= (int32_t)jpeg_file;		
	info->_priv_var_more		= 0; /* 1 for exif thumbnail */

	jpeg->client_data = NULL;
	
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
	register int16_t 	i;
	txt_data 		*comment;

	if( dsp_decoding)
		return;

	comment = ( txt_data *)info->_priv_ptr_more;

	for ( i = 0; i < txtdata->lines; i++) 
		strcpy( txtdata->txt[i] , comment->txt[i]);
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
	if( dsp_decoding)
	{	
		const void *source = ( uint8_t*)info->_priv_ptr + info->_priv_var_more;
	
		memcpy( ( void*)buffer, source, info->_priv_var);
		info->_priv_var_more += info->_priv_var;

		return TRUE;
	}

	if( jpeg_read_scanlines(( JPEG_DEC)info->_priv_ptr, ( JSAMPROW*)&buffer, 1))
		return TRUE;

	return FALSE;
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
	JPEG_DEC	jpeg;	
	txt_data 	*comment;	
	
	if( dsp_decoding)
	{
		if( info->_priv_ptr)
		{
		        /* DSP decoder uses Mxalloc() */
			Mfree( info->_priv_ptr);
                }
		return;
	}

	jpeg	= ( JPEG_DEC)info->_priv_ptr;
	comment = ( txt_data *)info->_priv_ptr_more;

	if( !jpeg)
		return;

	if( comment)
	{
		register int16_t i;

		for ( i = 0; i < comment->lines; i++)
		{
			if( comment->txt[i])
				free( comment->txt[i]);
		}
	 	free( comment);
	}

	jpeg_finish_decompress( jpeg);
	jpeg_destroy_decompress( jpeg);
	
	free( jpeg->err);
	free( jpeg);
	
	/* thumbnail mode? */	
	if( info->__priv_ptr_more)
	{
		free( info->__priv_ptr_more);
		return;
	}

	fclose( (FILE *)info->_priv_var);
}



/*==================================================================================*
 * boolean __CDECL encoder_init:													*
 *		Open the file "name", fit the "info" struct. ( see zview.h) and make others	*
 *		things needed by the encoder.												*
 *----------------------------------------------------------------------------------*
 * input:																			*
 *		name		->	The file to open.											*
 *		info		->	The IMGINFO struct. to fit.									*
 *----------------------------------------------------------------------------------*
 * return:	 																		*
 *      TRUE if all ok else FALSE.													*
 *==================================================================================*/
boolean __CDECL encoder_init( const char *name, IMGINFO info)
{
	JPEG_ERR 	jerr = NULL;
	JPEG_ENC 	jpeg = NULL;		   
	FILE* 		jpeg_file;
	jmp_buf 	escape;
	int16_t 		header = 0;

	if ( ( jpeg_file = fopen( name, "wb")) == NULL)
		return FALSE;

	jpeg 	= ( JPEG_ENC) malloc( sizeof( struct jpeg_decompress_struct));
	jerr 	= ( JPEG_ERR) malloc( sizeof( struct jpeg_error_mgr));

	if ( jpeg == NULL || jerr == NULL) 
	{
		if ( jerr) 		free( jerr);
		if ( jpeg) 		free( jpeg);
		fclose( jpeg_file);
		return FALSE;
	}
	
	if ( setjmp( escape)) 
	{
		if ( header > 1)
			jpeg_abort_compress( jpeg);

		jpeg_destroy_compress( jpeg);

		if ( jerr) 		free( jerr);
		if ( jpeg) 		free( jpeg);
		jerr 	= NULL;
		jpeg 	= NULL;
		fclose( jpeg_file);
		return FALSE;
	}

	jpeg->err               = jpeg_std_error( jerr);
	jpeg->err->emit_message = _jpeg_shutup;
	jpeg->err->error_exit   = _jpeg_errjmp;
	jpeg->client_data   	= &escape;	
	
	jpeg_create_compress( jpeg);
	jpeg_stdio_dest( jpeg, jpeg_file);

  	jpeg->image_width 		= info->width;
  	jpeg->image_height 		= info->height;
  	jpeg->input_components 	= info->components = 3;
	jpeg->in_color_space 	= JCS_RGB;

	jpeg_set_defaults( jpeg);
	jpeg_set_colorspace ( jpeg, color_space);
	
/*	jpeg->num_components 	= ( color_space == JCS_RGB ? 3 : 1);
	jpeg->jpeg_color_space 	= color_space;
*/	jpeg->progressive_mode	= progressive;

	jpeg_set_quality( jpeg, quality, TRUE);

	header = 1;
	jpeg_start_compress( jpeg, TRUE);
	header = 2;

	info->planes   			= 24;
	info->colors  			= 16777215L;
	info->orientation		= UP_TO_DOWN;
	info->real_width			= info->width;
	info->real_height			= info->height;
	info->memory_alloc 			= TT_RAM;
	info->indexed_color	 	= FALSE;
	info->page			 	= 1;
	info->delay 		 	= 0;
	info->_priv_ptr	 		= ( void*)jpeg;
	info->_priv_ptr_more	= NULL;				
	info->__priv_ptr_more	= NULL;	
	info->_priv_var	 		= (int32_t)jpeg_file;
	info->_priv_var_more	= 0;

	jpeg->client_data  		= NULL;

	return TRUE;
}


/*==================================================================================*
 * boolean __CDECL encoder_write:													*
 *		This function write data from buffer to file								*
 *----------------------------------------------------------------------------------*
 * input:																			*
 *		buffer		->	The source buffer.											*
 *		info		->	The IMGINFO struct		.									*
 *----------------------------------------------------------------------------------*
 * return:	 																		*
 *      TRUE if all ok else FALSE.													*
 *==================================================================================*/
boolean __CDECL encoder_write( IMGINFO info, uint8_t *buffer)
{
	( void)jpeg_write_scanlines( ( JPEG_ENC)info->_priv_ptr, ( JSAMPROW*)&buffer, 1);
	return TRUE;
}



/*==================================================================================*
 * boolean __CDECL encoder_quit:													*
 *		This function makes the last job like close the file handler and free the	*
 *		allocated memory.															*
 *----------------------------------------------------------------------------------*
 * input:																			*
 *		info		->	The IMGINFO struct. to fit.									*
 *----------------------------------------------------------------------------------*
 * return:	 																		*
 *      --																			*
 *==================================================================================*/
void __CDECL encoder_quit( IMGINFO info)
{
	JPEG_ENC jpeg = ( JPEG_ENC)info->_priv_ptr;
	if( jpeg)
	{
		jpeg_finish_compress( jpeg);
		jpeg_destroy_compress( jpeg);
		free( jpeg->err);
		free( jpeg);
		fclose( (FILE *)info->_priv_var);
	}
}



/*==================================================================================*
 * boolean __CDECL init:															*
 *		First function called from zview, in this one, you can make some internal	*
 *		initialisation.																*
 *----------------------------------------------------------------------------------*
 * input:																			*
 *		--																			*
 *----------------------------------------------------------------------------------*
 * return:	 																		*
 *      --																			*
 *==================================================================================*/
void jpg_init( void)
{
	long mach = 0, cpu = 0;

	/* DSP decoder present */
    if(get_cookie( C__JPD, ( long*)&jpgdrv) == 0)
    {
		jpgdrv = NULL; 
	} else
	{
		get_cookie(C__MCH, &mach);

		/* If Aranym is detected, we decode with the DSP routine */
		/* and we can use TT ram for DSP allocation	*/
		if( mach == 0x50000L)			
		{	
			dsp_ram = 3;
			return; 
		}

		get_cookie( C__CPU, &cpu);
		cpu &= 0xFFFF;

		/* if we have a CPU > 68030, we decode with this one, it's faster */ 
		if( cpu != 30)
			jpgdrv = NULL; 
	}
}
