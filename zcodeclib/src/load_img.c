#include "general.h"
#include "codecs.h"
#include "load_img.h"
#include "img.h"
#include "ztext.h"
#include "zvdi/color.h"
#include "zvdi/raster.h"
#include "zvdi/vdi.h"

boolean decoder_init_done = FALSE;

void 		  ( *raster)	  		( DECDATA, void *dst);
void 		  ( *raster_cmap) 		( DECDATA, void *);
void 		  ( *raster_true) 		( DECDATA, void *);
void 		  ( *cnvpal_color)		( IMGINFO, DECDATA);
void 		  ( *raster_gray) 		( DECDATA, void *);
void 	CDECL ( *decoder_quit)		( IMGINFO);
boolean	CDECL ( *decoder_init)		( const char *, IMGINFO);
boolean	CDECL ( *decoder_read)		( IMGINFO, uint8 *);


static IMAGE *setup ( IMGINFO info, DECDATA data, int16 w, int16 h, int16 keep_ratio)
{
	int16   n_planes = ( ( info->planes == 1 && info->components == 1) ?  1: nplanes);
	uint16	display_w, display_h;
	int16	wanted_w = ( w > 0 ? w : info->width), wanted_h = ( h > 0 ? h : info->height);
	float	precise_x, precise_y;
	size_t	src_line_size;
	IMAGE	*image;
	
	/* the image size is different that the original and we want to keep the aspect ratio */
	if( keep_ratio && ( info->width != wanted_w) && ( info->height != wanted_h))
	{
		float factor	= MAX( ( float)info->height / ( float)wanted_h, ( float)info->width / ( float)wanted_w);
		precise_x		= ( float)info->width  / factor;
		precise_y		= ( float)info->height / factor;

		display_w 		= ( ( uint16)precise_x > 0 ? ( uint16)precise_x : 16);
		display_h 		= ( ( uint16)precise_y > 0 ? ( uint16)precise_y : 16);
		data->IncXfx    = ((( uint32)info->width  << 16) + ( display_w >> 1)) / display_w;
		data->IncYfx    = ((( uint32)info->height << 16) + ( display_h >> 1)) / display_h;
	}
	else if(( info->width != wanted_w) && ( info->height != wanted_h))
	{
		float x_factor	= ( float)info->width / ( float)wanted_w;
		float y_factor	= ( float)info->height / ( float)wanted_h;
		precise_x		= ( float)info->width  / x_factor;
		precise_y		= ( float)info->height / y_factor;

		display_w 		= ( ( uint16)precise_x > 0 ? ( uint16)precise_x : 16);
		display_h 		= ( ( uint16)precise_y > 0 ? ( uint16)precise_y : 16);
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
	
	image = init_img( info->page, display_w, display_h, n_planes);
	
	if( image == NULL)
		return NULL;

/*	we assume that the pixel size is minimum 8 bits because some GNU libraries return 1 and 4 bits format like 8 bits ones.
	We add also a little more memory for avoid buffer overflow for plugin badly coded.  */
	src_line_size = ( info->width + 64) * info->components;

	data->RowBuf = ( uint8*)malloc( src_line_size);	

	if( !data->RowBuf)
		return NULL;

	if(( info->planes == 1 && info->components == 1) || nplanes > 8)
		data->DthBuf = NULL;
	else
	{
		size_t size = ( display_w + 15) * 3;
		
		data->DthBuf = malloc( size);

		if( data->DthBuf == NULL) 
			return NULL;
			
		memset( data->DthBuf, 0, size);
	}

	data->DthWidth 	= display_w;
	data->PixMask  	= ( 1 << info->planes) - 1;
	data->LnSize   	= image->image[0].fd_wdwidth * n_planes;

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

	image->delay = NULL;

	return image;
}

static inline void read_img ( IMAGE *img, IMGINFO info, DECDATA data)
{
	uint16 		*dst;
	int16  		y_dst, i, y, img_h 	= info->height;
	int32		line_size = data->LnSize;
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
			if( !decoder_read( info, buf))
				return;		

			while(( scale >> 16) < y) 
			{
				( *raster)( data, dst);
				dst   += line_size;
				scale += data->IncYfx;
				if (!--y_dst) break;
			}
		}
		
		if( img->delay != NULL )
		{
			img->delay[i] = info->delay;
		}
	}
}



void quit_img( IMGINFO info, DECDATA data)
{
	if( decoder_init_done == TRUE)
		decoder_quit( info);

	if( data->DthBuf) 
	   	free( data->DthBuf);

	if( data->RowBuf) 
	   free( data->RowBuf);

	free( data);
	free( info); 

	decoder_init_done = FALSE;
}


boolean get_pic_info( const char *file, IMGINFO info)
{
	int16 	i, j, c = 0;
	char 	extention[3+1], plugin[3];
	
	strcpy ( extention, file + strlen( file) - 3);
	str2upper( extention);

	/* We check if a plug-ins can do the job */
	for( i = 0; i < plugins_nbr; i++, c = 0)
	{
		for( j = 0; j < codecs[i]->user_ext; j++)
		{
			plugin[0] = codecs[i]->infos[c++];
			plugin[1] = codecs[i]->infos[c++];
			plugin[2] = codecs[i]->infos[c++];

			if( strncmp ( extention, plugin, 3) == 0)
			{
				if ( !( decoder_init 	= ldg_find( "reader_init", codecs[i]))
				  || !( decoder_read 	= ldg_find( "reader_read", codecs[i])) 
				  || !( decoder_quit 	= ldg_find( "reader_quit", codecs[i])))
				{
					return FALSE;
				}				
					
				return decoder_init( file, info);
			}
		}
	}
	
	/* I wish that it will never append ! */
	return FALSE;
}

IMAGE *load_img( const char *file, int16 w, int16 h, int16 keep_ratio)
{
	IMGINFO info;
	DECDATA data;
	IMAGE	*img;

	/* One codec or more must be present */
	if( codecs[0] == NULL)
		return( NULL);	

	info = ( img_info *)malloc( sizeof( img_info));
	
	if( !info)
		return( NULL);

	data = ( dec_data *)malloc( sizeof( dec_data));

	if( !data)
	{
		free( info);
		return ( NULL);
	}

	info->background_color	= 0xFFFFFF;
	info->thumbnail			= FALSE;		
			
	if (( decoder_init_done = get_pic_info( file, info)) == FALSE)
	{
		free( data);
		free( info);
		return ( NULL);
	}

	img = setup ( info, data, w, h, keep_ratio);
	if( img == NULL)
	{
		delete_img( img);
		quit_img( info, data);
		return ( NULL);
	}

	read_img ( img, info, data);

	quit_img( info, data);

	return ( img);
}
