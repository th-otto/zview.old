#include "general.h"
#include "prefs.h"
#include "mfdb.h"
#include "txt_data.h"
#include "pic_load.h"
#include "winimg.h"
#include "full_scr.h"
#include "pdf_load.h"
#include "menu.h"
#include "infobox.h"
#include "pdflib.h"
#include "pic_resize.h"
#include "wintimer.h"
#include "ztext.h"
#include "custom_font.h"
#include "catalog/catalog.h"
#include "catalog/catalog_other_event.h"
#include "catalog/catalog_icons.h"
#include "catalog/catalog_slider.h"


static int16 	dum;
static OBJECT	*menu;

/* Prototype */
void calc_slider( WINDATA *windata, OBJECT *slider_root);


/*==================================================================================*
 * void WindPdfSize:																*
 *		this function handle the WM_SIZED event for the WINDOW win					*
 *----------------------------------------------------------------------------------*
 * input:																			*
 * 		win			-> 	The target window.											*
 *----------------------------------------------------------------------------------*
 * returns: 																		*
 *		--																			*
 *==================================================================================*/

static void WindPdfSize( WINDOW *win)
{
	int16 x, y, w, h, old_h, rdw_frame = 0;
	uint32 old_frame_ypos;
	WINDATA	*windata = ( WINDATA *)DataSearch( win, WD_DATA);

	x = MAX( evnt.buff[6], win -> w_min);
	y = MAX( evnt.buff[7], win -> h_min);

	w = MIN( x, win -> w_max);
	h = MIN( y, win -> h_max);

	WindGet( win, WF_WORKXYWH, &dum, &dum, &dum, &old_h);

	wind_set( evnt.buff[3], WF_CURRXYWH, evnt.buff[4], evnt.buff[5], w, h);

	WindGet( win, WF_WORKXYWH, &x, &y, &w, &h);

	old_frame_ypos 	= windata -> ypos;

	if( ( uint16)h > windata -> ypos_max * windata -> h_u)
		windata -> ypos = 0;
	else
		windata -> ypos = MIN( windata -> ypos, windata -> ypos_max - h / windata -> h_u);

	if( ( uint16)h > win -> ypos_max * win -> h_u)
		win -> ypos = 0;
	else
		win -> ypos = MIN( win -> ypos, win -> ypos_max - h / win -> h_u);

	if( ( uint16)w > win -> xpos_max * win -> w_u)
		win -> xpos = 0;
	else
		win -> xpos = MIN( win -> xpos, win -> xpos_max - w / win -> w_u);


	if( old_frame_ypos != windata -> ypos)
		rdw_frame = 1;

	win->status &= ~WS_FULLSIZE;

	draw_page( win, x + windata->frame_width + windata->border_width , y, w - ( windata->frame_width + windata->border_width), h);

	if( ( old_h != h) && windata->frame_width)
	{
		if( rdw_frame)
			draw_page( win, x, y, windata->frame_width, h);
		else
			draw_page( win, x + windata->frame_width - 16, y, 16, h);
	}

	WindSlider( win, VSLIDER|HSLIDER);
}



/*==================================================================================*
 * void move_bookmark_work:															*
 *		this function moves the 'bookmark' frame work area of the window.			*
 *----------------------------------------------------------------------------------*
 * input:																			*
 * 		win			-> 	The target window.											*
 *		dy			-> 	The step fo the move.										*
 *----------------------------------------------------------------------------------*
 * returns: 																		*
 *		--																			*
 *==================================================================================*/

static void move_bookmark_work( WINDOW *win, int16 dy, WINDATA *windata)
{
	int16	x, y, w, h, absolute_dy;
	GRECT	rect, r1, r2, screen;

	absolute_dy = abs( dy);

	if ( dy)
	{
		WindGet( win, WF_WORKXYWH, &x, &y, &w, &h);
		y++;
		h--;
		w = windata->frame_width - 16;

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
			calc_slider( windata, windata->frame_slider);

		   	/* check it in the windom source, I'm not sure if the clipping area must be clip.g_x, clip.g_y, clip.g_w, clip.g_h  or not */
			ObjcWindDraw( win, windata->frame_slider, SLIDERS_BACK, 2, x + windata->frame_width - 15, y, 16, h - 31);
		}
	}
}




/*==================================================================================*
 * redraw_mini_entry:																*
 *		Send a redraw event for a mini_entry in the WINDOW *win.					*
 *----------------------------------------------------------------------------------*
 * input:																			*
 * 		win     = the target window													*
 * 		entry	= the icon to be redraw												*
 *----------------------------------------------------------------------------------*
 * return:																			*
 *		--																			*
 *==================================================================================*/

static void redraw_bookmark( WINDOW *win, int16 frame_width, Bookmark *entry, int16 xw, int16 yw, int16 hw)
{
	int16 x, y, w, h;

	x = entry->arrow_position.x1;
	y = entry->arrow_position.y1 - 5;
	w = entry->arrow_position.x2 + x_space + entry->txt_width + 20;
	h = entry->arrow_position.y2 - y + 10;

	x += xw;
	y += yw;

	/* send the redraw event only if the bookmark is visible */
	if (( y + entry->arrow_position.y2 > yw) && ( y < yw + hw))
		draw_page( win, x, y, MIN( w, frame_width), h);
}


static int16 find_bookmark_child_on_mouse( WINDOW *win, WINDATA *windata, Bookmark *entry, int16 mouse_x, int16 mouse_y, int16 x, int16 y, int16 w, int16 h)
{
	int16 	i, zoom;
	char 	temp[20];
	double 	scale = 1.0;

	if( entry->child == NULL)
		 return( 0);

	for ( i = 0; i < entry->nbr_child ; i++)
	{
		if ( entry->child[i].valid == FALSE)
			continue;

		if ( entry->child[i].state == ON)
			if ( find_bookmark_child_on_mouse( win, windata, &entry->child[i], mouse_x, mouse_y, x, y, w, h))
				return ( 1);

		if (( mouse_x >= entry->child[i].arrow_position.x2 + x_space && mouse_x <= entry->child[i].arrow_position.x2 + x_space + entry->child[i].txt_width	&& mouse_y >= entry->child[i].arrow_position.y1 && mouse_y <= entry->child[i].arrow_position.y2))
		{
			if ( windata->selected != &entry->child[i])
			{
				Bookmark *old_selected = windata->selected;
				windata->selected = &entry->child[i];

				if ( old_selected)
					redraw_bookmark( win, windata->frame_width, old_selected, x, y, h);

				redraw_bookmark( win, windata->frame_width, windata->selected, x, y, h);
			}

			if( entry->child[i].linked_page == windata->page_to_show || entry->child[i].linked_page > windata->img.page || entry->child[i].linked_page < 0)
				return( 1);

			graf_mouse( BUSYBEE, NULL);

			if( windata->img.image[windata->page_to_show].fd_addr != NULL)
			{
				gfree( windata->img.image[windata->page_to_show].fd_addr);
				windata->img.image[windata->page_to_show].fd_addr = NULL;
			}

   		    if( pdf_fit_to_win)
				scale = get_scale_value( &windata->img, entry->child[i].linked_page + 1, w - ( windata->frame_width + windata->border_width + 20), h - 20);

			zoom = windata->zoom_level;

			if( read_pdf( &windata->img, entry->child[i].linked_page + 1, scale))
			{
				scale = scale * 100.0;
				sprintf( temp, "%.0f%%", scale);
				windata->zoom_level	= ( int16)scale;

				ObjcStrCpy( win->tool.root, PDFTOOLBAR_PERCENT, temp);
				ObjcDraw( OC_TOOLBAR, win, PDFTOOLBAR_PERCENT, 1);

				sprintf( temp, "%d of %d", entry->child[i].linked_page + 1, windata->img.page);
				ObjcStrCpy( win->tool.root, PDFTOOLBAR_PAGE, temp);
				ObjcDraw( OC_TOOLBAR, win, PDFTOOLBAR_PAGE, 1);

				windata->page_to_show = entry->child[i].linked_page;

				win -> ypos_max = ( int32)( ( windata->img.image[windata->page_to_show].fd_h) >> 3);
				win -> xpos_max = ( int32)( ( windata->img.image[windata->page_to_show].fd_w + windata->frame_width + windata->border_width) >> 3);

				ObjcDraw( OC_TOOLBAR, win, PDFTOOLBAR_PREVIOUS, 1);
				ObjcDraw( OC_TOOLBAR, win, PDFTOOLBAR_NEXT, 1);

				if( zoom >= 200 && windata->zoom_level < 200)
					ObjcDraw( OC_TOOLBAR, win, PDFTOOLBAR_BIG, 1);

				if( zoom <= 25 && windata->zoom_level > 25)
					ObjcDraw( OC_TOOLBAR, win, PDFTOOLBAR_SMALL, 1);
			}

			win -> ypos 	= 0;
			win -> xpos 	= 0;

			draw_page( win, x  + windata->frame_width + windata->border_width, y, w - ( windata->frame_width + windata->border_width) , h);

			graf_mouse( ARROW, NULL);
			return( 1);
		}
		else if ( entry->child[i].state != UNKNOWN)
		{
			if (( mouse_x >= entry->child[i].arrow_position.x1 && mouse_x <= entry->child[i].arrow_position.x2 && mouse_y >= entry->child[i].arrow_position.y1	&& mouse_y <= entry->child[i].arrow_position.y2))
			{
				entry->child[i].state = ( entry->child[i].state == ON) ? OFF : ON;

				draw_page( win, x, y, windata->frame_width, h);

				return( 1);
			}
		}
	}
	return( 0);
}




/*==================================================================================*
 * void WindPdfMouse:																*
 *		this function handle the mouse event for the winview WINDOW					*
 *		catalog ( the entries).														*
 *----------------------------------------------------------------------------------*
 * input:																			*
 * 		win			-> 	The window to handle.										*
 *----------------------------------------------------------------------------------*
 * returns: 																		*
 *		--																			*
 *==================================================================================*/

static void WindPdfMouse( WINDOW *win)
{
	WINDATA	*windata = ( WINDATA *)DataSearch( win, WD_DATA);
	GRECT 		mouse;
	int16 		zoom, x, y, w, h, old_x, old_y, i = 0, nb_click, in_frame_border = 0, in_bookmark_frame = 0;
	char		temp[20];
	double		scale = 1.0;
//	Entry 		*entry_ptr = NULL, *old_selected = wicones->first_selected;

	WindGet( win, WF_WORKXYWH, &x, &y, &w, &h);

 	mouse.g_x 				= evnt.mx - x;
	mouse.g_y 				= evnt.my - y;
	old_x     				= evnt.mx;
	old_y     				= evnt.my;
	nb_click  				= evnt.nb_click;

	if ( windata->frame_width && ( mouse.g_x >= windata->frame_width  && mouse.g_x <= windata->frame_width + windata->border_width))
	{	/* mouse on frame border */
		in_frame_border = 1;
	}
	else if ( windata->frame_width && ( mouse.g_x < windata->frame_width + windata->border_width))
	{	/* mouse on browser */
		in_bookmark_frame = 1;
	}

	if ( in_frame_border && evnt.mbut == 1)
	{
		int16 last_mouse_x;

		graf_mouse( FLAT_HAND, NULL);

		while( !wind_update( BEG_MCTRL));

		graf_dragbox( windata->border_width, h, x + windata->frame_width, y, x + 100, y, w - ( 180), h, &last_mouse_x, &dum );

		/* Set the new toolbar position */
		if ( last_mouse_x != x + windata->frame_width)
		{
			windata->frame_width =  last_mouse_x - x;

			win -> w_min = windata->frame_width + windata->border_width + 80;
			win -> xpos_max = ( int32)( ( windata->img.image[windata->page_to_show].fd_w + windata->frame_width + windata->border_width - 1) >> 3);

			EvntRedraw( win);
			WindSlider ( win, HSLIDER);
		}

		wind_update( END_MCTRL);

		graf_mouse( ARROW, NULL);
	}
	else if ( in_bookmark_frame)
	{
		if( ( ( windata -> ypos_max * windata -> h_u) >= h) && ( mouse.g_x > windata->frame_width - 15) && ( mouse.g_x < windata->frame_width))
		{
			int16 res;

			windata->frame_slider->ob_x = x + windata->frame_width - 15;
			windata->frame_slider->ob_y = y + 2;

			res = objc_find( windata->frame_slider, SLIDERS_BOX, 2, evnt.mx, evnt.my);

			if( res != -1)
			{
				int32 	pos, old_ypos = windata->ypos;
				int16	dy, selected_object, page, redraw_arrow_slider = 0;

				switch ( res)
				{
					case SLIDERS_UP:
						do
						{
							if ( windata -> ypos > 0L)
							{
								if( redraw_arrow_slider == 0)
									ObjcWindChange( win, windata->frame_slider, SLIDERS_UP, windata->frame_slider->ob_x, windata->frame_slider[SLIDERS_UP].ob_y + y, windata->frame_slider[SLIDERS_UP].ob_width + 1, windata->frame_slider[SLIDERS_UP].ob_height + 1, SELECTED);

								windata -> ypos--;
								move_bookmark_work( win, -windata->h_u, windata);
								redraw_arrow_slider	= 1;
							}

							graf_mkstate( &dum, &dum, &res, &dum);

						}while( res);

						if( redraw_arrow_slider)
						{
							windata->frame_slider[SLIDERS_UP].ob_state &= ~SELECTED;
							ObjcWindDraw( win, windata->frame_slider, SLIDERS_UP, 1, windata->frame_slider->ob_x, windata->frame_slider[SLIDERS_UP].ob_y + y, windata->frame_slider[SLIDERS_UP].ob_width + 1, windata->frame_slider[SLIDERS_UP].ob_height + 1);
						}
						break;

					case SLIDERS_DOWN:
						do
						{
							if (( windata -> ypos < windata -> ypos_max - h / windata -> h_u) && ( windata -> ypos_max > h / windata -> h_u ))
							{
								if( redraw_arrow_slider == 0)
									ObjcWindChange( win, windata->frame_slider, SLIDERS_DOWN, windata->frame_slider->ob_x, windata->frame_slider[SLIDERS_DOWN].ob_y + y, windata->frame_slider[SLIDERS_DOWN].ob_width + 1, windata->frame_slider[SLIDERS_DOWN].ob_height + 1, SELECTED);

								windata -> ypos++;
								move_bookmark_work( win, windata->h_u, windata);
								redraw_arrow_slider	= 1;
							}

							graf_mkstate( &dum, &dum, &res, &dum);

						} while( res);

						if( redraw_arrow_slider)
						{
							windata->frame_slider[SLIDERS_DOWN].ob_state &= ~SELECTED;
							ObjcWindDraw( win, windata->frame_slider, SLIDERS_DOWN, 1, windata->frame_slider->ob_x, windata->frame_slider[SLIDERS_DOWN].ob_y + y, windata->frame_slider[SLIDERS_DOWN].ob_width + 1, windata->frame_slider[SLIDERS_DOWN].ob_height + 1);
						}
						break;

					case SLIDERS_MOVER:
						if( app.aes4 & AES4_XGMOUSE)
							graf_mouse( M_SAVE, 0L);

						graf_mouse( FLAT_HAND, NULL);

						while( !wind_update( BEG_MCTRL));

						res = graf_slidebox( windata->frame_slider, SLIDERS_BACK, SLIDERS_MOVER, 1);

						wind_update( END_MCTRL);

						pos = ( int32)( windata->ypos_max - h / windata->h_u) * res / 1000L;

						if ( pos < 0)
							pos = 0;

						dy = ( int16)(( pos - windata->ypos) * windata->h_u);

						windata->ypos =  pos;

						if( dy && ( old_ypos != windata->ypos))
							move_bookmark_work( win, dy, windata);

						if( app.aes4 & AES4_XGMOUSE)
							graf_mouse( M_RESTORE, 0L);

						graf_mouse( ARROW,NULL);

						break;

					case SLIDERS_BACK:
						objc_offset( windata->frame_slider, SLIDERS_MOVER, &dum, &dy);

						dum  			= ( evnt.my < dy) ? WA_UPPAGE : WA_DNPAGE;
						selected_object = ( evnt.my < dy) ? SLIDERS_UP : SLIDERS_DOWN;

						while( !wind_update( BEG_MCTRL));

						windata->frame_slider[selected_object].ob_state |= SELECTED;

						if( dum == WA_DNPAGE)
						{
							do
							{
								page = h / windata -> h_u;

								if ( windata -> ypos < windata -> ypos_max - page)
								{
									windata -> ypos = MIN( windata->ypos_max, windata->ypos) + page;
									windata -> ypos = MIN( windata -> ypos, windata -> ypos_max - page);
									dy = ( int16) (( windata->ypos - old_ypos) * windata->h_u);
									move_bookmark_work( win, dy, windata);
								}

								graf_mkstate( &dum, &dum, &res, &dum);
							} while( res);
						}
						else
						{
							do
							{
								if ( windata -> ypos > 0L)
								{
									pos = MAX( 0L, windata->ypos - h / windata->h_u);
									dy = ( int16) (( pos - windata->ypos) * windata->h_u);
									windata->ypos = pos;
									move_bookmark_work( win, dy, windata);
								}

								graf_mkstate( &dum, &dum, &res, &dum);
							} while( res);
						}

						windata->frame_slider[selected_object].ob_state &= ~SELECTED;

						ObjcWindDraw( win, windata->frame_slider, selected_object, 1, x, y, w, h);

						wind_update( END_MCTRL);

						break;

					default:
						break;
				}
			}
		}
		else
		{
			for ( i = 0; i < windata->nbr_bookmark; i++)
			{
				if( windata->root[i].valid == FALSE)
					continue;

				if ( windata->root[i].state == ON)
					if ( find_bookmark_child_on_mouse( win, windata, &windata->root[i], mouse.g_x, mouse.g_y, x, y, w, h))
						break;

				if (( mouse.g_x >= windata->root[i].arrow_position.x2 + x_space && mouse.g_x <= windata->root[i].arrow_position.x2 + x_space + windata->root[i].txt_width	&& mouse.g_y >= windata->root[i].arrow_position.y1 && mouse.g_y <= windata->root[i].arrow_position.y2))
				{
					if ( windata->selected != &windata->root[i])
					{
						Bookmark *old_selected = windata->selected;
						windata->selected = &windata->root[i];

						if ( old_selected)
							redraw_bookmark( win, windata->frame_width, old_selected, x, y, h);

						redraw_bookmark( win, windata->frame_width, windata->selected, x, y, h);
					}

					if( windata->root[i].linked_page == windata->page_to_show || windata->root[i].linked_page > windata->img.page || windata->root[i].linked_page < 0)
						break;

					graf_mouse( BUSYBEE, NULL);

					if( windata->img.image[windata->page_to_show].fd_addr != NULL)
					{
						gfree( windata->img.image[windata->page_to_show].fd_addr);
						windata->img.image[windata->page_to_show].fd_addr = NULL;
					}

		   		    if( pdf_fit_to_win)
						scale = get_scale_value( &windata->img, windata->root[i].linked_page + 1, w - ( windata->frame_width + windata->border_width + 20) , h - 20);

					zoom = windata->zoom_level;

					if( read_pdf( &windata->img, windata->root[i].linked_page + 1, scale))
					{
						scale = scale * 100.0;
						sprintf( temp, "%.0f%%", scale);
						windata->zoom_level	= ( int16)scale;

						ObjcStrCpy( win->tool.root, PDFTOOLBAR_PERCENT, temp);
						ObjcDraw( OC_TOOLBAR, win, PDFTOOLBAR_PERCENT, 1);

						sprintf( temp, "%d of %d", windata->root[i].linked_page + 1, windata->img.page);
						ObjcStrCpy( win->tool.root, PDFTOOLBAR_PAGE, temp);
						ObjcDraw( OC_TOOLBAR, win, PDFTOOLBAR_PAGE, 1);

						windata->page_to_show = windata->root[i].linked_page;

						win -> ypos_max = ( int32)( ( windata->img.image[windata->page_to_show].fd_h) >> 3);
						win -> xpos_max = ( int32)( ( windata->img.image[windata->page_to_show].fd_w + windata->frame_width + windata->border_width) >> 3);

						ObjcDraw( OC_TOOLBAR, win, PDFTOOLBAR_PREVIOUS, 1);
						ObjcDraw( OC_TOOLBAR, win, PDFTOOLBAR_NEXT, 1);

						if( zoom >= 200 && windata->zoom_level < 200)
							ObjcDraw( OC_TOOLBAR, win, PDFTOOLBAR_BIG, 1);

						if( zoom <= 25 && windata->zoom_level > 25)
							ObjcDraw( OC_TOOLBAR, win, PDFTOOLBAR_SMALL, 1);
					}

					win -> ypos 	= 0;
					win -> xpos 	= 0;

					draw_page( win, x  + windata->frame_width + windata->border_width, y, w - ( windata->frame_width + windata->border_width) , h);

					graf_mouse( ARROW, NULL);

					break;
				}
				else if( windata->root[i].state != UNKNOWN)
				{
					if (( mouse.g_x >= windata->root[i].arrow_position.x1 && mouse.g_x <= windata->root[i].arrow_position.x2 && mouse.g_y >= windata->root[i].arrow_position.y1 && mouse.g_y <= windata->root[i].arrow_position.y2))
					{
						windata->root[i].state = ( windata->root[i].state == ON) ? OFF : ON;

						draw_page( win, x, y, windata->frame_width, h);

						break;
					}
				}
			}
		}
	}

	/* a wait loop while the mouse button is pressed */
	while(( evnt.mbut == 1) || ( evnt.mbut == 2))
		graf_mkstate( &evnt.mx, &evnt.my, &evnt.mbut, &evnt.mkstate);
}

/*
static void size_popup( WINDOW *win, int obj_index)
{
	int16 x, y;
	int choice;
	char *items[] = { "200%", "150%", "100%", "50%", "25%", "-------------", "Fit to window"};

	objc_offset( win->tool.root, obj_index, &x, &y);

	choice = MenuPopUp ( items, x, y, 7, -1, smooth_thumbnail + 1 , P_LIST + P_WNDW + P_CHCK);

	if( choice < 0)
		return;

	smooth_thumbnail = choice - 1;

	strcpy( pref_dialog[PREFS_SMOOTH_METHOD].ob_spec.free_string, items[smooth_thumbnail]);
   	ObjcDraw( OC_FORM, win, PREFS_SMOOTH_METHOD, 1);
}
*/

/*==================================================================================*
 * WindPdfTool:																		*
 *		Toolbar binding.															*
 *----------------------------------------------------------------------------------*
 * input:																			*
 * 		win		-> Pointer to the target window										*
 *----------------------------------------------------------------------------------*
 * returns: 																		*
 *      --																			*
 *==================================================================================*/

void WindViewTop( WINDOW *win);
static void WindPdfTool( WINDOW *win)
{
	WINDATA	*windata = ( WINDATA *)DataSearch( win, WD_DATA);
    IMAGE 	*img 	 = &windata->img;
    int16	x, y, w, h, zoom, object = evnt.buff[4];
    double  scale = 1.0;
    char	temp[20];

	switch( object)
	{
		case PDFTOOLBAR_OPEN:
			Menu_open_image();
			break;

		case PDFTOOLBAR_INFO:
		        WindViewTop( win);
			infobox();
			break;

		case PDFTOOLBAR_SMALL:
			zoom = windata->zoom_level;

			if( windata->zoom_level <= 25)
				return;
			else if( windata->zoom_level > 200)
				windata->zoom_level = 200;
			else if( windata->zoom_level > 150)
				windata->zoom_level = 150;
			else if( windata->zoom_level > 100)
				windata->zoom_level = 100;
			else if( windata->zoom_level > 50)
				windata->zoom_level = 50;
			else if( windata->zoom_level > 25)
				windata->zoom_level = 25;

			graf_mouse( BUSYBEE, NULL);

			if( img->image[windata->page_to_show].fd_addr != NULL)
			{
				gfree( img->image[windata->page_to_show].fd_addr);
				img->image[windata->page_to_show].fd_addr = NULL;
			}

		    scale = ( scale / 100.0) * ( double)windata->zoom_level;

			if( read_pdf( img, windata->page_to_show + 1, scale))
			{
				sprintf( temp, "%d%%", windata->zoom_level);
				ObjcStrCpy( win->tool.root, PDFTOOLBAR_PERCENT, temp);
				ObjcDraw( OC_TOOLBAR, win, PDFTOOLBAR_PERCENT, 1);

				win -> ypos_max = ( int32)(  windata->img.image[windata->page_to_show].fd_h >> 3);
				win -> xpos_max = ( int32)(( windata->img.image[windata->page_to_show].fd_w + windata->frame_width + windata->border_width) >> 3);
			}

			win -> ypos	= 0;
			win -> xpos	= 0;

			graf_mouse( ARROW, NULL);

		    WindGet( win, WF_WORKXYWH, &x, &y, &w, &h);
			draw_page( win, x  + windata->frame_width + windata->border_width, y, w - ( windata->frame_width + windata->border_width) , h);


			if( zoom >= 200 && windata->zoom_level < 200)
				ObjcDraw( OC_TOOLBAR, win, PDFTOOLBAR_BIG, 1);
			break;

		case PDFTOOLBAR_BIG:
			zoom = windata->zoom_level;

			if( windata->zoom_level >= 200)
				return;
			else if( windata->zoom_level < 25)
				windata->zoom_level = 25;
			else if( windata->zoom_level < 50)
				windata->zoom_level = 50;
			else if( windata->zoom_level < 100)
				windata->zoom_level = 100;
			else if( windata->zoom_level < 150)
				windata->zoom_level = 150;
			else if( windata->zoom_level < 200)
				windata->zoom_level = 200;

			graf_mouse( BUSYBEE, NULL);

			if( img->image[windata->page_to_show].fd_addr != NULL)
			{
				gfree( img->image[windata->page_to_show].fd_addr);
				img->image[windata->page_to_show].fd_addr = NULL;
			}

		    scale = ( scale / 100.0) * ( double)windata->zoom_level;

			if( read_pdf( img, windata->page_to_show + 1, scale))
			{
				sprintf( temp, "%d%%", windata->zoom_level);
				ObjcStrCpy( win->tool.root, PDFTOOLBAR_PERCENT, temp);
				ObjcDraw( OC_TOOLBAR, win, PDFTOOLBAR_PERCENT, 1);

				win -> ypos_max = ( int32)(  windata->img.image[windata->page_to_show].fd_h >> 3);
				win -> xpos_max = ( int32)(( windata->img.image[windata->page_to_show].fd_w + windata->frame_width + windata->border_width) >> 3);
			}

			win -> ypos	= 0;
			win -> xpos	= 0;

			graf_mouse( ARROW, NULL);

		    WindGet( win, WF_WORKXYWH, &x, &y, &w, &h);
			draw_page( win, x  + windata->frame_width + windata->border_width, y, w - ( windata->frame_width + windata->border_width) , h);

			if( zoom <= 25 && windata->zoom_level > 25)
				ObjcDraw( OC_TOOLBAR, win, PDFTOOLBAR_SMALL, 1);
			break;

		case PDFTOOLBAR_NEXT:
			if( windata->page_to_show == windata->img.page - 1)
				return;

			graf_mouse( BUSYBEE, NULL);

			if( img->image[windata->page_to_show].fd_addr != NULL)
			{
				gfree( img->image[windata->page_to_show].fd_addr);
				img->image[windata->page_to_show].fd_addr = NULL;
			}

			windata->page_to_show++;

		    WindGet( win, WF_WORKXYWH, &x, &y, &w, &h);

		    if( pdf_fit_to_win)
			    scale = get_scale_value( img, windata->page_to_show + 1, w - ( windata->frame_width + windata->border_width + 20) , h - 20);

			zoom = windata->zoom_level;

			if( read_pdf( img, windata->page_to_show + 1, scale))
			{
				scale = scale * 100.0;
				sprintf( temp, "%.0f%%", scale);
				windata->zoom_level	= ( int16)scale;
				ObjcStrCpy( win->tool.root, PDFTOOLBAR_PERCENT, temp);
				ObjcDraw( OC_TOOLBAR, win, PDFTOOLBAR_PERCENT, 1);

				sprintf( temp, "%d of %d", windata->page_to_show + 1, img->page);
				ObjcStrCpy( win->tool.root, PDFTOOLBAR_PAGE, temp);
				ObjcDraw( OC_TOOLBAR, win, PDFTOOLBAR_PAGE, 1);

				win -> ypos_max = ( int32)(  windata->img.image[windata->page_to_show].fd_h >> 3);
				win -> xpos_max = ( int32)(( windata->img.image[windata->page_to_show].fd_w + windata->frame_width + windata->border_width) >> 3);

				if( zoom >= 200 && windata->zoom_level < 200)
					ObjcDraw( OC_TOOLBAR, win, PDFTOOLBAR_BIG, 1);

				if( zoom <= 25 && windata->zoom_level > 25)
					ObjcDraw( OC_TOOLBAR, win, PDFTOOLBAR_SMALL, 1);
			}

			win -> ypos	= 0;
			win -> xpos	= 0;

			graf_mouse( ARROW, NULL);

			ObjcDraw( OC_TOOLBAR, win, PDFTOOLBAR_PREVIOUS, 1);
			draw_page( win, x  + windata->frame_width + windata->border_width, y, w - ( windata->frame_width + windata->border_width) , h);

			break;

		case PDFTOOLBAR_PREVIOUS:
			if( windata->page_to_show == 0)
				return;

			graf_mouse( BUSYBEE, NULL);

			if( img->image[windata->page_to_show].fd_addr != NULL)
			{
				gfree( img->image[windata->page_to_show].fd_addr);
				img->image[windata->page_to_show].fd_addr = NULL;
			}

			windata->page_to_show--;

		    WindGet( win, WF_WORKXYWH, &x, &y, &w, &h);

   		    if( pdf_fit_to_win)
				scale = get_scale_value( img, windata->page_to_show + 1, w - ( windata->frame_width + windata->border_width + 20) , h - 20);

			zoom = windata->zoom_level;

			if( read_pdf( img, windata->page_to_show + 1, scale))
			{
				sprintf( temp, "%d of %d", windata->page_to_show + 1, img->page);
				ObjcStrCpy( win->tool.root, PDFTOOLBAR_PAGE, temp);
				ObjcDraw( OC_TOOLBAR, win, PDFTOOLBAR_PAGE, 1);

				scale = scale * 100.0;
				sprintf( temp, "%.0f%%", scale);
				windata->zoom_level	= ( int16)scale;
				ObjcStrCpy( win->tool.root, PDFTOOLBAR_PERCENT, temp);
				ObjcDraw( OC_TOOLBAR, win, PDFTOOLBAR_PERCENT, 1);

				win -> ypos_max = ( int32)(  windata->img.image[windata->page_to_show].fd_h >> 3);
				win -> xpos_max = ( int32)(( windata->img.image[windata->page_to_show].fd_w + windata->frame_width + windata->border_width) >> 3);

				if( zoom >= 200 && windata->zoom_level < 200)
					ObjcDraw( OC_TOOLBAR, win, PDFTOOLBAR_BIG, 1);

				if( zoom <= 25 && windata->zoom_level > 25)
					ObjcDraw( OC_TOOLBAR, win, PDFTOOLBAR_SMALL, 1);

			}

			win -> ypos	= 0;
			win -> xpos	= 0;

			graf_mouse( ARROW, NULL);

			ObjcDraw( OC_TOOLBAR, win, PDFTOOLBAR_NEXT, 1);
			draw_page( win, x  + windata->frame_width + windata->border_width, y, w - ( windata->frame_width + windata->border_width) , h);

			break;


		case PDFTOOLBAR_PERCENT:
		//	size_popup( win, object);
			break;

		default:
			break;
	}

	ObjcChange( OC_TOOLBAR, win, object, NORMAL, 1);
}


static void WindPdfTop( WINDOW *win)
{
	menu = get_tree( MENU_BAR);

	menu_ienable( menu, MENU_BAR_SHOW_FULLSCREEN, 1);
	menu_ienable( menu, MENU_BAR_INFORMATION, 1);
	menu_ienable( menu, MENU_BAR_SAVE, 0);
	menu_ienable( menu, MENU_BAR_DELETE, 0);
	WindSet( win, WF_TOP, win->handle, 0, 0, 0);
	wglb.appfront = wglb.front = win;
}


static void WindPdfKeyb( WINDOW *win)
{
	switch ( evnt.keybd >> 8)
	{
		case SC_UPARW:
			if( !( evnt.mkstate & ( K_LSHIFT|K_RSHIFT)))
				snd_arrw( win, WA_UPLINE);
			else
		case SC_PGUP:
				snd_arrw( win, WA_UPPAGE);
			break;

		case SC_DWARW:
			if( !( evnt.mkstate & ( K_LSHIFT|K_RSHIFT)))
				snd_arrw( win, WA_DNLINE);
			else
		case SC_PGDN:  /* page down */
				snd_arrw( win, WA_DNPAGE);
			break;

		case SC_LFARW:
			if( !( evnt.mkstate & ( K_LSHIFT|K_RSHIFT)))
				snd_arrw( win, WA_LFLINE);
			else
				snd_arrw( win, WA_LFPAGE);
			break;

		case SC_RTARW:
			if( !( evnt.mkstate & ( K_LSHIFT|K_RSHIFT)))
				snd_arrw( win, WA_RTLINE);
			else
				snd_arrw( win, WA_RTPAGE);
			break;

		case SC_MINUS:
//			WindViewZoom( win, 2);
//			ObjcDraw( OC_TOOLBAR, win, VIEWTOOLBAR_SMALL, 1);
			break;

		case SC_PLUS:
//			WindViewZoom( win, 1);
//			ObjcDraw( OC_TOOLBAR, win, VIEWTOOLBAR_BIG, 1);
			break;

		default:
			break;
	}
}


/*==================================================================================*
 * draw_bookmark:																	*
 *		draw one bookmark in the WINDOW *win.										*
 *----------------------------------------------------------------------------------*
 * return:																			*
 *		--																			*
 *==================================================================================*/

static void draw_bookmark( WINDOW *win, Bookmark *selected, Bookmark *entry, int16 x, int16 y, int16 h)
{
	int16	xy[8], xtext, ytext;

	if( ( entry->arrow_position.y1 + y) < y || ( entry->arrow_position.y1 + y) >= y + h)
		return;

	/* DRAW ARROW IS NEEDED */
	if ( entry->state != UNKNOWN)
	{
		if ( entry->state == ON)
		{
			xy[0] =	entry->arrow_position.x1 + x;
			xy[1] = entry->arrow_position.y1 + 3 + y;
			xy[2] = xy[0] + 10;
			xy[3] = xy[1];
			xy[4] = xy[0] + 5;
			xy[5] = xy[1] + 5;
		}
		else
		{
			xy[0] =	entry->arrow_position.x1 + x + 4;
			xy[1] = entry->arrow_position.y1 + y;
			xy[2] = xy[0];
			xy[3] = xy[1] + 10;
			xy[4] = xy[0] + 5;
			xy[5] = xy[1] + 5;
		}

		vsf_color( win->graf.handle, LBLACK);
		v_fillarea( win->graf.handle, 3, xy);
	}

	/* DRAW TEXT */
	xtext = entry->arrow_position.x2 + x + x_space;
	ytext = entry->arrow_position.y1 + y + 2;

	if ( entry == selected)
	{
		xy[0] = xtext - 2;
		xy[2] = xy[0] + entry->txt_width + 4;
		xy[1] = ytext - 1;
		xy[3] = xy[1] + hcell + 2;

		vsf_color( win->graf.handle, BLACK);
		v_bar( win->graf.handle, xy);

		draw_text( win->graf.handle, xtext, ytext, WHITE, entry->name);
	}
	else
		draw_text( win->graf.handle, xtext, ytext, BLACK, entry->name);
}



/*==================================================================================*
 * draw_bookmark_child:																*
 *		draw every bookmark's childs in the WINDOW *win.							*
 *----------------------------------------------------------------------------------*
 * return:																			*
 *		number of lines drawn														*
 *==================================================================================*/

static int16 draw_bookmark_child( WINDOW *win, Bookmark *selected, Bookmark *entry, int16 x, int16 y, int16 xw, int16 yw, int16 hw)
{
	int16 i, lines = 0;

	if( entry->child == NULL)
		 return lines;

	for ( i = 0; i < entry->nbr_child ; i++)
	{
		if( entry->child[i].valid == FALSE)
			continue;

		entry->child[i].arrow_position.x1 = x + 3;
		entry->child[i].arrow_position.y1 = y + 3;
		entry->child[i].arrow_position.x2 = x + 14;
		entry->child[i].arrow_position.y2 = y + 14;

		draw_bookmark( win, selected, &entry->child[i], xw, yw, hw);
		lines++;
		y += 18;

		if( entry->child[i].state != ON)
			continue;

		dum = draw_bookmark_child( win, selected, &entry->child[i], x + 15, y, xw, yw, hw);

		y += ( dum * 18);

		lines += dum;
	}

	return lines;
}


void calc_slider( WINDATA *windata, OBJECT *slider_root)
{
	int16 max_mover_size 	= slider_root[SLIDERS_BACK].ob_height;
	int16 full_win_size  	= windata->ypos_max * windata->h_u;
	int16 win_h 			= slider_root->ob_height + 3;

	if ( win_h >= full_win_size)
	{
		windata -> ypos = 0;

		slider_root[SLIDERS_MOVER].ob_y 		= 0;
		slider_root[SLIDERS_MOVER].ob_height  	= max_mover_size;

	}
	else
	{
		int16 ligne_reste;
		float mover_position	= 0L;
		float position_step		= 0L;
		float mover_size 		= MAX( ( float)max_mover_size * ( ( float)win_h / ( float)full_win_size), 10L);

		if( windata->ypos)
		{
			ligne_reste		= windata->ypos_max - ( win_h / windata->h_u);
			position_step	= (( float)max_mover_size - mover_size) / ( float)ligne_reste;
			mover_position 	= position_step * ( float)windata->ypos;

			while( (( int16)mover_size + ( int16)mover_position > max_mover_size) && windata->ypos > 0)
			{
				windata->ypos--;
				mover_position 	-= position_step;
			}
		}

		slider_root[SLIDERS_MOVER].ob_height  	= ( int16)mover_size;
		slider_root[SLIDERS_MOVER].ob_y 		= ( int16)mover_position;
	}
}


static void WindPdfRedraw( WINDOW *win)
{
	int16	lines = 0, y, i, xw, yw, ww, hw, tmp, xy[8], pxy[4], page;
	WINDATA	*windata = ( WINDATA *)DataSearch( win, WD_DATA);
    MFDB 	*picture, screen = {0};

	page = windata->page_to_show;

	WindGet( win, WF_WORKXYWH, &xw, &yw, &ww, &hw);

	/* Multipages ? */
	if( windata->frame_width) /* browser active */
	{
		pxy[0] = xw;
		pxy[1] = yw;
		pxy[2] = pxy[0] + windata->frame_width;
		pxy[3] = pxy[1] + hw - 1;

		/* clean the first frame */
		vsf_color( win->graf.handle, WHITE);
		v_bar( win->graf.handle, pxy);


		/* Draw the 1st frame ( navigation folder) */
		y = 0 - ( ( int16)windata -> ypos * windata -> h_u);

		for ( i = 0 ; i < windata->nbr_bookmark; i++)
		{
			if( windata->root[i].valid == FALSE)
				continue;

			windata->root[i].arrow_position.x1 = 3;
			windata->root[i].arrow_position.y1 = y + 3;
			windata->root[i].arrow_position.x2 = 14;
			windata->root[i].arrow_position.y2 = y + 14;

			draw_bookmark( win, windata->selected, &windata->root[i], xw, yw, hw);

			lines++;

			y += windata -> h_u;

			if( windata->root[i].state != ON)
				continue;

			dum = draw_bookmark_child( win, windata->selected, &windata->root[i], 15, y, xw, yw, hw);

			y += ( dum * windata -> h_u);

			lines += dum;
		}

		windata -> ypos_max = lines;

		/* Draw the frame border */
		pxy[0] = pxy[2];
		vsl_color( win->graf.handle, BLACK);
		v_pline( win->graf.handle, 2, pxy);

		pxy[0]++;
		pxy[2] = pxy[0];
		vsl_color( win->graf.handle, WHITE);
		v_pline( win->graf.handle, 2, pxy);

		pxy[0]++;
		pxy[2] = pxy[0];
		vsl_color( win->graf.handle, LWHITE);
		v_pline( win->graf.handle, 2, pxy);

		pxy[0]++;
		pxy[2] = pxy[0];
		v_pline( win->graf.handle, 2, pxy);

		pxy[0]++;
		pxy[2] = pxy[0];
		vsl_color( win->graf.handle, BLACK);
		v_pline( win->graf.handle, 2, pxy);

		pxy[0] = xw;
		pxy[1] = yw;
		pxy[2] = pxy[0] + windata->frame_width - 1;
		pxy[3] = yw;
		v_pline( win->graf.handle, 2, pxy);

		if( ( windata -> ypos_max * windata -> h_u) >= hw)
		{
			windata->frame_slider->ob_x 					= xw + windata->frame_width - 15;
			windata->frame_slider->ob_y 					= yw + 2;
			windata->frame_slider->ob_height  				= hw - 3;
			windata->frame_slider[SLIDERS_BACK].ob_height 	= hw - 31;
			windata->frame_slider[SLIDERS_UP].ob_y 			= windata->frame_slider->ob_height - 31;
			windata->frame_slider[SLIDERS_DOWN].ob_y 		= windata->frame_slider->ob_height - 15;

			calc_slider( windata, windata->frame_slider);
			objc_draw( windata->frame_slider, SLIDERS_BOX, 2, clip.g_x, clip.g_y, clip.g_w, clip.g_h);
		}

		xw +=  ( windata->frame_width + border_size);
		ww -=  ( windata->frame_width + border_size);
	}

	pxy[0] = xw;
	pxy[1] = yw;
	pxy[2] = pxy[0] + ww - 1;
	pxy[3] = yw;

	vsl_color( win->graf.handle, BLACK);
	v_pline( win->graf.handle, 2, pxy);

	yw++;

	/* if the picture or the zoomed picture don't exist, we draw a gray rectangle and we exit the function */
	if( windata->zoom_picture.fd_addr)
		picture = &windata->zoom_picture;
	else if( windata->img.image[page].fd_addr)
		picture = &windata->img.image[page];
	else
	{
		pxy[0] = xw;
		pxy[1] = yw;
		pxy[2] = pxy[0] + ww - 1;
		pxy[3] = pxy[1] + hw - 1;

		vsf_color( win->graf.handle, LWHITE);
		v_bar( win->graf.handle, pxy);
		return;
	}

	xy[0] = ( int16)win->xpos * win->w_u;
	xy[4] = xw;

	if( picture->fd_w < ww)
		xy[4] += ( ( ww - picture->fd_w) >> 1);

	tmp = MIN( ww, ( ( picture->fd_w) - xy[0]));

	xy[2] = xy[0] + tmp - 1;
	xy[6] = xy[4] + tmp - 1;


	xy[1] = ( int16)win->ypos * win->h_u;
	xy[5] = yw;

	if( picture->fd_h < hw)
		xy[5] += ( ( hw - picture->fd_h) >> 1);

	tmp = MIN( hw, ( ( picture->fd_h) - xy[1]));

	xy[3] = xy[1] + tmp - 1;
	xy[7] = xy[5] + tmp - 1;

	if(( xy[6] <= ( xw + ww)) || ( xy[7] <= ( yw + hw)))
	{
		pxy[0] = xw;
		pxy[1] = yw;
		pxy[2] = pxy[0] + ww - 1;
		pxy[3] = pxy[1] + hw - 1;

		vsf_color( win->graf.handle, LWHITE);
		v_bar( win->graf.handle, pxy);
	}

	if ( picture->fd_nplanes == 1)
	{
		int16	color[2] = { BLACK, WHITE};

		vrt_cpyfm( win->graf.handle, MD_REPLACE, xy, picture, &screen, color);
	}
	else
		vro_cpyfm( win->graf.handle, S_ONLY, xy, picture, &screen);

	WindSlider ( win, VSLIDER|HSLIDER);
}



static void WindPdfClose( WINDOW *win)
{
	WINDATA	*windata = ( WINDATA *)DataSearch( win, WD_DATA);
    IMAGE 	*img 	 = &windata->img;

	delete_txt_data( img);
	delete_mfdb( img->image, img->page);

	if( windata->icon.fd_addr != NULL)
		gfree( windata->icon.fd_addr);

//	if( windata->zoom_picture.fd_addr != NULL)
//		gfree( windata->zoom_picture.fd_addr);

	pdf_quit( img);

	if( windata->root)
	{
		int i;

		for ( i = 0; i < windata->nbr_bookmark; i++)
		{
			if ( windata->root[i].nbr_child)
				delete_bookmark_child( &windata->root[i]);
		}

		gfree( windata->root);
	}

	if( windata->frame_slider)
		ObjcFree( windata->frame_slider);

	gfree( windata);

	DataDelete( win, WD_DATA);
	WindDelete( win);

	if( wglb.first)
	{
		snd_msg( wglb.first, WM_TOPPED, 0, 0, 0, 0);
	}
	else
	{
		menu_ienable( menu, MENU_BAR_SHOW_FULLSCREEN, 0);
		menu_ienable( menu, MENU_BAR_SAVE, 0);
		menu_ienable( menu, MENU_BAR_INFORMATION, 0);
		menu_ienable( menu, MENU_BAR_CLOSE, 0);
	}
}




WINDOW *WindPdf( char *filename)
{
	WINDOW 		*winview = NULL;
	WINDATA		*windata;
	IMAGE 		*img;
	double		scale = 1.0;
	int16		w, h;
	char		temp[10];

	if ( ( windata = ( WINDATA*) gmalloc( sizeof( WINDATA))) == NULL)
	{
		graf_mouse( ARROW, NULL);
		errshow( "", ENOMEM);
		return NULL;
	}

	windata->icon.fd_addr 			= NULL;
	windata->zoom_picture.fd_addr	= NULL;
	windata->zoom_level				= 100;
	windata->page_to_show			= 0;
	windata->pause			 		= FALSE;
	windata->frame_width 			= 0;
	windata->border_width 			= 0;
	windata->root					= NULL;
	windata->selected				= NULL;
	windata->frame_slider			= NULL;
	windata->nbr_bookmark 			= 0;

	img = &windata->img;

	img->_priv_ptr 		= NULL;
	img->view_mode 		= full_size;
	img->progress_bar 	= show_read_progress_bar;

	strcpy( windata->name, filename);

	if ( ( winview	= WindCreate( WAT_NOINFO, app.x, app.y, app.w, app.h)) == NULL)
	{
		gfree( windata);
		errshow( "", ALERT_WINDOW);
		graf_mouse( ARROW, NULL);
		return NULL;
	}

	WindSetPtr( winview, WF_TOOLBAR,  get_tree( PDFTOOLBAR), WindPdfTool);

	WindCalc( WC_WORK, winview, 0, 0, app.w, app.h, &dum, &dum, &w, &h);

	if( !pdf_load( filename, img, w - 225, h - 20))
	{
		WindDelete( winview);
		gfree( windata);
		// errshow( "", IMG_NO_VALID);
		graf_mouse( ARROW, NULL);
		return NULL;
	}

	strcpy( windata->title, get_pdf_title());
	sprintf( windata->info, "zView - [%s]", fullpathname_to_filename( windata->name));

	if( img->page > 1)
	{
		pdf_build_bookmark( windata, winview);

		if( windata->nbr_bookmark)
		{
			OBJECT *frame;
			rsrc_gaddr( 0, SLIDERS, &frame);
			windata->frame_slider = ObjcDup( frame, NULL);
			EvntAttach( winview, WM_XBUTTON, WindPdfMouse);
			EvntAttach( winview, WM_SIZED, WindPdfSize);
			windata -> frame_width	= 200;
			windata -> border_width	= 5;
		}
	}

	winview -> h_min 	= 80;
	winview -> w_min 	= MAX( 80, windata->frame_width + windata->border_width + 80);
	winview -> h_u   	= 8;
	winview -> w_u   	= 8;
	winview -> ypos_max = ( int32)( ( img->image[0].fd_h) >> 3);
	winview -> xpos_max = ( int32)( ( img->image[0].fd_w + windata->frame_width + windata->border_width) >> 3);
	windata -> h_u   	= 18;
	windata -> ypos   	= 0;
	windata -> ypos_max = 1;

    DataAttach( winview, WD_DATA, 	  windata);
	EvntAttach( winview, WM_REDRAW,	  WindPdfRedraw);
	EvntAttach( winview, WM_DESTROY,  WindPdfClose);
	EvntAttach( winview, WM_XKEYBD,	  WindPdfKeyb);
	EvntAttach( winview, WM_TOPPED,   WindPdfTop);
	EvntAttach( winview, WM_ARROWED,  Win_Arrow);
	EvntAttach( winview, WM_UPPAGE,   Win_UpPage);
	EvntAttach( winview, WM_DNPAGE,   Win_DownPage);
	EvntAttach( winview, WM_LFPAGE,   Win_LeftPage);
	EvntAttach( winview, WM_RTPAGE,   Win_RightPage);
	EvntAttach( winview, WM_UPLINE,   Win_UpLine);
	EvntAttach( winview, WM_DNLINE,   Win_DownLine);
	EvntAttach( winview, WM_RTLINE,   Win_RightLine);
	EvntAttach( winview, WM_LFLINE,   Win_LeftLine);
	EvntAttach( winview, WM_VSLID ,   Win_VSlide);
	EvntAttach( winview, WM_HSLID ,   Win_HSlide);


	WindSetStr( winview, WF_ICONDRAW, WindViewIcon);
	WindSetStr( winview, WF_NAME,	  windata->info);

	RsrcUserDraw ( OC_TOOLBAR, winview, PDFTOOLBAR_BIG, draw_icon_greater, windata);
	RsrcUserDraw ( OC_TOOLBAR, winview, PDFTOOLBAR_SMALL, draw_icon_smaller, windata);
	RsrcUserDraw ( OC_TOOLBAR, winview, PDFTOOLBAR_NEXT, draw_icon_forward, windata);
	RsrcUserDraw ( OC_TOOLBAR, winview, PDFTOOLBAR_PREVIOUS, draw_icon_back, windata);
	RsrcUserDraw ( OC_TOOLBAR, winview, PDFTOOLBAR_OPEN, draw_icon_open, NULL);
	RsrcUserDraw ( OC_TOOLBAR, winview, PDFTOOLBAR_INFO, draw_icon_info, NULL);
//	RsrcUserDraw ( OC_TOOLBAR, winview, PDFTOOLBAR_SAVE, draw_icon_save, NULL);
	RsrcUserDraw ( OC_TOOLBAR, winview, PDFTOOLBAR_PRINT, draw_icon_printer, NULL);
	RsrcUserDraw ( OC_TOOLBAR, winview, PDFTOOLBAR_FIND, draw_icon_find, NULL);

	sprintf( temp, "1 of %d", img->page);
	ObjcStrCpy( winview->tool.root, PDFTOOLBAR_PAGE, temp);

    if( pdf_fit_to_win)
		scale = get_scale_value( img, windata->page_to_show + 1, w - ( windata->frame_width + windata->border_width + 20) , h - 20);

	sprintf( temp, "%.0f%%", scale * 100);

	ObjcStrCpy( winview->tool.root, PDFTOOLBAR_PERCENT, temp);


	if ( !( WindOpen( winview, app.x, app.y, app.w, app.h)))
	{
		WindPdfClose( winview);
		errshow( "", ALERT_WINDOW);
		graf_mouse( ARROW, NULL);
		return NULL;
	}

	menu = get_tree( MENU_BAR);

	menu_ienable( menu, MENU_BAR_INFORMATION, 1);
	menu_ienable( menu, MENU_BAR_SAVE, 0);
	menu_ienable( menu, MENU_BAR_SHOW_FULLSCREEN, 1);
	menu_ienable( menu, MENU_BAR_CLOSE, 1);

	graf_mouse( ARROW, NULL);

	return winview;
}
