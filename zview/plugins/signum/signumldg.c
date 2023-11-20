#include "plugin.h"
#include "zvplugin.h"
#include "ldglib/ldg.h"

static void __CDECL init(void)
{
}


static PROC SIGNUMFunc[] =
{
	{ "plugin_init", 	"", init },
	{ "reader_init", 	"", reader_init },
	{ "reader_read",    "", reader_read },
	{ "reader_quit",    "", reader_quit },
	{ "reader_get_txt", "", reader_get_txt }
};

LDGLIB signum_plugin =
{
	0x100, 	/* Plugin version */
	5,					/* Number of plugin's functions */
	SIGNUMFunc,			/* List of functions */
	"IMC\0",			/* File's type Handled */
	LDG_NOT_SHARED, 	/* The flags NOT_SHARED is used here.. even if zview plugins are reentrant 
					   	   and are shareable, we must use this flags because we don't know if the 
					   	   user has ldg.prg deamon installed on his computer */
	0,					/* Function called when the plugin is unloaded */
	1					/* Howmany file type are supported by this plugin */
};


int main(void)
{
	ldg_init(&signum_plugin);
	return 0;
}
