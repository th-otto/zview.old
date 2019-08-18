#include "general.h"
#include "pic_load.h"
#include "prefs.h"
#include "progress.h"
#include "plugins.h"
#include "mfdb.h"
#include "ztext.h"
#include "chrono.h"
#include "txt_data.h"
#include "zvdi/color.h"
#include "zvdi/raster.h"
#include "zvdi/vdi.h"
#include <math.h>
#include "resample.h"
#include "plugins/common/zvplugin.h"

boolean decoder_init_done;


void 		  ( *raster)	  			( DECDATA, void *dst);
void 		  ( *raster_cmap) 			( DECDATA, void *);
void 		  ( *raster_true) 			( DECDATA, void *);
void 		  ( *cnvpal_color)			( IMGINFO, DECDATA);
void 		  ( *raster_gray) 			( DECDATA, void *);
struct _ldg_funcs ldg_funcs;
SLB *curr_input_plugin;
SLB *curr_output_plugin;



static int16 setup ( IMAGE *img, IMGINFO info, DECDATA data)
{
	int16   i, n_planes = ( ( info->planes == 1 && info->components == 1) ?  1: app.nplanes);
	uint16	display_w, display_h;
	double	precise_x, precise_y, factor;
	size_t	src_line_size;

	if( img->view_mode && ( info->width > thumbnail[thumbnail_size][0] || info->height > thumbnail[thumbnail_size][1]) && ( !smooth_thumbnail || n_planes < 16))
	{
		factor	  = MAX( ( ( double)info->height / ( double)thumbnail[thumbnail_size][1]), ( ( double)info->width / ( double)thumbnail[thumbnail_size][0]));
		precise_x = ( double)info->width  / factor;
		precise_y = ( double)info->height / factor;

		display_w 		= ( uint16)( precise_x > 0 ? precise_x : 16);
		display_h 		= ( uint16)( precise_y > 0 ? precise_y : 16);
		data->IncXfx    = ((( uint32)info->width  << 16) + ( display_w >> 1)) / display_w;
		data->IncYfx    = ((( uint32)info->height << 16) + ( display_h >> 1)) / display_h;
	}
	else
	{
		display_w 		= info->width;
		display_h 		= info->height;
		data->IncXfx    = 0x10000uL;
		data->IncYfx	= 0x10000uL;
	}

	if( img->view_mode)
		/* if we are in preview mode, we decompress only one image */
		img->page = 1;
	else
		img->page = info->page;


	if( ( img->image = ( MFDB *)malloc( sizeof( MFDB) *  img->page)) == NULL)
		return( 0);


	for ( i = 0 ; i < img->page ; i++)	
	{
		if ( !init_mfdb( &img->image[i], display_w, display_h, n_planes))
			return( 0);
	}

	if( info->num_comments)
	{
		/* 	we initialise the txt_data struct...  */
		if( !init_txt_data( img, info->num_comments, info->max_comments_length))
			return ( 0);
		
		if (curr_input_plugin)
			plugin_reader_get_txt(curr_input_plugin, info, img->comments);
		else
			ldg_funcs.decoder_get_txt(info, img->comments);
	} else
	{
		img->comments = NULL;
	}

	/*
	 * we assume that the pixel size is minimum 8 bits because some GNU libraries return 1 and 4 bits format like 8 bits ones.
	 * We add also a little more memory for avoid buffer overflow for plugin badly coded.
	 */
	src_line_size = ( info->width + 64) * info->components;

	data->RowBuf = ( uint8*)malloc( src_line_size);	

	if( !data->RowBuf)
		return ( 0);
	
	if(( info->planes == 1 && info->components == 1) || app.nplanes > 8)
		data->DthBuf = NULL;
	else
	{
		size_t size = ( display_w + 15) * 3;
		
		if( ( data->DthBuf = malloc( size)) == NULL) 
			return ( 0);
			
		memset( data->DthBuf, 0, size);
	}

	data->DthWidth 	= display_w;
	data->PixMask  	= ( 1 << info->planes) - 1;
	data->LnSize   	= img->image[0].fd_wdwidth * n_planes;

	if ( info->planes == 1 && info->components == 1) 
	{
		cnvpal_mono( info, data);
		raster = raster_mono;
	}
	else
	{
		if ( info->indexed_color)
		{
			( *cnvpal_color)( info, data);
			raster = raster_cmap;
		}
		else
			raster = ( info->components >= 3 ? raster_true : raster_gray);
	}

	img->img_w      = info->real_width;
	img->img_h      = info->real_height;
	img->colors		= info->colors;
	img->bits   	= info->planes;

	strcpy( img->info, 			info->info);
	strcpy( img->compression, 	info->compression);	

	return( 1);
}


/*==================================================================================*
 * read_img:																		*
 *				Read a image and fit the MFDB within IMAGE structur with the data.	*
 *----------------------------------------------------------------------------------*
 * img: 		The struct. with the MFDB to be fitted.								*
 * info:		The struct. with the line and dither buffer							*
 *----------------------------------------------------------------------------------*
 * returns: 	-																	*
 *==================================================================================*/
static inline void read_img ( IMAGE *img, IMGINFO info, DECDATA data)
{
	uint16 		*dst;
	int16  		y_dst, i, y, progress_counter = 0, img_h 	= info->height;
	int32		line_size = data->LnSize, line_to_decode = img_h * img->page;
	uint8  		*buf = data->RowBuf;

	for ( i = 0; i < img->page; i++)
	{
		uint32 scale = ( data->IncYfx + 1) >> 1;
		y_dst  		 = img->image[0].fd_h;
		dst   		 = img->image[i].fd_addr;

		if( info->orientation == DOWN_TO_UP)
		{
			dst += data->LnSize * ( y_dst - 1);			
			line_size = -data->LnSize;
		}

		info->page_wanted = i;

		for( y = 1; y <= img_h; y++)
		{
			if (curr_input_plugin)
			{
				if (!plugin_reader_read(curr_input_plugin, info, buf))
					return;		
			} else
			{
				if( !ldg_funcs.decoder_read( info, buf))
					return;		
			}

			while(( scale >> 16) < y) 
			{
				( *raster)( data, dst);
				dst   += line_size;
				scale += data->IncYfx;
				if (!--y_dst) break;
			}

			if( img->progress_bar)
			{
				progress_counter++;
				win_progress(( int16)((( int32)progress_counter * 150L) / line_to_decode));
			}
		}
		
		img->delay[i]  	= info->delay;
	}
}



/*==================================================================================*
 * quit_img:																		*
 *				Close the image's file and free the line/dither/info buffer.		*
 *----------------------------------------------------------------------------------*
 * info:		The struct. to liberate												*
 *----------------------------------------------------------------------------------*
 * returns: 	-																	*
 *==================================================================================*/

void quit_img( IMGINFO info, DECDATA data)
{
	if (decoder_init_done)
	{
		if (curr_input_plugin)
			plugin_reader_quit(curr_input_plugin, info);
		else
			ldg_funcs.decoder_quit(info);
	}
	
	if( data->DthBuf != NULL) 
	   	free( data->DthBuf);

	if( data->RowBuf != NULL) 
	   free( data->RowBuf);

	free( data);
	free( info); 

	decoder_init_done = FALSE;
}


CODEC *get_codec( const char *file)
{
	int16 	i, j, c;
	char 	plugin[4];
	LDG *ldg;
	const char *p;
	int match;
	const char *dot;
	char extension[MAXNAMLEN];

	curr_input_plugin = NULL;
	dot = strrchr(file, '.');
	if (dot == NULL)
		return FALSE;
	strcpy(extension, dot + 1);
	str2upper(extension);
	plugin[3] = '\0';

	/* We check if a plug-ins can do the job */
	for( i = 0; i < plugins_nbr; i++)
	{
		switch (codecs[i].type)
		{
		case CODEC_LDG:
			ldg = codecs[i].c.ldg;
			match = FALSE;
			if (codecs[i].num_extensions == 0)
			{
				/*
				 * newer plugin, with 0-terminated list
				 */
				p = codecs[i].extensions;
				while (*p)
				{
					if (strcmp(extension, p) == 0)
					{
						match = TRUE;
						break;
					}
					p += strlen(p) + 1;
				}
			} else
			{
				/* old version, with exactly 3 chars per extension */
				for( j = 0; j < codecs[i].num_extensions; j++)
				{
					c = 0;
					plugin[0] = codecs[i].extensions[c++];
					plugin[1] = codecs[i].extensions[c++];
					plugin[2] = codecs[i].extensions[c++];
		
					if (strcmp(extension, plugin) == 0)
					{
						match = TRUE;
						break;
					}
				}
			}
			if (match)
			{
				if ( !( ldg_funcs.decoder_init 	= ldg_find( "reader_init", ldg))
				  || !( ldg_funcs.decoder_read 	= ldg_find( "reader_read", ldg)) 
				  || !( ldg_funcs.decoder_quit 	= ldg_find( "reader_quit", ldg))
				  || !( ldg_funcs.decoder_get_txt = ldg_find( "reader_get_txt", ldg)))
				{
					errshow(codecs[i].extensions, LDG_ERR_BASE + ldg_error());
					return NULL;
				}				

				/* decoder_get_page_size = ldg_find( "reader_get_page_size", ldg); */

				return &codecs[i];
			}
			break;
		case CODEC_SLB:
			match = FALSE;
			p = codecs[i].extensions;
			while (*p)
			{
				if (strcmp(extension, p) == 0)
				{
					curr_input_plugin = &codecs[i].c.slb;
					return &codecs[i];
				}
				p += strlen(p) + 1;
			}
			break;
		}
	}
	
	/* I wish that it will never happen ! */
	return NULL;
}


boolean get_pic_info( const char *file, IMGINFO info)
{
	if (get_codec(file))
	{
		if (curr_input_plugin)
			return plugin_reader_init(curr_input_plugin, file, info);
		return ldg_funcs.decoder_init(file, info);
	}
	return FALSE;
}


/*==================================================================================*
 * pic_load:																		*
 *				read a supported picture and fill a IMAGE structure.				*
 *----------------------------------------------------------------------------------*
 * file: 		 The file that will be read.										*
 * img: 		 The IMAGE struct. that will be filled. 			 				*
 *----------------------------------------------------------------------------------*
 * returns: 	'0' if error or picture not supported								*
 *==================================================================================*/

boolean pic_load( const char *file, IMAGE *img)
{
	IMGINFO info;
	DECDATA data;

	info = ( img_info *) malloc( sizeof( img_info));
	
	if ( !info)
	{
		errshow(NULL, -ENOMEM);
		return FALSE;
	}

	data = ( dec_data *) malloc( sizeof( dec_data));

	if ( !data)
	{
		errshow(NULL, -ENOMEM);
		free( info);
		return FALSE;
	}

	if( img->progress_bar)
		win_progress_begin( "Please Wait...");

	/* We initialise some variables needed by the codecs */
	info->background_color	= 0xFFFFFF;
	info->thumbnail			= img->view_mode;

	chrono_on(); 
	
	if(( decoder_init_done = get_pic_info( file, info)) == FALSE)
	{
		free( data);
		free( info);
		win_progress_end();
		errshow(NULL, IMG_NO_VALID);
		return FALSE;
	}

	if( !setup ( img, info, data))
	{
		errshow(NULL, -ENOMEM);
		quit_img( info, data);
		delete_mfdb( img->image, img->page);
		win_progress_end();
		return FALSE;
	}

	read_img ( img, info, data);

	quit_img( info, data);
	
	win_progress_end();

	if( img->view_mode && ( img->image[0].fd_w > thumbnail[thumbnail_size][0] || img->image[0].fd_h > thumbnail[thumbnail_size][1]) && smooth_thumbnail && img->image[0].fd_nplanes >= 16)
	{
		MFDB 	resized_image;
		double	precise_x, precise_y, factor;
		uint16	display_w, display_h;

		factor	  = MAX((( double)img->image[0].fd_h / ( double)thumbnail[thumbnail_size][1]), (( double)img->image[0].fd_w / ( double)thumbnail[thumbnail_size][0]));
		precise_x = ( double)img->image[0].fd_w  / factor;
		precise_y = ( double)img->image[0].fd_h / factor;
		display_w = ( uint16)( precise_x > 0 ? precise_x : 16);
		display_h = ( uint16)( precise_y > 0 ? precise_y : 16);
		
		init_mfdb( &resized_image, display_w, display_h, img->image[0].fd_nplanes);

		smooth_resize( &img->image[0], &resized_image, smooth_thumbnail);

		free( img->image[0].fd_addr);

		img->image[0].fd_addr 		= resized_image.fd_addr;
		img->image[0].fd_w 			= resized_image.fd_w;
		img->image[0].fd_h 			= resized_image.fd_h;
		img->image[0].fd_wdwidth	= resized_image.fd_wdwidth;
	}

	chrono_off( img->working_time);
		
	return TRUE;
}
