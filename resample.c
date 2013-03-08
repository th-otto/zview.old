#include "general.h"
#include "mfdb.h"
#include "pic_load.h"
#include "zvdi/pixel.h"
#include <math.h>

#define MagickEpsilon  1.0e-6

typedef struct
{
	double	weight;
	long	pixel;
}ContributionInfo;

typedef struct
{
  double (*function)( double);
  double support;
} FilterInfo;


static double ( *algo)( double x) = NULL;
double algo_support = 3.0f;


static inline uint8 RoundToQuantum( double value)
{
	if( value < 0.0)
		return( 0);
    
	if ( value >= 255.0)
    	return(( uint8) 255);
    	
  	return(( uint8)( value + 0.5));
}


static double Sinc( double x)
{
	if( x == 0.0)
    	return( 1.0);
    
	return( sin( x) / x);
}


static double Lanczos( double x)
{
	x = fabs(x);

	return( x < 3.0) ? Sinc( M_PI * x) * Sinc( 1.0471975511965977461533333333333 * x) : 0.0;	
}



static double Gaussian( double x)
{
	return( exp((double)( -2.0 * x * x)) * sqrt( 2.0 / M_PI));
}


static double Cubic( double x)
{
	if( x < -2.0)
	    return( 0.0);
	if( x < -1.0)
	    return( ( 2.0 + x) * ( 2.0 + x) * ( 2.0 + x) / 6.0);
	if( x < 0.0)
    	return( ( 4.0 + x * x * ( -6.0 - 3.0 * x)) / 6.0);
	if( x < 1.0)
	    return( ( 4.0 + x * x * ( -6.0 + 3.0 * x)) / 6.0);
	if( x < 2.0)
	    return( ( 2.0 - x) * ( 2.0 - x) * ( 2.0 - x) / 6.0);

	return(0.0);
}


static double Blackman( double x)
{
	return( 0.42 + 0.5 * cos( M_PI * ( double)x) + 0.08 * cos( 2 * M_PI * (double) x));
}

static double Triangle( double x)
{
	if( x < -1.0)
    	return( 0.0);
  	if( x < 0.0)
    	return( 1.0 + x);
  	if( x < 1.0)
    	return( 1.0 - x);

   	return( 0.0);
}


static double Quadratic( double x)
{
	if( x < -1.5)
    	return( 0.0);
  	if( x < -0.5)
    	return( 0.5 * ( x + 1.5) * ( x + 1.5));
  	if( x < 0.5)
    	return( 0.75 - x * x);
  	if( x < 1.5)
    	return( 0.5 * ( x - 1.5) * ( x - 1.5));
  	
    return( 0.0);
}


static void HorizontalFilter( MFDB *source, MFDB *destination, double x_factor, ContributionInfo *contribution)
{
	long start, stop;
 	double center, density, scale, support;
 	register long i, x, y, n;
	uint8	source_red, source_green, source_blue; 	
	uint32	source_line_size, dst_line_size;

	source_line_size	= (( uint32)source->fd_wdwidth << 1 ) * ( uint32)source->fd_nplanes;
	dst_line_size 		= (( uint32)destination->fd_wdwidth << 1 ) * ( uint32)destination->fd_nplanes;
	scale 				= MAX( 1.0 / x_factor, 1.0);
	support 			= scale * algo_support;

	if( support <= 0.5)
    {
		/* Reduce to point sampling. */
		support = ( double)( 0.5 + MagickEpsilon);
		scale = 1.0;
    }

    scale = 1.0 / scale;

	for( x = 0; x < destination->fd_w; x++)
	{
    	center	= ( double)( x + 0.5) / x_factor;
    	start	= ( long)MAX( center - support + 0.5, 0);
    	stop	= ( long)MIN( center + support + 0.5, ( double)source->fd_w);
    	density = 0.0;
    	
    	for( n = 0; n < (stop-start); n++)
    	{
    		contribution[n].pixel = start + n;
    		contribution[n].weight = algo( scale * (( double)( start + n) - center + 0.5));
    		density += contribution[n].weight;
    	}
    	
    	if(( density != 0.0) && ( density != 1.0))
      	{
        	density = 1.0 / density;
        	
        	for( i = 0; i < n; i++)
          		contribution[i].weight *= density;
      	}
					 /* source->fd_h  */
    	for( y = 0; y < destination->fd_h; y++)
    	{
    		register double gamma 	= 0.0;
    		register double red 	= 0.0;
    		register double green 	= 0.0;
    		register double blue 	= 0.0;

			for( i = 0; i < n; i++)
      		{
        		getPixel( contribution[i].pixel, y, source_line_size, &source_red, &source_green, &source_blue, source->fd_addr);

         		red		+= contribution[i].weight * source_red;
        		green	+= contribution[i].weight * source_green;
				blue	+= contribution[i].weight * source_blue;
        		gamma	+= contribution[i].weight;
      		}
      
      		gamma = 1.0 / ( fabs( gamma) <= MagickEpsilon ? 1.0 : gamma);

      		setPixel( x, y, dst_line_size, RoundToQuantum( gamma * red), RoundToQuantum( gamma * green), RoundToQuantum( gamma * blue), destination->fd_addr);
    	}
    }
}



static void VerticalFilter( MFDB *source, MFDB *destination, double y_factor, ContributionInfo *contribution)
{
	register long i, y, x, n;
	long 	start, stop;
	double	center, density, scale, support;
	uint8	source_red, source_green, source_blue;
	uint32	source_line_size, dst_line_size;

	source_line_size	= (( uint32)source->fd_wdwidth << 1 ) * ( uint32)source->fd_nplanes;
	dst_line_size 		= (( uint32)destination->fd_wdwidth << 1 ) * ( uint32)destination->fd_nplanes;
	scale 				= MAX( 1.0 / y_factor, 1.0);
	support 			= scale * algo_support;

	if( support <= 0.5)
    {
      /* Reduce to point sampling. */
      support = ( double)( 0.5 + MagickEpsilon);
      scale = 1.0;
    }
    
	scale = 1.0 / scale;

	for( y = 0; y < destination->fd_h; y++)
	{
		center	= ( double)( y + 0.5) / y_factor;
    	start 	= ( long)MAX( center - support + 0.5, 0);
    	stop 	= ( long)MIN( center + support + 0.5, ( double) source->fd_h);
    	density = 0.0;
    
    	for( n = 0; n < ( stop - start); n++)
    	{
    		contribution[n].pixel = start + n;
    		contribution[n].weight = algo( scale * (( double)( start + n) - center + 0.5));
    		density += contribution[n].weight;
    	}

    	if(( density != 0.0) && ( density != 1.0))
    	{
    		/* Normalize. */
			density = 1.0 / density;
        
			for( i = 0; i < n; i++)
        		contribution[i].weight *= density;
		}
        
    	for ( x = 0; x < destination->fd_w; x++)
       	{
    		register double gamma 	= 0.0;
    		register double red 	= 0.0;
    		register double green 	= 0.0;
    		register double blue 	= 0.0;
    
    		for ( i = 0; i < n; i++)
       		{
        		getPixel( x, contribution[i].pixel, source_line_size, &source_red, &source_green, &source_blue, source->fd_addr);
        		
         		red		+= contribution[i].weight * source_red;
        		green	+= contribution[i].weight * source_green;
				blue	+= contribution[i].weight * source_blue;
     			gamma	+= contribution[i].weight;
      		}
      
      		gamma = 1.0 / ( fabs( gamma) <= MagickEpsilon ? 1.0 : gamma);

      		setPixel( x, y, dst_line_size, RoundToQuantum( gamma * red), RoundToQuantum( gamma * green), RoundToQuantum( gamma * blue), destination->fd_addr);
    	}
  	}
}

int16 smooth_resize( MFDB *source_image, MFDB *resized_image, int resize_algo)
{
	MFDB tmp_img;
	double support, x_factor, x_support, y_factor, y_support;
	ContributionInfo *contribution;
	
	static const FilterInfo filters[7] =
	{
    	{ NULL, 		0},
    	{ Triangle, 	1.0f},
    	{ Blackman, 	1.0f},
    	{ Gaussian, 	1.25f},
    	{ Quadratic,	1.5f},
    	{ Cubic, 		2.0f},
    	{ Lanczos, 		3.0f}
	};
	
    algo		 = filters[resize_algo].function;
    algo_support = filters[resize_algo].support;
    
	x_factor	= (double) resized_image->fd_w / (double) source_image->fd_w;
	y_factor	= (double) resized_image->fd_h / (double) source_image->fd_h;
	x_support	= MAX( 1.0 / x_factor, 1.0) * algo_support;
	y_support	= MAX( 1.0 / y_factor, 1.0) * algo_support;
	support 	= MAX( x_support, y_support);

	if( support < algo_support)
    	support = algo_support;
 	
	contribution = ( ContributionInfo *)gmalloc( ( size_t)( 2.0 * MAX( support, 0.5) + 3) * sizeof( *contribution));

	if( contribution == NULL)
		return 0;

    /* Resize image. */
	if (( resized_image->fd_w * (( uint32) source_image->fd_h + resized_image->fd_h)) > ( resized_image->fd_h * ((uint32) source_image->fd_w + resized_image->fd_w)))
    {
		if( !init_mfdb( &tmp_img, resized_image->fd_w, source_image->fd_h, app.nplanes))
        {
        	gfree(contribution);
        	return 0;
        }
          
      	HorizontalFilter( source_image , &tmp_img, x_factor, contribution);
     	VerticalFilter( &tmp_img, resized_image, y_factor, contribution);
    }
	else
    {
		if( !init_mfdb( &tmp_img, source_image->fd_w, resized_image->fd_h, app.nplanes))
        {
        	gfree(contribution);
        	return 0;
        }

      	VerticalFilter( source_image, &tmp_img, y_factor, contribution);
      	HorizontalFilter( &tmp_img, resized_image, x_factor, contribution);
    }

	gfree( contribution);
	gfree( tmp_img.fd_addr);

    return 1;
}
