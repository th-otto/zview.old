#include "../general.h"
#include "../pic_load.h"
#include "../pic_resize.h"
#include "raster.h"
#include "raster_resize.h"
#include "color.h"
#include "p2c.h"
#include "pixel.h"
#include <mint/cookie.h>

int16 	pixel_val[256];
uint32 	cube216[216];
uint32  graymap[32];
int16	planar = 0;

extern void 	save_colors( void);

/* prototype */
int16 vdi_init ( void);

int16 vdi_init( void)
{
	int16 		out[273] = { -1, }; 
	int16 		depth;
	boolean	  	reverse, z_trail;
	long		dum;

	if( !get_cookie( C_EdDI, (long*)&dum))
		return( 0);
	
	vq_scrninfo ( app.handle, out);

 	if ( app.nplanes <= 8)
	{
		save_colors();
		memcpy ( pixel_val, out + 16, 512);
	}

	depth 	= (( uint16)out[4] == 0x8000u ? 15 : out[2]);	/* bits per pixel used */
	reverse = (out[16] < out[48]);							/* intel crap...       */
	z_trail = (out[48] > 0);								/* bits are shifted to the right side */

	if ( depth == 1) 
	{	/* monochrome */
		cnvpal_color = cnvpal_1_2;
		raster_cmap  = raster_D1;
		raster_true  = dither_D1;
		raster_gray  = gscale_D1;	
		resize 		 = resize_mono;
	}
	else if ( out[0] == 0) /* Planar */
	{
		planar = 1;

		switch ( depth)
		{
			case 2:
				cnvpal_color = cnvpal_1_2;
				raster_cmap  = raster_D2;
				raster_true  = dither_D2;
				raster_gray  = gscale_D2;
				planar_to_chunky = planar2_to_chunky8;
				break;
			case 4:
				cnvpal_color = cnvpal_4_8;
				raster_cmap  = raster_I4;
				raster_true  = dither_I4;
				raster_gray  = gscale_I4;
				resize 		 = resize_I4;
				planar_to_chunky = planar4_to_chunky8;
				break;
			case 8:
				cnvpal_color = cnvpal_4_8;
				raster_cmap  = raster_I8;
				raster_true  = dither_I8;
				raster_gray  = gscale_I8;
				resize 		 = resize_I8;
				planar_to_chunky = planar8_to_chunky8;
				break;
		}
	}
	else if ( out[0] == 2) /* chunky */
		switch ( depth)
		{
			case  8:
				cnvpal_color = cnvpal_4_8;
				raster_cmap  = raster_P8;
				raster_true  = dither_P8;
				raster_gray  = gscale_P8;
				resize 		 = resize_P8;	
				break;

			case 15: 
				if (!(out[14] & 0x02)) 
				{
					cnvpal_color = cnvpal_15;

					if (reverse) 
					{
						raster_cmap = raster_15r;
						raster_gray = gscale_15r;
						raster_true = dither_15r;
					} 
					else 
					{
						raster_cmap = raster_15;
						raster_gray = gscale_15;
						raster_true = dither_15;
					}

					resize = resize_16;	

					break;
				}

			case 16:
				cnvpal_color = cnvpal_high;

				if (reverse) 
				{
					raster_cmap = raster_16r;
					raster_gray = gscale_16r;
					raster_true = dither_16r;
					getPixel	= getPixel_16r;
					setPixel	= setPixel_16r;
				} 
				else 
				{
					raster_cmap = raster_16;
					raster_gray = gscale_16;
					raster_true = dither_16;
					getPixel	= getPixel_16;
					setPixel	= setPixel_16;					
				}

				resize = resize_16;	

				break;

			case 24:
				cnvpal_color = cnvpal_true;

				if (reverse) 
				{
					raster_cmap = raster_24r;
					raster_gray = gscale_24r;
					raster_true = dither_24r;
					getPixel	= getPixel_24r;
					setPixel	= setPixel_24r;					
				} 
				else 
				{
					raster_cmap = raster_24;
					raster_gray = gscale_24;
					raster_true = dither_24;
					getPixel	= getPixel_24;
					setPixel	= setPixel_24;					
				}

				resize = resize_24;	

				break;

			case 32:
				cnvpal_color = cnvpal_true;

				if (reverse) 
				{
					raster_cmap = raster_32r;
					raster_gray = gscale_32r;
					raster_true = dither_32r;
					getPixel	= getPixel_32r;
					setPixel	= setPixel_32r;					
				} 
				else if( z_trail) 
				{
					raster_cmap = raster_32z;
					raster_gray = gscale_32z;
					raster_true = dither_32z;
					getPixel	= getPixel_32z;
					setPixel	= setPixel_32z;					
				} 
				else 
				{
					raster_cmap = raster_32;
					raster_gray = gscale_32;
					raster_true = dither_32;
					getPixel	= getPixel_32;
					setPixel	= setPixel_32;					
				}

				resize = resize_32;				

				break;	
		}

	if (depth == 4 || depth == 8) 
	{
		uint32 * dst;
		uint32   r, g, b;
		dst = cube216;

		for (r = 0x000000uL; r <= 0xFF0000uL; r += 0x330000uL) 
		{
			for (g = 0x000000uL; g <= 0x00FF00uL; g += 0x003300uL) 
			{
				for (b = 0x000000uL; b <= 0x0000FFuL; b += 0x000033uL) 
				{
					*(dst++) = color_lookup ( r | g | b, pixel_val);
				}
			}
		}

		dst = graymap;

		for ( g = 0x000000uL; g <= 0xF8F8F8uL; g += 0x080808uL) 
			*( dst++) = color_lookup ( g | (( g >> 5) & 0x030303uL), pixel_val);
	}

	return( 1);
}

