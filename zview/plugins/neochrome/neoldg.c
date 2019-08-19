#include "zview.h"
#include "imginfo.h"
#include "zvneo.h"
#include "plugin_version.h"

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
	libshare_init();
}

static PROC NEOFunc[] = 
{
	{ "plugin_init", 	"", init},
	{ "reader_init", 	"", reader_init},
	{ "reader_get_txt", "", reader_get_txt},
	{ "reader_read", 	"", reader_read},
	{ "reader_quit", 	"", reader_quit}
};


static LDGLIB neo_plugin =
{
	0x200, 			/* Plugin version */
	sizeof(NEOFunc) / sizeof(NEOFunc[0]),					/* Number of plugin's functions */
	NEOFunc,				/* List of functions */
	"NEO\0",					/* File's type Handled */
	LDG_NOT_SHARED, 		/* The flags NOT_SHARED is used here.. even if zview plugins are reentrant 
						       and are shareable, we must use this flags because we don't know if the 
							   user has ldg.prg deamon installed on his computer */	
	libshare_exit,				/* Function called when the plugin is unloaded */
	0						/* Howmany file type are supported by this plugin */
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
	ldg_init( &neo_plugin);
	return( 0);
}
