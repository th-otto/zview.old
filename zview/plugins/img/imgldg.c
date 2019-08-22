#include "zview.h"
#include "imginfo.h"
#include "img.h"
#include "zvimg.h"

static void __CDECL init( void)
{
	libshare_init();
}


static PROC IMGFunc[] =
{
	{ "plugin_init", 	"", init},
	{ "reader_init", 	"", reader_init},
	{ "reader_get_txt", "", reader_get_txt},
	{ "reader_read", 	"", reader_read},
	{ "reader_quit", 	"", reader_quit}
};


static LDGLIB img_plugin =
{
	0x200, 				/* Plugin version */
	sizeof(IMGFunc) / sizeof(IMGFunc[0]),					/* Number of plugin's functions */
	IMGFunc,			/* List of functions */
	"IMG\0",				/* File's type Handled */
	LDG_NOT_SHARED, 	/* The flags NOT_SHARED is used here.. even if zview plugins are reentrant 
						   and are shareable, we must use this flags because we don't know if the 
						   user has ldg.prg deamon installed on his computer */
	libshare_exit,				/* Function called when the plugin is unloaded */
	0					/* Howmany file type are supported by this plugin */
};



int main(void)
{
	ldg_init( &img_plugin);
	return( 0);
}
