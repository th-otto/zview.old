#include "general.h"
#include "catalog/catalog_icons.h"
#include "pic_load.h"
#include "menu.h"
#include "prefs.h"
#include "winimg.h"
#include "pdf_load.h"
#include "pdflib/pdflib.h"
#include "plugins.h"
#include "av_prot.h"
#include "wintimer.h"
#include "zvdi/vdi.h"
#include "catalog/catalog.h"
#include "version.h"
#include "version_date.h"

char const program_version[] = " " NAME_STRING " " __STRINGIFY(VER_MAJOR) "." __STRINGIFY(VER_MINOR) "." __STRINGIFY(VER_MICRO) " ";
char const program_date[] = VERSION_DATE;

#ifndef VA_START
#define VA_START 0x4711
#endif


WINDOW *win_catalog = NULL;

char 	zview_path[MAX_PATH];
char *zview_slb_dir;
char *zview_slb_dir_end;
char 	startup_path[MAX_PATH];

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
	
	if( pdf_initialized)
		pdf_exit();
#ifdef ZVPDF_SLB
	zvpdf_close();
#endif

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

static void __CDECL evnt_applexit(WINDOW *win, short buff[8])
{
	applexit();
}


static void applinit( void)
{		
	ApplInit();
	
/*	is it unusefull ? */
	if( _AESnumapps == -1)
		menu_register( _AESapid, "  zView   ");

	if( !RsrcLoad( "zview.rsc")) 
	{
		errshow(NULL, E_RSC);
		ApplExit();
		exit(1);
	}

	if( !vdi_init())
	{
		errshow(NULL, NO_EDDI);
		ApplExit();
		exit(1);
	}

	Pdomain(1);

	RsrcXtype( RSRC_XTYPE, NULL, 0);

	if( AvInit( "ZVIEW   ", A_START|A_QUOTE, 1000 ) >= 0)
    	EvntAttach( NULL, VA_START, va_start);

	/* Get the current path for the different file loading like icon, etc... */
	{
		size_t len;
		
		startup_path[0] = 'A' + Dgetdrv();
		startup_path[1] = ':';
		Dgetpath(startup_path + 2, 0);
		len = strlen(startup_path);
		if (len > 0 && startup_path[len - 1] != '\\' && startup_path[len - 1] != '/')
			strcat(startup_path, "\\");
		if (zview_path[0] == '\0')
			strcpy(zview_path, startup_path);
	}

	zview_slb_dir = malloc(strlen(zview_path) + 20);
	strcpy(zview_slb_dir, zview_path);
	strcat(zview_slb_dir, "slb\\");
	zview_slb_dir_end = zview_slb_dir + strlen(zview_slb_dir);
#if defined(__mcoldfire__)
	strcat(zview_slb_dir, "v4e\\");
#elif defined(__mc68020__) || defined(__mc68030__) || defined(__mc68040__) || defined(__mc68060__) || defined(__mc68080__) || defined(__apollo__)
	strcat(zview_slb_dir, "020\\");
#else
	strcat(zview_slb_dir, "000\\");
#endif

	prefs_read();

	MenuDesktop();	

	EvntAttach( NULL, AP_TERM, evnt_applexit);

	/* Load the plugins.. */
	
	if( plugins_init() == 0)
	{
		errshow(NULL, NOZCODECS);
		applexit();
	}

	TimerInit( 20);

	evnt.bclick   	= 258;
	evnt.bmask   	= 3;
	evnt.bstate   	= 0; 
}


#if __WINDOM_MAJOR__ >= 2
void snd_msg( WINDOW *win, int msg, int par1, int par2, int par3, int par4) {
	ApplWrite( _AESapid, msg, win?win->handle : 0, par1, par2, par3, par4);
}


void frm_cls( WINDOW *win)
{
	stdFormClose(win, 0, 0, NULL);
}


int CallStGuide(char *path)
{
	return ApplWrite( appl_find("ST-GUIDE"), VA_START, ADR(path), 0, 0, 0);
}
#endif


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
	if (argc > 0 && argv[0] && argv[0][0])
	{
		char *p1, *p2;
		strcpy(zview_path, argv[0]);
		p1 = strrchr(zview_path, '/');
		p2 = strrchr(zview_path, '\\');
		if (p1 == NULL || p2 > p1)
			p1 = p2;
		if (p1)
			*++p1 = '\0';
		else
			zview_path[0] = '\0';
	}
	applinit();

 	if ( argc > 1)
		WindView( argv[argc - 1]);

	for(;;) 
		EvntWindom( MU_MESAG|MU_TIMER|MU_KEYBD|MU_BUTTON);

	return 0;
}
