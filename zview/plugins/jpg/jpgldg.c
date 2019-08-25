#include <stdlib.h>
#include <stdio.h>
#include <jpeglib.h>
#define HAVE_BOOLEAN
#include "plugin.h"
#include "zvplugin.h"
#include <gem.h> /* for MFDB */
#include "jpgdh.h"
#include <libexif/exif-data.h>
#include <libexif/exif-utils.h>
#include "zvjpg.h"
#include "ldglib/ldg.h"

/*==================================================================================*
 * boolean set_jpg_option:															*
 *		This function set some encoder's options							 		*
 *----------------------------------------------------------------------------------*
 * input:																			*
 *----------------------------------------------------------------------------------*
 * return:	 																		*
 *      --																			*
 *==================================================================================*/
static void __CDECL set_jpg_option( int16_t set_quality, J_COLOR_SPACE set_color_space, boolean set_progressive)
{
	quality = set_quality;
	color_space = set_color_space;
	progressive = set_progressive;
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
static void __CDECL init( void)
{
    jpg_init();
}


static PROC LibFunc[] = 
{ 
	{ "plugin_init",    "", init},
	{ "reader_init",    "", reader_init},
	{ "reader_get_txt", "", reader_get_txt},
	{ "reader_read",    "", reader_read},
	{ "reader_quit",    "", reader_quit},
	{ "encoder_init", 	"", encoder_init},
	{ "encoder_write",	"", encoder_write},
	{ "encoder_quit", 	"", encoder_quit},
	{ "set_jpg_option",	"", set_jpg_option}
};


static LDGLIB plugin =
{
	0x200, 			/* Plugin version */
	sizeof(LibFunc) / sizeof(LibFunc[0]),					/* Number of plugin's functions */
	LibFunc,			/* List of functions */
	"JPG\0JPE\0JPEG\0",		/* File's type Handled */
	LDG_NOT_SHARED, 	/* The flags NOT_SHARED is used here.. */
	0,
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
	ldg_init( &plugin);
	return( 0);
}

