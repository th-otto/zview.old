#include "../general.h"
#include "catalog.h"
#include "../prefs.h"

/* Prototype */
void WinCatalog_Fulled( WINDOW *win); 
void WinCatalog_top( WINDOW *win); 
void WinCatalog_bottom( WINDOW *win);


/*==================================================================================*
 * void WinCatalog_Fulled:															*
 *		this function handle the WM_FULLED event for the main frame in the			* 
 *		catalog ( the entries).														*
 *----------------------------------------------------------------------------------*
 * input:																			*
 * 		win			-> 	The target window.											*
 *----------------------------------------------------------------------------------*
 * returns: 																		*	
 *		--																			*
 *==================================================================================*/

void WinCatalog_Fulled( WINDOW *win) 
{
	int16 		x, y, w, h;
	WINDICON 	*wicones = (WINDICON *)DataSearch( win, WD_ICON);

	if( win->status & WS_FULLSIZE) 
	{
		wind_get( evnt.buff[3], WF_PREVXYWH, &x, &y, &w, &h);
		wind_set( evnt.buff[3], WF_CURRXYWH, x, y, w, h);
	} else
		wind_set( evnt.buff[3], WF_CURRXYWH, app.x, app.y, win->w_max, win->h_max);
	
	win->status ^= WS_FULLSIZE;

	win 	-> ypos = 0;
	wicones -> ypos = 0;

	WindSlider( win, VSLIDER);

	WindGet ( win, WF_WORKXYWH, &x, &y, &w, &h);
	draw_page( win, x + browser_frame_width + border_size , y, w - ( browser_frame_width + border_size), h);
	WindMakePreview( win);
}


/*==================================================================================*
 * void WinCatalog_top:																*
 *		this function handle the WM_TOPPED event for the main frame in the			* 
 *		catalog ( the entries).														*
 *----------------------------------------------------------------------------------*
 * input:																			*
 * 		win			-> 	The target window.											*
 *----------------------------------------------------------------------------------*
 * returns: 																		*	
 *		--																			*
 *==================================================================================*/

void WinCatalog_top( WINDOW *win) 
{
	WINDICON *wicones = (WINDICON *)DataSearch( win, WD_ICON);
	OBJECT *menu = get_tree( MENU_BAR);

	if( !wicones->first_selected)
	{
		menu_ienable( menu, MENU_BAR_INFORMATION, 0);
		menu_ienable( menu, MENU_BAR_SAVE, 0);
		menu_ienable( menu, MENU_BAR_DELETE, 0);
	}
	else
	{
		if( wicones->first_selected->type == ET_IMAGE)
			menu_ienable( menu, MENU_BAR_SAVE, 1);

		menu_ienable( menu, MENU_BAR_DELETE, 1);
		menu_ienable( menu, MENU_BAR_INFORMATION, 1); 
	}

	menu_ienable( menu, MENU_BAR_SHOW_FULLSCREEN, 0);
}


/*==================================================================================*
 * void WinCatalog_bottom:															*
 *		this function handle the WM_BOTTOMED event for the main frame in the		* 
 *		catalog ( the entries).														*
 *----------------------------------------------------------------------------------*
 * input:																			*
 * 		win			-> 	The target window.											*
 *----------------------------------------------------------------------------------*
 * returns: 																		*	
 *		--																			*
 *==================================================================================*/

void WinCatalog_bottom( WINDOW *win) 
{
	win = win; // for prevent compiler warning
	menu_ienable( get_tree( MENU_BAR), MENU_BAR_INFORMATION, 0); 
	menu_ienable( get_tree( MENU_BAR), MENU_BAR_SAVE, 0);
	menu_ienable( get_tree( MENU_BAR), MENU_BAR_DELETE, 0);
}

