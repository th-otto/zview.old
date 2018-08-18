#include "../general.h"
#include "catalog_iconify.h"


/*==================================================================================*
 * void catalog_iconify:															*
 *		Function changes the window's name when the catalog window is iconified.	*
 *----------------------------------------------------------------------------------*
 * input:																			*
 *		win -> the window to handle.												*
 *----------------------------------------------------------------------------------*
 * returns: 																		*
 *      --																			*
 *==================================================================================*/
 
void __CDECL catalog_iconify( WINDOW *win EVNT_BUFF_PARAM)
{		
	WindSetStr( win, WF_NAME, "Zview"); 
}


/*==================================================================================*
 * void catalog_uniconify:															*
 *		Function restore the window's name when the catalog window is uniconified.	*
 *----------------------------------------------------------------------------------*
 * input:																			*
 *		win -> the window to handle.												*
 *----------------------------------------------------------------------------------*
 * returns: 																		*
 *      --																			*
 *==================================================================================*/

void __CDECL catalog_uniconify( WINDOW *win EVNT_BUFF_PARAM)
{
	WINDICON *wicones = ( WINDICON *)DataSearch( win, WD_ICON);
	
	WindSetStr( win, WF_NAME, wicones->directory);
}
