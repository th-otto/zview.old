#include "plugin.h"
#include "zvplugin.h"
#include "ldglib/ldg.h"


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
}


static PROC RAWFunc[] =
{
	{ "plugin_init", 	"", init},
	{ "reader_init", 	"", reader_init},
	{ "reader_get_txt", "", reader_get_txt},
	{ "reader_read", 	"", reader_read},
	{ "reader_quit", 	"", reader_quit}
};


LDGLIB raw_plugin =
{
	0x200, 			/* Plugin version */
	sizeof(RAWFunc) / sizeof(RAWFunc[0]),					/* Number of plugin's functions */
	RAWFunc,			/* List of functions */
	"RAW\0",			/* File's type Handled */
	LDG_NOT_SHARED, 	/* The flags NOT_SHARED is used here.. even if zview plugins are reentrant 
					   	   and are shareable, we must use this flags because we don't know if the 
					   	   user has ldg.prg deamon installed on his computer */
	0,			/* Function called when the plugin is unloaded */
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
	ldg_init( &raw_plugin);
	return( 0);
}
