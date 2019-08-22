#include "zview.h"
#include "imginfo.h"
#include "zvdoodle.h"

/*
 * 1.11 Handles salert.txt in codecs folder
 */

static void __CDECL init(void)
{
}


static PROC DOODLEFunc[] =
{
	{ "plugin_init", 	"", init },
	{ "reader_init", 	"", reader_init },
	{ "reader_read",    "", reader_read },
	{ "reader_quit",    "", reader_quit },
	{ "reader_get_txt", "", reader_get_txt }
};

LDGLIB doodle_plugin =
{
	0x111, 	/* Plugin version */
	5,					/* Number of plugin's functions */
	DOODLEFunc,			/* List of functions */
	"DOO\0",			/* File's type Handled */
	LDG_NOT_SHARED, 	/* The flags NOT_SHARED is used here.. even if zview plugins are reentrant 
					   	   and are shareable, we must use this flags because we don't know if the 
					   	   user has ldg.prg deamon installed on his computer */
	0,					/* Function called when the plugin is unloaded */
	0					/* Howmany file type are supported by this plugin */
};


int main(void)
{
	ldg_init(&doodle_plugin);
	return 0;
}
