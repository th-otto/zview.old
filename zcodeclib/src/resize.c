#include "general.h"
#include "load_img.h"
#include "mfdb.h"
#include "zvdi/raresize.h"
#include "zvdi/vdi.h"
#include "zvdi/p2c.h"


void  ( *resize)( DECDATA, void *dst);
void  ( *my_resize)( DECDATA, void *dst);

extern int32 plane2packed( int32 no_words, int32 plane_length, int16 no_planes, void *src, void *dst); 


static int16 resize_setup( MFDB *in,  MFDB *out, DECDATA data, int16 w, int16 h, int16 keep_ratio)
{
	uint16	display_w, display_h;
	float	precise_x, precise_y;

	/* the image size is different that the original and we want to keep the aspect ratio */
	if( keep_ratio)
	{
		float factor	= MAX( ( float)in->fd_h / ( float)h, ( float)in->fd_w / ( float)w);
		precise_x		= ( float)in->fd_w  / factor;
		precise_y		= ( float)in->fd_h / factor;

		display_w 		= ( ( uint16)precise_x > 0 ? ( uint16)precise_x : 16);
		display_h 		= ( ( uint16)precise_y > 0 ? ( uint16)precise_y : 16);
		data->IncXfx    = ((( uint32)in->fd_w  << 16) + ( display_w >> 1)) / display_w;
		data->IncYfx    = ((( uint32)in->fd_h << 16) + ( display_h >> 1)) / display_h;
	}
	else
	{
		float x_factor	= ( float)in->fd_w / ( float)w;
		float y_factor	= ( float)in->fd_h / ( float)h;
		precise_x		= ( float)in->fd_w / x_factor;
		precise_y		= ( float)in->fd_h / y_factor;

		display_w 		= ( ( uint16)precise_x > 0 ? ( uint16)precise_x : 16);
		display_h 		= ( ( uint16)precise_y > 0 ? ( uint16)precise_y : 16);
		data->IncXfx    = ((( uint32)in->fd_w  << 16) + ( display_w >> 1)) / display_w;
		data->IncYfx    = ((( uint32)in->fd_h << 16) + ( display_h >> 1)) / display_h;
	}

	if ( !init_mfdb( out, display_w, display_h, in->fd_nplanes))
		return ( 0);

	data->RowBuf 	= in->fd_addr;

	if( planar)	
	{
		data->DstBuf = shared_malloc((( uint32)in->fd_wdwidth << 4 ) + 256uL);

		if( data->DstBuf == NULL)
		{
			delete_mfdb( out, 1);
			return ( 0);
		}
	}
	else
		data->DstBuf = NULL;

	data->DthWidth 	= display_w;
	data->LnSize   	= out->fd_wdwidth * out->fd_nplanes;

	if( out->fd_nplanes == 1) 
		my_resize = resize_mono;
	else
		my_resize = resize;

	return( 1);
}


static inline void resize_img( MFDB *in,  MFDB *out, DECDATA data)
{
	uint16 		*dst = out->fd_addr;
	int16  		y = 0;
	int32		in_line_size  = (( uint32)in->fd_wdwidth << 1 ) * ( uint32)in->fd_nplanes, 
				out_line_size = data->LnSize;
	uint32		scale = ( data->IncYfx + 1) >> 1;
	int16		y_dst  = out->fd_h;


	while ( y < in->fd_h)
	{
		y++;

		if( planar)
		{
			if( ( scale >> 16) < y)
				planar_to_chunky( data->RowBuf, data->DstBuf, in->fd_w);
		}
	
		while (( scale >> 16) < y) 
		{
			my_resize( data, dst);
			dst   += out_line_size;
			scale += data->IncYfx;
			if ( !--y_dst) break;
		}

		data->RowBuf += in_line_size;
	}
}


/*==================================================================================*
 * pic_resize:																		*
 *				Create a resized MFDB. 												*
 *----------------------------------------------------------------------------------*
 * in: 		 	The src MFDB.														*
 * img: 		The target MFDB.						 			 				*
 *----------------------------------------------------------------------------------*
 * returns: 	'0' if error else 1.												*
 *==================================================================================*/

MFDB *pic_resize( MFDB *in, int16 w, int16 h, int16 keep_ratio)
{
	DECDATA data;
	MFDB	*out;

	/* 2 planes zoom not available */
	if( in->fd_nplanes == 2)
		return NULL;

	out = ( MFDB *)shared_malloc( sizeof( MFDB));

	if( out == NULL)
		return NULL;

	data = ( dec_data *)shared_malloc( sizeof( dec_data));

	if( !data)
	{
		shared_free( out);
		return NULL;
	}


	if( !resize_setup( in, out, data, w, h, keep_ratio))
	{
		shared_free( data);
		shared_free( out);
		return NULL;
	}

	resize_img ( in, out, data);

	if( data->DstBuf)
		shared_free( data->DstBuf);

	shared_free( data);

	return( out);
}
