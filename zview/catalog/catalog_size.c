#include "../general.h"
#include "../prefs.h"
#include "catalog_mini_entry.h"
#include "catalog.h"

/* Prototype */
void WinCatalog_Size( WINDOW *win);

/*==================================================================================*
 * void WinCatalog_Size:															*
 *		this function handle the WM_SIZED event for the main frame in the			* 
 *		catalog ( the entries).														*
 *----------------------------------------------------------------------------------*
 * input:																			*
 * 		win			-> 	The target window.											*
 *----------------------------------------------------------------------------------*
 * returns: 																		*	
 *		--																			*
 *==================================================================================*/

void WinCatalog_Size( WINDOW *win) 
{
	int16 dum, x, y, w, h, old_h, rdw_win = 0, rdw_frame = 0;
	uint32 old_win_ypos, old_frame_ypos;
    WINDICON *wicones = (WINDICON *)DataSearch( win, WD_ICON);	

	x = MAX( evnt.buff[6], win -> w_min);
	y = MAX( evnt.buff[7], win -> h_min);

	w = MIN( x, win -> w_max);
	h = MIN( y, win -> h_max);

	WindGet( win, WF_WORKXYWH, &dum, &dum, &dum, &old_h);
	
	wind_set( evnt.buff[3], WF_CURRXYWH, evnt.buff[4], evnt.buff[5], w, h);

	WindGet( win, WF_WORKXYWH, &x, &y, &w, &h);

	old_win_ypos 	= win -> ypos;
	old_frame_ypos 	= wicones -> ypos;

	if( ( uint16)h > wicones -> ypos_max * wicones -> h_u)
		wicones -> ypos = 0;
	else
		wicones -> ypos = MIN( wicones -> ypos, wicones -> ypos_max - h / wicones -> h_u);

	if( ( uint16)h > win -> ypos_max * win -> h_u)
		win -> ypos = 0;
	else
		win -> ypos = MIN( win -> ypos, win -> ypos_max - h / win -> h_u);

	if( old_frame_ypos != wicones -> ypos)
	{	
		rdw_frame = 1;
	}

	if( old_win_ypos != win -> ypos)
	{	
		rdw_win = 1;
	}
	else if ( w >= (( wicones->columns + 1) * wicones->case_w + border_size + browser_frame_width) || w < wicones->columns * wicones->case_w + border_size + browser_frame_width)
	{	
		if ( w < (( wicones->nbr_icons + 1) * wicones->case_w + border_size + browser_frame_width))	   	 
			rdw_win = 1;
	}
	
	win->status &= ~WS_FULLSIZE; 

	if( rdw_win)
	{ 
		draw_page( win, x + browser_frame_width + border_size , y + 1, w - ( browser_frame_width + border_size), h - 1);
		
		WindMakePreview( win);
	}
	else if( ( old_h != h) && browser_frame_width)
	{
		if( rdw_frame)
			draw_page( win, x, y + 1, browser_frame_width, h - 1);
		else if( ( wicones -> ypos_max * wicones -> h_u) >= h)
		{
			need_frame_slider						= 1;
			frame_slider_root->ob_x 				= x + browser_frame_width - 15;
			frame_slider_root->ob_y 				= y + 2;
			frame_slider_root->ob_height  			= h - 3;	
			
			frame_slider_root[SLIDERS_BACK].ob_height = h - 31;			

			frame_slider_root[SLIDERS_UP].ob_y 		= frame_slider_root->ob_height - 31;		
			frame_slider_root[SLIDERS_DOWN].ob_y 	= frame_slider_root->ob_height - 15;		
	
			calc_mini_entry_slider( wicones, frame_slider_root);

		   	/* check it in the windom source, I'm not sure if the clipping area must be clip.g_x, clip.g_y, clip.g_w, clip.g_h  or not */
			ObjcWindDraw( win, frame_slider_root, SLIDERS_BOX, 2, x + browser_frame_width - 15, y, 16, h);

			WindSlider( win, VSLIDER);
		}
		else /* if( need_frame_slider) */
		{
			need_frame_slider = 0;
			draw_page( win, x + browser_frame_width - 16 , y + 1, 16, h - 1);		   	
		}
	}
	else if( old_h != h)
	{
		WindSlider( win, VSLIDER);
	}
}
