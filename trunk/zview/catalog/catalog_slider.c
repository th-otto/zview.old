#include "../general.h"
#include "../prefs.h"
#include "catalog_mini_entry.h"
#include "catalog.h"


/* prototype */
void WinCatalog_Arrow( WINDOW *win);
void WinCatalog_VSlide( WINDOW *win);
void WinCatalog_DownLine( WINDOW *win);
void WinCatalog_UpLine( WINDOW *win);
void WinCatalog_DownPage( WINDOW *win);
void WinCatalog_UpPage( WINDOW *win);
void move_area( int16 handle, GRECT *screen, int16 dx, int16 dy);
void move_main_work( WINDOW *win, int16 xw, int16 yw, int16 ww, int16 hw, int16 dx, int16 dy, int16 first_frame_width, int16 border_width); 
void move_frame_work( WINDOW *win, int16 dy); 


/*==================================================================================*
 * void move_area:																	*
 *		utility function for the move_*_work functions.								*
 *----------------------------------------------------------------------------------*
 * input:																			*
 * 		handle		-> 	The vdi handle.												*
 *		screen		-> 	The area coordinate to move.								*
 *		dy			-> 	The step for the move.										*
 *----------------------------------------------------------------------------------*
 * returns: 																		*	
 *		--																			*
 *==================================================================================*/
 
void move_area( int16 handle, GRECT *screen, int16 dx, int16 dy) 
{
	int16 	xy[8];
	MFDB 	screen_mfdb = {0};
	GRECT 	g;
	
	wind_get( 0, WF_CURRXYWH, &g.g_x, &g.g_y, &g.g_w, &g.g_h);

	rc_intersect( &g, screen);
	
	xy[0] = screen -> g_x;
	xy[1] = screen -> g_y;
	xy[2] = xy[0] + screen -> g_w - 1;
	xy[3] = xy[1] + screen -> g_h - 1;
	xy[4] = xy[0] + dx;
	xy[5] = xy[1] + dy;
	xy[6] = xy[2] + dx;
	xy[7] = xy[3] + dy;
	
	vro_cpyfm( handle, S_ONLY, xy, &screen_mfdb, &screen_mfdb);
}



/*==================================================================================*
 * void move_work:																	*
 *		this function moves the main frame ( entries) work area of the window.		*
 *----------------------------------------------------------------------------------*
 * input:																			*
 * 		win			-> 	The target window.											*
 *		dy			-> 	The step for the move.										*
 *----------------------------------------------------------------------------------*
 * returns: 																		*	
 *		--																			*
 *==================================================================================*/

void move_main_work( WINDOW *win, int16 xw, int16 yw, int16 ww, int16 hw, int16 dx, int16 dy, int16 first_frame_width, int16 border_width) 
{
	int16	x = xw, y = yw + 1, w = ww, h = hw - 1, absolute_dy, absolute_dx;
	GRECT	rect, r1, r2, screen;

	absolute_dy = abs( dy);
	absolute_dx = abs( dx);

	if ( dx || dy) 
	{
		x += ( first_frame_width + border_width);
		w -= ( first_frame_width + border_width);

		while( !wind_update(BEG_UPDATE));
		graf_mouse( M_OFF, 0L);		
		rc_set( &rect, x, y, w, h);

		wind_get_grect( 0, WF_CURRXYWH, &screen);
		wind_get( win -> handle, WF_FIRSTXYWH, &r1.g_x, &r1.g_y, &r1.g_w, &r1.g_h);

		while ( r1.g_w && r1.g_h) 
		{
			if( rc_intersect( &rect, &r1) && rc_intersect( &screen, &r1)) 
			{
				if ( ( absolute_dx < r1.g_w) && ( absolute_dy < r1.g_h))
				{
					r2 = r1; /* save */

					if ( dy > 0) 
					{
						r1.g_y += dy;
						r1.g_h -= dy;
					} 
					else
						r1.g_h += dy;

					if (dx > 0) 
					{
						r1.g_x += dx;
						r1.g_w -= dx;
					}
					else 
						r1.g_w += dx;
				
					move_area( win->graf.handle, &r1, -dx, -dy);

					if (dx)
					{
						r1 = r2 ;  /* restore */
						
						if (dx > 0) 
						{
							r1.g_x += r1.g_w - dx;
							r1.g_w = dx;
						} 
						else 
							r1.g_w = - dx;
						
						if (dy > 0) 
							r1.g_h -= dy;
						else 
						{
							r1.g_y -= dy;
							r1.g_h += dy;
						}
						rc_clip_on( win->graf.handle, &r1);
						EvntExec( win, WM_REDRAW);
						rc_clip_off( win->graf.handle);
					}

					if ( dy)
					{
						r1 = r2 ;  /* restore */

						if (dy > 0) 
						{
							r1.g_y += r1.g_h - dy;
							r1.g_h = dy;
						} 
						else
							r1.g_h = -dy;
						
						rc_clip_on( win->graf.handle, &r1);
						EvntExec( win, WM_REDRAW);
						rc_clip_off( win->graf.handle);
					}
				}
				else
				{
					rc_clip_on( win->graf.handle, &r1);
					EvntExec( win, WM_REDRAW);
					rc_clip_off( win->graf.handle);
				}
			}
			wind_get( win->handle, WF_NEXTXYWH, &r1.g_x, &r1.g_y, &r1.g_w, &r1.g_h);
		}
		graf_mouse( M_ON, 0L);
		wind_update(END_UPDATE);

		if ( dy) 
			WindSlider( win, VSLIDER);
		if ( dx) 
			WindSlider( win, HSLIDER);			
	}
}



/*==================================================================================*
 * void move_frame_work:															*
 *		this function moves the mini_entry frame work area of the window.			*
 *----------------------------------------------------------------------------------*
 * input:																			*
 * 		win			-> 	The target window.											*
 *		dy			-> 	The step fo the move.										*
 *----------------------------------------------------------------------------------*
 * returns: 																		*	
 *		--																			*
 *==================================================================================*/

void move_frame_work( WINDOW *win, int16 dy) 
{
	int16	x, y, w, h, absolute_dy;
	GRECT	rect, r1, r2, screen;

	absolute_dy = abs( dy);

	if ( dy) 
	{
		WindGet( win, WF_WORKXYWH, &x, &y, &w, &h);
		y++;
		h--;
		w = browser_frame_width - 16;

		while( !wind_update(BEG_UPDATE));
		graf_mouse( M_OFF, 0L);		
		rc_set( &rect, x + 1, y, w, h);
		wind_get_grect( 0, WF_CURRXYWH, &screen);
		wind_get( win -> handle, WF_FIRSTXYWH, &r1.g_x, &r1.g_y, &r1.g_w, &r1.g_h);

		while ( r1.g_w && r1.g_h) 
		{
			if( rc_intersect( &rect, &r1) && rc_intersect( &screen, &r1))
			{
				if ( ( absolute_dy < r1.g_h) )
				{
					r2 = r1; /* save */

					if ( dy > 0) 
					{
						r1.g_y += dy;
						r1.g_h -= dy;
					} 
					else
						r1.g_h += dy;
				
					move_area( win->graf.handle, &r1, 0, -dy);

					if ( dy)
					{
						r1 = r2 ;  /* restore */

						if (dy > 0) 
						{
							r1.g_y += r1.g_h - dy;
							r1.g_h = dy;
						} 
						else
							r1.g_h = -dy;
						
						rc_clip_on( win->graf.handle, &r1);
						EvntExec( win, WM_REDRAW);
						rc_clip_off( win->graf.handle);
					}
				}
				else
				{
					rc_clip_on( win->graf.handle, &r1);
					EvntExec( win, WM_REDRAW);
					rc_clip_off( win->graf.handle);
				}
			}
			wind_get( win->handle, WF_NEXTXYWH, &r1.g_x, &r1.g_y, &r1.g_w, &r1.g_h);
		}
		wind_update(END_UPDATE);
		graf_mouse( M_ON, 0L);

		if ( dy)
		{
		    WINDICON *wicones = (WINDICON *)DataSearch( win, WD_ICON);	

			need_frame_slider = 1;
			calc_mini_entry_slider( wicones, frame_slider_root);
	
		   	/* check it in the windom source, I'm not sure if the clipping area must be clip.g_x, clip.g_y, clip.g_w, clip.g_h  or not */
			ObjcWindDraw( win, frame_slider_root, SLIDERS_BACK, 2, x + browser_frame_width - 15, y, 16, h - 31);
		}
	}
}


/*==================================================================================*
 * void WinCatalog_VSlide:															*
 *		this function handle the WM_VSLID event for the main frame in the			* 
 *		catalog ( the entries).														*
 *----------------------------------------------------------------------------------*
 * input:																			*
 * 		win			-> 	The target window.											*
 *----------------------------------------------------------------------------------*
 * returns: 																		*	
 *		--																			*
 *==================================================================================*/

void WinCatalog_VSlide( WINDOW *win) 
{
	int32 	pos;
	int16	x, y, w, h, dy;
	int16 	old_ypos = win->ypos;

	WindGet( win, WF_WORKXYWH, &x, &y, &w, &h);
	pos = ( int32)( win->ypos_max - h / win->h_u) * ( int32)evnt.buff[4] / 1000L;

	if ( pos < 0) 
		pos = 0;

	dy = ( int16)(( pos - win->ypos) * win->h_u);
	win->ypos =  ( int16) pos;

	if( dy && ( old_ypos != win->ypos)) 
	{
		move_main_work( win, x, y, w, h, 0, dy, browser_frame_width, border_size);

		/* we look if image are visible and we create the preview */		
		WindMakePreview( win);
	}
}





/*==================================================================================*
 * void WinCatalog_DownPage:														*
 *		this function handle the WM_DNPAGE event for the main frame in the			* 
 *		catalog ( the entries).														*
 *----------------------------------------------------------------------------------*
 * input:																			*
 * 		win			-> 	The target window.											*
 *----------------------------------------------------------------------------------*
 * returns: 																		*	
 *		--																			*
 *==================================================================================*/

void WinCatalog_DownPage( WINDOW *win) 
{
	int16	page, x, y, w, h, dy;
	int32	old_pos = win -> ypos;

	WindGet( win, WF_WORKXYWH, &x, &y, &w, &h);

	page = h / win -> h_u;

	if ( win -> ypos < win -> ypos_max - page) 
	{
		win -> ypos = MIN( win->ypos_max, win->ypos) + page;
		win -> ypos = MIN( win -> ypos, win -> ypos_max - page);
		dy = ( int16) (( win->ypos - old_pos) * win->h_u);
		move_main_work( win, x, y, w, h, 0, dy, browser_frame_width, border_size);

		/* After each 'downpage', we look if image are visible and we create the preview */
		WindMakePreview( win);
	}
}


/*==================================================================================*
 * void WinCatalog_UpPage:															*
 *		this function handle the WM_UPPAGE event for the main frame in the			* 
 *		catalog ( the entries).														*
 *----------------------------------------------------------------------------------*
 * input:																			*
 * 		win			-> 	The target window.											*
 *----------------------------------------------------------------------------------*
 * returns: 																		*	
 *		--																			*
 *==================================================================================*/

void WinCatalog_UpPage( WINDOW *win) 
{
	int32	pos;
	int16	x, y, w, h, dy;

	if ( win -> ypos > 0L) 
	{
		WindGet( win, WF_WORKXYWH, &x, &y, &w, &h);
		pos = MAX( 0L, win->ypos - h / win->h_u);
		dy = ( int16) (( pos - win->ypos) * win->h_u);
		win->ypos = pos;
		move_main_work( win, x, y, w, h, 0, dy, browser_frame_width, border_size);

		/* After each 'uppage', we look if image are visible and we create the preview */
		WindMakePreview( win);
	}
}


/*==================================================================================*
 * void WinCatalog_UpLine:															*
 *		this function handle the WM_UPLINE event for the main frame in the			* 
 *		catalog ( the entries).														*
 *----------------------------------------------------------------------------------*
 * input:																			*
 * 		win			-> 	The target window.											*
 *----------------------------------------------------------------------------------*
 * returns: 																		*	
 *		--																			*
 *==================================================================================*/

void WinCatalog_UpLine( WINDOW *win) 
{
	if ( win -> ypos > 0L) 
	{
		int16	x, y, w, h;
		
		win->ypos --;
		WindGet( win, WF_WORKXYWH, &x, &y, &w, &h);
		move_main_work( win, x, y, w, h, 0, -win->h_u, browser_frame_width, border_size);

		/* After each 'upline', we look if image are visible and we create the preview */
		WindMakePreview( win);
	}
}


/*==================================================================================*
 * void WinCatalog_DownLine:														*
 *		this function handle the WM_DNLINE event for the main frame in the			* 
 *		catalog ( the entries).														*
 *----------------------------------------------------------------------------------*
 * input:																			*
 * 		win			-> 	The target window.											*
 *----------------------------------------------------------------------------------*
 * returns: 																		*	
 *		--																			*
 *==================================================================================*/

void WinCatalog_DownLine( WINDOW *win) 
{
	int16	x, y, w, h;

	WindGet( win, WF_WORKXYWH, &x, &y, &w, &h);

	if (( win -> ypos < win -> ypos_max - h / win -> h_u) && ( win -> ypos_max > h / win -> h_u )) 
	{
		win -> ypos ++;
		move_main_work( win, x, y, w, h, 0, win->h_u, browser_frame_width, border_size);

		/* After each 'downpage', we look if image are visible and we create the preview */
		WindMakePreview( win);
	}
}


/*==================================================================================*
 * void WinCatalog_Arrow:															*
 *		this function handle the WM_ARROWED event for the main frame in the			* 
 *		catalog ( the entries).														*
 *----------------------------------------------------------------------------------*
 * input:																			*
 * 		win			-> 	The target window.											*
 *----------------------------------------------------------------------------------*
 * returns: 																		*	
 *		--																			*
 *==================================================================================*/

void WinCatalog_Arrow( WINDOW *win) 
{
	switch( evnt.buff[4]) 
	{
		case WA_UPPAGE:
			WinCatalog_UpPage( win);
			break;
		case WA_DNPAGE:
			WinCatalog_DownPage( win);
			break;
		case WA_UPLINE:
			WinCatalog_UpLine( win);
			break;
		case WA_DNLINE:
			WinCatalog_DownLine( win);
			break;
		default:
			break;
	}
}

