#include <tiffio.h>
#define HAVE_INTS_DEFINED
#include "plugin.h"
#include "zvplugin.h"
#include "zvtiff.h"

#define VERSION 0x200
#define NAME    "Tagged Image File Format"
#define AUTHOR  "Zorro"
#define DATE     __DATE__ " " __TIME__
#define MISCINFO "Using " TIFFLIB_VERSION_STR

#define alpha_composite( composite, fg, alpha) {						\
    uint16_t temp = (( uint16_t)( fg) * ( uint16_t)( alpha) + ( uint16_t)128);	\
    ( composite) = ( uint8_t)(( temp + ( temp >> 8)) >> 8);				\
}

uint16_t 	encode_compression 	= COMPRESSION_LZW;
int		quality 			= 75;	


#ifdef PLUGIN_SLB
long __CDECL get_option(zv_int_t which)
{
	switch (which)
	{
	case OPTION_CAPABILITIES:
		return CAN_DECODE | CAN_ENCODE;
	case OPTION_EXTENSIONS:
		return (long)("TIF\0" "TIFF\0");

	case OPTION_QUALITY:
		return quality;
	case OPTION_COMPRESSION:
		return encode_compression;

	case INFO_NAME:
		return (long)NAME;
	case INFO_VERSION:
		return VERSION;
	case INFO_DATETIME:
		return (long)DATE;
	case INFO_AUTHOR:
		return (long)AUTHOR;
	case INFO_MISC:
		return (long)MISCINFO;
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
	case OPTION_COMPRESSION:
		if (TIFFFindCODEC(value) == NULL)
			return -ERANGE;
		encode_compression = value;
		return value;
	}
	return -ENOSYS;
}


static long init_tiff_slb(void)
{
	struct _zview_plugin_funcs *funcs;
	SLB *slb;
	long ret;

	funcs = get_slb_funcs();
	slb = get_slb_funcs()->p_slb_get(LIB_TIFF);
	if (slb->handle == 0)
	{
		if ((ret = funcs->p_slb_open(LIB_TIFF)) < 0)
			return ret;
	}
	return 0;
}
#endif


static void TIFF_CALLBACK my_errorhandler(const char *module, const char *format, va_list ap)
{
	(void)module;
	(void)format;
	(void)ap;
}


static void TIFF_CALLBACK my_errorhandler_ext(thandle_t clientdata, const char *module, const char *format, va_list ap)
{
	(void)clientdata;
	(void)module;
	(void)format;
	(void)ap;
}


static void TIFF_CALLBACK my_warninghandler(const char *module, const char *format, va_list ap)
{
	(void)module;
	(void)format;
	(void)ap;
}


static void TIFF_CALLBACK my_warninghandler_ext(thandle_t clientdata, const char *module, const char *format, va_list ap)
{
	(void)clientdata;
	(void)module;
	(void)format;
	(void)ap;
#if 0
	printf("zvtiff: ");
	if (module != NULL)
		printf("%s: ", module);
	printf("Warning: ");
	vfprintf(stderr, format, ap);
	printf("\n");
#endif
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
	TIFF *tif;
	uint32_t w, h, *raster;
	uint16_t compression, bitpersample, samplesperpixel, planes;

#ifdef PLUGIN_SLB
	if (init_tiff_slb() < 0)
		return FALSE;
#endif

	TIFFSetErrorHandler(my_errorhandler);
	TIFFSetErrorHandlerExt(my_errorhandler_ext);
	TIFFSetWarningHandler(my_warninghandler);
	TIFFSetWarningHandlerExt(my_warninghandler_ext);

	if(( tif = TIFFOpen( name, "r")) == NULL)
	{
		return FALSE;
    }

/*
	do
	{
		info->page++;	
    }while( TIFFReadDirectory( tif));
*/	

    TIFFGetField( tif, TIFFTAG_IMAGEWIDTH, &w);
    TIFFGetField( tif, TIFFTAG_IMAGELENGTH, &h);
    TIFFGetField( tif, TIFFTAG_COMPRESSION, &compression);
    TIFFGetField( tif, TIFFTAG_BITSPERSAMPLE, &bitpersample);
    TIFFGetField( tif, TIFFTAG_SAMPLESPERPIXEL, &samplesperpixel);

	planes = bitpersample * samplesperpixel;

	info->width 				= ( uint16_t)w;
	info->height 				= ( uint16_t)h;
	info->components			= 3;
	info->planes 				= planes;
	info->indexed_color			= FALSE;
	info->page = 1;
	
	planes = MIN( 24, planes);

	info->colors  				= ( uint32_t)( 1uL << planes);
	info->real_width			= info->width;
	info->real_height			= info->height;
	info->memory_alloc 			= TT_RAM;
	info->delay					= 0;
	info->orientation			= UP_TO_DOWN;
	info->num_comments			= 0;
	info->max_comments_length 	= 0;
	

	strcpy( info->info, "Tag Image File Format - TIFF");	

	switch( compression)
	{
		case COMPRESSION_NONE:
			strcpy( info->compression, "None");	
			break;
		case COMPRESSION_CCITTRLE: /* CCITT modified Huffman RLE */
			strcpy( info->compression, "HRLE");	
			break;
		case COMPRESSION_CCITTFAX3: /* CCITT Group 3 fax encoding */
			strcpy( info->compression, "FAX3");	
			break;
		case COMPRESSION_CCITTFAX4: /* CCITT Group 4 fax encoding */
			strcpy( info->compression, "FAX4");	
			break;
		case COMPRESSION_LZW: /* Lempel-Ziv  & Welch */
			strcpy( info->compression, "LZW");	
			break;
		case COMPRESSION_OJPEG: /* Old JPG, no supported ! */
			TIFFClose( tif);
			return FALSE;	
		case COMPRESSION_JPEG: /* JPEG DCT compression */
			strcpy( info->compression, "JPEG");	
			break;
		case COMPRESSION_T85: /* TIFF/FX T.85 JBIG compression */
		case COMPRESSION_T43: /* !TIFF/FX T.43 colour by layered JBIG compression */
			TIFFClose( tif);
			return FALSE;	
		case COMPRESSION_NEXT: /* NeXT 2-bit RLE */
			strcpy( info->compression, "NeXT");	
			break;
		case COMPRESSION_CCITTRLEW: /* #1 w/ word alignment RLE*/
			strcpy( info->compression, "RLEw");	
			break;
		case COMPRESSION_PACKBITS: /* Macintosh RLE */
			strcpy( info->compression, "MAC");	
			break;
		case COMPRESSION_THUNDERSCAN: /* ThunderScan RLE */
			strcpy( info->compression, "Thun");	
			break;
		case COMPRESSION_IT8CTPAD: /* IT8 CT w/padding */
			strcpy( info->compression, "IT8");	
			break;
		case COMPRESSION_IT8LW: /* IT8 Linework RLE */
			strcpy( info->compression, "IT8R");	
			break;
		case COMPRESSION_IT8MP: /* IT8 Monochrome picture */
			strcpy( info->compression, "IT8M");	
			break;
		case COMPRESSION_IT8BL: /* IT8 Binary line art */
			strcpy( info->compression, "IT8B");	
			break;
		case COMPRESSION_PIXARFILM: /* Pixar companded 10/11bit LZW */
		case COMPRESSION_PIXARLOG:
			strcpy( info->compression, "Pixa");	
			break;
		case COMPRESSION_ADOBE_DEFLATE:
		case COMPRESSION_DEFLATE: /* Deflate compression */
			strcpy( info->compression, "Defl");	
			break;
		case COMPRESSION_DCS: /* Kodak DCS encoding */
			strcpy( info->compression, "DCS");	
			break;
		case COMPRESSION_JBIG: /* ISO JBIG */
			strcpy( info->compression, "JBIG");	
			break;
		case COMPRESSION_SGILOG: /* SGI Log Luminance RLE / 24bits*/
		case COMPRESSION_SGILOG24:
			strcpy( info->compression, "SGI");	
			break;
		case COMPRESSION_JP2000: /* Leadtools JPEG2000 */
			strcpy( info->compression, "JP20");
			break;
		case COMPRESSION_LERC:
			strcpy( info->compression, "Lerc");
			break;
		case COMPRESSION_LZMA:
			strcpy( info->compression, "LZMA");
			break;
		case COMPRESSION_ZSTD:
			strcpy( info->compression, "ZStd");
			break;
		case COMPRESSION_WEBP:
			strcpy( info->compression, "WebP");
			break;
		default:	
			strcpy( info->compression, "???");	
			break;
    }

	raster = ( uint32_t*)malloc( ( w * h) << 2);

	if( raster == NULL)
	{
		TIFFClose( tif);
		return FALSE;	
	}

	info->_priv_ptr				= ( void*)raster;
	info->_priv_ptr_more		= ( void*)raster;	
	info->__priv_ptr_more 		= ( void*)tif;

	if( TIFFReadRGBAImageOriented( tif, w, h, raster, ORIENTATION_TOPLEFT, 0))
		return TRUE;
	
	free( raster);
	TIFFClose( tif);
	return FALSE;	
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
    uint8_t 	r = 0, g = 0, b = 0, a = 0;
	uint16_t	i, j = 0;
	uint32_t	*img = ( uint32_t*)info->_priv_ptr_more;

	for( i = 0; i < info->width; i++)
	{
		a =  TIFFGetA(img[i]);
		r =  TIFFGetR(img[i]);
		g =  TIFFGetG(img[i]);
		b =  TIFFGetB(img[i]);

		alpha_composite( buffer[j++], r, a);
		alpha_composite( buffer[j++], g, a);
		alpha_composite( buffer[j++], b, a);
	}

	info->_priv_ptr_more = ( uint32_t*)info->_priv_ptr_more + info->width;

	return TRUE;
}


/*==================================================================================*
 * boolean __CDECL reader_get_txt													*
 *		This function , like other function must be always present.					*
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
	(void)info;
	(void)txtdata;
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
	TIFF 	*tif = ( TIFF*)info->__priv_ptr_more;

	free(info->_priv_ptr);
	TIFFClose( tif);
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
	TIFF *tif;

	if(( tif = TIFFOpen( name, "w")) == NULL)
	{
		return FALSE;
    }
	
	TIFFSetField( tif, TIFFTAG_IMAGEWIDTH,		info->width);
	TIFFSetField( tif, TIFFTAG_IMAGELENGTH,		info->height);
	TIFFSetField( tif, TIFFTAG_ORIENTATION,		ORIENTATION_TOPLEFT);
	TIFFSetField( tif, TIFFTAG_SAMPLESPERPIXEL, 3);
	TIFFSetField( tif, TIFFTAG_BITSPERSAMPLE,	8);
	TIFFSetField( tif, TIFFTAG_PLANARCONFIG,	PLANARCONFIG_CONTIG);
	TIFFSetField( tif, TIFFTAG_PHOTOMETRIC,		PHOTOMETRIC_RGB);
	TIFFSetField( tif, TIFFTAG_SAMPLEFORMAT,	SAMPLEFORMAT_UINT);
	TIFFSetField( tif, TIFFTAG_COMPRESSION,		encode_compression);

	if( encode_compression == COMPRESSION_JPEG)
	{
		TIFFSetField( tif, TIFFTAG_JPEGQUALITY,		quality);
		TIFFSetField( tif, TIFFTAG_JPEGCOLORMODE,	JPEGCOLORMODE_RGB);
    }

	TIFFSetField( tif, TIFFTAG_ROWSPERSTRIP, TIFFDefaultStripSize( tif, 0));

	info->planes   			= 24;
	info->colors  			= 16777215L;
	info->orientation		= UP_TO_DOWN;
	info->memory_alloc 		= TT_RAM;
	info->indexed_color	 	= FALSE;
	info->page			 	= 1;
	info->delay 		 	= 0;
	info->_priv_ptr	 		= ( void*)tif;
	info->_priv_ptr_more	= NULL;				
	info->__priv_ptr_more	= NULL;	
	info->_priv_var	 		= 0uL;
	info->_priv_var_more	= 0uL;


	return TRUE;
}


/*==================================================================================*
 * boolean __CDECLencoder_write:													*
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
	if ( TIFFWriteScanline( ( TIFF *)info->_priv_ptr, buffer, info->_priv_var++, 0) < 0)
		return FALSE;

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
	TIFF 	*tif = ( TIFF*)info->_priv_ptr;

	TIFFClose( tif);
}
