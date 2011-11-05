#include "general.h"
#include "pic_load.h"
#include "prefs.h"
#include "progress.h"
#include "plugins.h"
#include "mfdb.h"
#include "winimg.h"
#include "chrono.h"
#include "txt_data.h"
#include "zvdi/color.h"
#include "zvdi/raster.h"
#include "zvdi/vdi.h"
#include "pdf/pdflib.h"


int16 pdf_initialised = FALSE;


/*==================================================================================*
 * read_pdf:																		*
 *				Read a image and fit the MFDB within IMAGE structur with the data.	*
 *----------------------------------------------------------------------------------*
 * img: 		The struct. with the MFDB to be fitted.								*
 * page:		The wanted page.													*
 *----------------------------------------------------------------------------------*
 * returns: 	0 if error.															*
 *==================================================================================*/
int16 read_pdf( IMAGE *img, int16 page, double scale)
{
	DECDATA 		data;
	uint16 			*dst;
	uint8			*src, *src_prt;
	uint32			src_line_size;
	uint16  		x, y;

	data = ( dec_data *) gmalloc( sizeof( dec_data));

	if ( !data)
	{
		errshow( "", ENOMEM);
		return ( 0);
	}

	data->IncXfx    = 0x10000uL;
	data->IncYfx	= 0x10000uL;

	chrono_on(); 
	
	pdf_decode_page( img, page, scale);

	if( app.nplanes > 8)
		data->DthBuf = NULL;
	else
	{
		size_t size = ( img->img_w + 15) * 3;
		
		data->DthBuf = gmalloc( size);
		
		if( data->DthBuf == NULL) 
		{
			gfree( data);
			errshow( "", ENOMEM);
			return ( 0);
		}
			
		memset( data->DthBuf, 0, size);
	}

	data->DthWidth 	= img->img_w;
	data->PixMask  	= img->colors;

	if ( !init_mfdb( &img->image[page - 1], img->img_w, img->img_h, app.nplanes))
	{
		if( data->DthBuf) 
	   		gfree( data->DthBuf);
	   	
		gfree( data);
		errshow( "", ENOMEM);
		return ( 0);
	}	

	data->LnSize = img->image[page - 1].fd_wdwidth * app.nplanes;
	dst   		 = ( uint16*)img->image[page - 1].fd_addr;

	src_line_size = img->img_w * 3;	

	src = ( uint8*)pdf_get_page_addr( img);
	src_prt = src;
	
	/* now, we draw a line around the pdf */
	for( y = 0 ; y < img->img_h ; y++, src_prt += src_line_size)
	{
		if( y == 0 || y == img->img_h - 1)
		{
			for( x = 0 ; x < src_line_size ; x += 3)
			{
				src_prt[x]     = 0x00;
				src_prt[x + 1] = 0x00;
				src_prt[x + 2] = 0x00;
			}
		}

		src_prt[0] = 0x00;
		src_prt[1] = 0x00;
		src_prt[2] = 0x00;

		src_prt[src_line_size - 3] = 0x00;
		src_prt[src_line_size - 2] = 0x00;
		src_prt[src_line_size - 1] = 0x00;		
	}
		
	for( y = 0 ; y < img->img_h ; y++, src += src_line_size)
	{
		data->RowBuf = src;
		( *raster_true)( data, dst);
		dst   += data->LnSize;
	}
			
	if( data->DthBuf) 
   		gfree( data->DthBuf);
	   	
	gfree( data);	

	chrono_off( ( char*)img->working_time);

	return( 1);	
}


double get_scale_value( IMAGE *img, int16 page, uint16 target_width, uint16 target_height)
{
	int fit_pdf_to_win = 3; /* 0 = 100%, 1 = fit width, 2 = fit height, 3 = best fit */
	double w_scale, h_scale, scale;
	uint16 doc_width, doc_height;
	
	pdf_get_page_size( img, page, &doc_width, &doc_height);
	
	switch( fit_pdf_to_win)
	{
		case 1:
			scale = (double)target_width / doc_width;
			break;	
				
		case 2:
			scale = (double)target_height / doc_height;
			break;		
		
		case 3:
			w_scale = (double)target_width / doc_width;
			h_scale = (double)target_height / doc_height;

			scale = MIN (w_scale, h_scale);
			break;
			
		default:	
			scale = 1;
	}

	return scale;
}	


/*==================================================================================*
 * pdf_load:																		*
 *				initialise and read the first page of a PDF and fill the IMAGE		* 
 *				struct.																*
 *----------------------------------------------------------------------------------*
 * file: 		The file that will be read.											*
 * img:	 		The IMAGE struct. to be filled. 					 				*
 *----------------------------------------------------------------------------------*
 * returns: 	'0' if error.														*
 *==================================================================================*/

int16 pdf_load( const char *file, IMAGE *img, uint16 width, uint16 height)
{	
	int16 i;
	uint16 w = width;
	double scale = 1.0;
	
	if( pdf_initialised == FALSE)
		pdf_initialised = pdf_init( zview_path);

	if( lib_pdf_load( file, img) == FALSE)
		return( 0);

	if( ( img->image = ( MFDB *)gmalloc( sizeof( MFDB) *  img->page)) == NULL)
	{
		pdf_quit( img);
		return ( 0);
	}

	for( i = 0 ; i < img->page ; i++)	
	{
		MFDB 	*mfdb = &img->image[i];
		
		mfdb->fd_addr = NULL;
		img->delay[i] = 0;
	}

	
	/* if the pdf owns only one page, we don't need the bookmark's frame */
	if( img->page == 1)
		w += 205;
	
	if( pdf_fit_to_win)
		scale = get_scale_value( img, 1, w, height);
		
	if( !read_pdf( img, 1, scale))
	{
		pdf_quit( img);
		return ( 0);
	}

	if( !init_txt_data( img, 8, 256))
	{
		pdf_quit( img);
		delete_mfdb( img->image, img->page);
		return ( 0);
	}
		
	pdf_get_info( img, img->comments);

	return ( 1);
}
