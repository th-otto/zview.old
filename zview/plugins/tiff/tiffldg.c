#include <tiffio.h>
#define HAVE_INTS_DEFINED
#include "zview.h"

#define alpha_composite( composite, fg, alpha) {						\
    uint16 temp = (( uint16)( fg) * ( uint16)( alpha) + ( uint16)128);	\
    ( composite) = ( uint8)(( temp + ( temp >> 8)) >> 8);				\
}

void 	CDECL init( void);
boolean CDECL reader_init( const char *name, IMGINFO info);
void    CDECL reader_get_txt( IMGINFO info, txt_data *txtdata);
boolean CDECL reader_read( IMGINFO info, uint8 *buffer);
void 	CDECL reader_quit( IMGINFO info);
boolean CDECL encoder_init( const char *name, IMGINFO info);
boolean CDECL encoder_write( IMGINFO info, uint8 *buffer);
void 	CDECL encoder_quit( IMGINFO info);
void 	CDECL set_tiff_option( int16 set_quality, uint16 set_encode_compression);


PROC TIFFFunc[] = 
{
	{ "plugin_init",    "", init},
	{ "reader_init",    "", reader_init},
	{ "reader_get_txt", "", reader_get_txt},
	{ "reader_read",    "", reader_read},
	{ "reader_quit",    "", reader_quit},
	{ "encoder_init", 	"", encoder_init},
	{ "encoder_write",	"", encoder_write},
	{ "encoder_quit", 	"", encoder_quit},
	{ "set_tiff_option","", set_tiff_option}
};


LDGLIB tiff_plugin =
{
	0x0200, 			/* Plugin version */
	9,					/* Number of plugin's functions */
	TIFFFunc,			/* List of functions */
	"TIFIFF",			/* File's type Handled */
	LDG_NOT_SHARED, 	/* The flags NOT_SHARED is used here.. even if zview plugins are reentrant 
					   	   and are shareable, we must use this flags because we don't know if the 
					   	   user has ldg.prg deamon installed on his computer */
	NULL,				/* Function called when the plugin is unloaded */
	2L					/* Howmany file type are supported by this plugin */
};

uint16 	encode_compression 	= COMPRESSION_LZW;
int		quality 			= 75;	

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
	TIFF *tif;
	uint32 w, h, *raster;
	uint16 compression, bitpersample, samplesperpixel, planes;

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

	info->width 				= ( uint16)w;
	info->height 				= ( uint16)h;
	info->components			= 3;
	info->planes 				= planes;
	info->indexed_color			= FALSE;
	info->page = 1;
	
	planes = MIN( 24, planes);

	info->colors  				= ( uint32)( 1uL << planes);
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
		case 1:
			strcpy( info->compression, "None");	
			break;
		case 2: /* CCITT modified Huffman RLE */
			strcpy( info->compression, "HRLE");	
			break;
		case 3: /* CCITT Group 3 fax encoding */
			strcpy( info->compression, "FAX3");	
			break;
		case 4: /* CCITT Group 4 fax encoding */
			strcpy( info->compression, "FAX4");	
			break;
		case 5: /* Lempel-Ziv  & Welch */
			strcpy( info->compression, "LZW");	
			break;
		case 6: /* Old JPG, no supported ! */
			TIFFClose( tif);
			return FALSE;	
		case 7: /* JPEG DCT compression */
			strcpy( info->compression, "JPEG");	
			break;
		case 32766: /* NeXT 2-bit RLE */
			strcpy( info->compression, "NeXT");	
			break;
		case 32771: /* #1 w/ word alignment RLE*/
			strcpy( info->compression, "RLEw");	
			break;
		case 32773: /* Macintosh RLE */
			strcpy( info->compression, "MAC");	
			break;
		case 32809: /* ThunderScan RLE */
			strcpy( info->compression, "Thun");	
			break;
		case 32895: /* IT8 CT w/padding */
			strcpy( info->compression, "IT8");	
			break;
		case 32896: /* IT8 Linework RLE */
			strcpy( info->compression, "IT8R");	
			break;
		case 32897: /* IT8 Monochrome picture */
			strcpy( info->compression, "IT8M");	
			break;
		case 32898: /* IT8 Binary line art */
			strcpy( info->compression, "IT8B");	
			break;
		case 32908: /* Pixar companded 10/11bit LZW */
		case 32909:
			strcpy( info->compression, "Pixa");	
			break;
		case 8:
		case 32946: /* Deflate compression */
			strcpy( info->compression, "Defl");	
			break;
		case 32947: /* Kodak DCS encoding */
			strcpy( info->compression, "DCS");	
			break;
		case 34661: /* ISO JBIG */
			strcpy( info->compression, "JBIG");	
			break;
		case 34676: /* SGI Log Luminance RLE / 24bits*/
		case 34677:
			strcpy( info->compression, "SGI");	
			break;
		case 34712: /* Leadtools JPEG2000 */
			strcpy( info->compression, "JP20");	
			break;
		default:	
			strcpy( info->compression, "???");	
			break;
    }

	raster = ( uint32*)malloc( ( w * h) << 2);

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
    uint8 	r = 0, g = 0, b = 0, a = 0;
	uint16	i, j = 0;
	uint32	*img = ( uint32*)info->_priv_ptr_more;

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

	info->_priv_ptr_more = ( uint32*)info->_priv_ptr_more + info->width;

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
	TIFF 	*tif = ( TIFF*)info->__priv_ptr_more;

	free( ( uint32*)info->_priv_ptr);
	TIFFClose( tif);
}



/*==================================================================================*
 * boolean CDECL encoder_init:														*
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
boolean CDECL encoder_init( const char *name, IMGINFO info)
{	
	TIFF *tif;

	if(( tif = TIFFOpen( name, "w")) == NULL)
	{
		return FALSE;
    }
	
	TIFFSetField( tif, TIFFTAG_IMAGEWIDTH,		( uint32)info->width);
	TIFFSetField( tif, TIFFTAG_IMAGELENGTH,		( uint32)info->height);
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
 * boolean CDECLencoder_write:														*
 *		This function write data from buffer to file								*
 *----------------------------------------------------------------------------------*
 * input:																			*
 *		buffer		->	The source buffer.											*
 *		info		->	The IMGINFO struct		.									*
 *----------------------------------------------------------------------------------*
 * return:	 																		*
 *      TRUE if all ok else FALSE.													*
 *==================================================================================*/
boolean CDECL encoder_write( IMGINFO info, uint8 *buffer)
{
	if ( TIFFWriteScanline( ( TIFF *)info->_priv_ptr, buffer, info->_priv_var++, 0) < 0)
		return FALSE;

	return TRUE;
}



/*==================================================================================*
 * boolean CDECL encoder_quit:														*
 *		This function makes the last job like close the file handler and free the	*
 *		allocated memory.															*
 *----------------------------------------------------------------------------------*
 * input:																			*
 *		info		->	The IMGINFO struct. to fit.									*
 *----------------------------------------------------------------------------------*
 * return:	 																		*
 *      --																			*
 *==================================================================================*/
void CDECL encoder_quit( IMGINFO info)
{
	TIFF 	*tif = ( TIFF*)info->_priv_ptr;

	TIFFClose( tif);
}



/*==================================================================================*
 * boolean set_tiff_option:															*
 *		This function set some encoder's options							 		*
 *----------------------------------------------------------------------------------*
 * input:																			*
 *----------------------------------------------------------------------------------*
 * return:	 																		*
 *      --																			*
 *==================================================================================*/
void CDECL set_tiff_option( int16 set_quality, uint16 set_encode_compression)
{
	encode_compression 	= set_encode_compression;
	quality = set_quality;
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
	ldg_init( &tiff_plugin);
	return( 0);
}
