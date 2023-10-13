#include "plugin.h"
#include "zvplugin.h"
#include <webp/decode.h>
#include <webp/encode.h>
#include "zvwebp.h"
#include "ldglib/ldg.h"

/*==================================================================================*
 * boolean set_webp_option:															*
 *		This function set some encoder's options							 		*
 *----------------------------------------------------------------------------------*
 * input:																			*
 *----------------------------------------------------------------------------------*
 * return:	 																		*
 *      --																			*
 *==================================================================================*/
static void __CDECL set_webp_option( int16_t set_quality, uint16_t set_encode_compression)
{
	compression_level = set_encode_compression;
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
static void __CDECL init(void)
{
}

static PROC WEBPFunc[] = {
	{ "plugin_init", "", init },
	{ "reader_init", "", reader_init },
	{ "reader_get_txt", "", reader_get_txt },
	{ "reader_read", "", reader_read },
	{ "reader_quit", "", reader_quit },
	{ "encoder_init", 	"", encoder_init},
	{ "encoder_write",	"", encoder_write},
	{ "encoder_quit", 	"", encoder_quit},
	{ "set_webp_option","", set_webp_option}
};


static LDGLIB webp_plugin = {
	0x200,								/* Plugin version */
	sizeof(WEBPFunc) / sizeof(WEBPFunc[0]),	/* Number of plugin's functions */
	WEBPFunc,							/* List of functions */
	"WEB\0WEBP\0",						/* File's type Handled */
	LDG_NOT_SHARED,						/* The flags NOT_SHARED is used here.. even if zview plugins are reentrant
										   and are shareable, we must use this flags because we don't know if the
										   user has ldg.prg deamon installed on his computer */
	0,									/* Function called when the plugin is unloaded */
	1									/* Howmany file type are supported by this plugin */
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
int main(void)
{
	ldg_init(&webp_plugin);
	return 0;
}
