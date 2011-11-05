#include "general.h"
#include "catalog/catalog_icons.h"
#include "menu.h"
#include "prefs.h"
#include "pic_load.h"
#include "winimg.h"
#include "pdf_load.h"
#include "pdf/pdflib.h"
#include "plugins.h"
#include "av_prot.h"
#include "wintimer.h"
#include "zvdi/vdi.h"
#include "catalog/catalog.h"
#include <av.h>


WINDOW *win_catalog = NULL;

char 	zview_path[MAX_PATH];
int16	mint_os = 0, magic_os = 0;

/*==================================================================================*
 * void applexit:																	*
 *		The clean "end" function:													* 
 *		- Close all the windows.													*
 * 		- Free up the memory used by the plugins ( if any), the icons and the rsc	* 
 *		  file.																		*
 *		- Close the windom's av_client function.									*
 *		- Finnish the Application.													*
 *----------------------------------------------------------------------------------*
 * input:																			*
 *		--																			*
 *----------------------------------------------------------------------------------*
 * returns: 																		*
 *      --																			*
 *==================================================================================*/

void applexit( void) 
{
    while( wglb.first) 
    {
        snd_msg( wglb.first, WM_DESTROY, 0, 0, 0, 0);
        EvntWindom( MU_MESAG);
    }

	EvntDelete( NULL, WM_XTIMER);
	
	if( pdf_initialised == TRUE)
		pdf_exit();
	
	( void)prefs_write();

	plugins_quit();

	icons_exit();

	MenuBar ( NULL, 0);	
	RsrcXtype( 0, NULL, 0);
	RsrcFree();
	AvExit();
	ApplExit();
	#ifdef DEBUG
	gMemReport();
	#endif

	exit( 0);
}

/*==================================================================================*
 * void applinit:																	*
 *		The global initialisation function:											* 
 *		- Initialise the application via Windom ( "ApplInit").						*
 * 		- Inquire if MagiC/MiNT are present and set the good Pdomain().				*
 *		- Inquire screen's information ( "vdi_init").								*
 *		- Register the application with the AES ( "menu_register").					*
 *		- Load the ressource's file	and read the application prefs ( "prefs_read").	*
 *		- Initialise the av_client , the menu bar and load the plugins ( if any).	* 
 *----------------------------------------------------------------------------------*
 * input:																			*
 *		--																			*
 *----------------------------------------------------------------------------------*
 * returns: 																		*
 *      --																			*
 *==================================================================================*/

static void applinit( void)
{		
	ApplInit();

	if( !vdi_init())
	{
		errshow( "", NO_EDDI);
		ApplExit();
		exit( 0);
	}


/*	is it unusefull ? */
	if( _AESnumapps == -1)
		menu_register( app.id, "  zView   ");

	if( !RsrcLoad( "zview.rsc")) 
	{
		errshow( "", E_RSC);
		ApplExit();
		exit( 0);
	}

	magic_os = vq_magx();
	mint_os  = vq_mint();

	if ( magic_os || mint_os)
	{
		( void)Pdomain( 1);
	}

	RsrcXtype( RSRC_XTYPE, NULL, 0);

	if( AvInit( "ZVIEW   ", A_START|A_QUOTE, 1000 ) >= 0)
    	EvntAttach( NULL, VA_START, va_start);

	/* Get the current path for the different file loading like icon, etc... */
	zview_path[0] = 'A' + Dgetdrv();
	zview_path[1] = ':';
	Dgetpath( zview_path + 2, 0);

	prefs_read();

	MenuDesktop();	

    EvntAttach( NULL, AP_TERM,  applexit);

	/* Load the plugins.. */
	
	if( plugins_init() == 0)
	{
		errshow( "", NOZCODECS);
		applexit();
	}

	TimerInit( 20);

	evnt.bclick   	= 258;
	evnt.bmask   	= 3;
	evnt.bstate   	= 0; 
}


/*==================================================================================*
 * int main:																		*
 *		The main function:															* 
 *----------------------------------------------------------------------------------*
 * input:																			*
 *		argc, *argv ->	the application's parameters.								*
 *----------------------------------------------------------------------------------*
 * returns: 																		*
 *      0.																			*
 *==================================================================================*/

int main( int argc, char *argv[])
{
	applinit();

 	if ( argc > 1)
		WindView( argv[argc - 1]);

	for(;;) 
		EvntWindom( MU_MESAG|MU_TIMER|MU_KEYBD|MU_BUTTON);

	return 0;
}
