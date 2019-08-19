#include <tiffio.h>
#define HAVE_INTS_DEFINED
#include "zview.h"
#include "imginfo.h"
#include "zvtiff.h"
#include "plugin_version.h"

/*==================================================================================*
 * boolean set_tiff_option:															*
 *		This function set some encoder's options							 		*
 *----------------------------------------------------------------------------------*
 * input:																			*
 *----------------------------------------------------------------------------------*
 * return:	 																		*
 *      --																			*
 *==================================================================================*/
static void __CDECL set_tiff_option( int16_t set_quality, uint16_t set_encode_compression)
{
	encode_compression 	= set_encode_compression;
	quality = set_quality;
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
static void __CDECL init( void)
{
	libshare_init();
}


static PROC TIFFFunc[] =
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


static LDGLIB tiff_plugin =
{
	0x200, 	/* Plugin version */
	sizeof(TIFFFunc) / sizeof(TIFFFunc[0]),					/* Number of plugin's functions */
	TIFFFunc,			/* List of functions */
	"TIF\0TIFF\0",			/* File's type Handled */
	LDG_NOT_SHARED, 	/* The flags NOT_SHARED is used here.. even if zview plugins are reentrant 
					   	   and are shareable, we must use this flags because we don't know if the 
					   	   user has ldg.prg deamon installed on his computer */
	libshare_exit,				/* Function called when the plugin is unloaded */
	0					/* Howmany file type are supported by this plugin */
};

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
