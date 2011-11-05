#include "general.h"
#include "prefs.h"
#include "mfdb.h"
#include "txt_data.h"
#include "pic_load.h"
#include "winimg.h"
#include "winpdf.h"
#include "full_scr.h"
#include "menu.h"
#include "infobox.h"
#include "pdf_load.h"
#include "pdf/pdflib.h"
#include "pic_resize.h"
#include "wintimer.h"
#include "ztext.h"
#include "custom_font.h"
#include "catalog/catalog.h"
#include "catalog/catalog_other_event.h"
#include "catalog/catalog_icons.h"
#include "catalog/catalog_slider.h"


static int16 dum;

extern void save_dialog( const char *fullfilename);

/* Prototype */
WINDOW *WindView( char *filename);

/*==================================================================================*
 * void WindViewIcon:																*
 *		Function to handle iconfied windows drawing... If the icon doesn't exist,	*
 *		It is created.																*
 *----------------------------------------------------------------------------------*
 * input:																			*
 *		win -> the window to handle.												*
 *----------------------------------------------------------------------------------*
 * returns: 																		*
 *      --																			*
 *==================================================================================*/

void WindViewIcon( WINDOW *win)
{
	int16 	posx, posy, x, y, w, h, xy[8];
	WINDATA	*windata = ( WINDATA *)DataSearch( win, WD_DATA);
	IMAGE 	*wview	 = &windata->img;
	MFDB 	*out = NULL, screen	 = {0};

	WindGet( win, WF_WORKXYWH, &x, &y, &w, &h);

	if( wview->image[windata->page_to_show].fd_w > w || wview->image[windata->page_to_show].fd_h > h)
	{
		if( windata->icon.fd_addr == NULL)
		{
			windata->icon.fd_w = w;
			windata->icon.fd_h = h;

			if( pic_resize( &wview->image[windata->page_to_show], &windata->icon) == 0)
			{	
				draw_window_iconified( win);
				return;
			}
		}
	}

	xy[0] = x;
	xy[1] = y;
	xy[2] = xy[0] + w - 1;
	xy[3] = xy[1] + h - 1;

	vsf_color( win->graf.handle, BLACK);
	v_bar( win->graf.handle, xy);

	if( windata->icon.fd_addr)
		out = &windata->icon;	
	else
		out = &wview->image[windata->page_to_show];

	posx    = MAX( x + (( w - out->fd_w) >> 1), x);
	posy 	= MAX( y + (( h - out->fd_h) >> 1), y);

	if (posx == x)
		xy[2] = w - 1;
	else
		xy[2] = out->fd_w - 1;

	if (posy == y)
		xy[3] = h - 1;
	else
		xy[3] = out->fd_h - 1;

	xy[0] = 0;
	xy[1] = 0;
	xy[4] = posx;
	xy[5] = posy;
	xy[6] = posx + xy[2];
	xy[7] = posy + xy[3];

	/* draw the image */
	if ( wview->image[0].fd_nplanes == 1)
	{
		int16 color[2] = { BLACK, WHITE};

		vrt_cpyfm( win->graf.handle, MD_REPLACE, xy, out, &screen, color);
	}
	else
		vro_cpyfm( win->graf.handle, S_ONLY, xy, out, &screen);
}



static void WindViewZoom( WINDOW *win) 
{ 
	WINDATA	*windata = ( WINDATA *)DataSearch( win, WD_DATA);
	MFDB    *picture = &windata->zoom_picture;
    IMAGE 	*img	 = &windata->img;
    uint16	width = 0, height = 0;	

	if( ( img->page > 1) && ( img->delay[0] > 0))
		return;

	if( windata->zoom_picture.fd_addr != NULL)		
	{		
		gfree( windata->zoom_picture.fd_addr);
		windata->zoom_picture.fd_addr = NULL;
	}	
		
	switch ( windata->zoom_level)
	{
		case 200:
			width  = img->image[windata->page_to_show].fd_w << 1;
			height = img->image[windata->page_to_show].fd_h << 1;		
			break;
			
		case 150:
			width  = (uint16)MAX(( ( double)img->image[windata->page_to_show].fd_w * 1.5 + 0.5), 1.0);
			height = (uint16)MAX(( ( double)img->image[windata->page_to_show].fd_h * 1.5 + 0.5), 1.0);		
			break;				

		case 100:
			break;				
						
		case 50:
			width  = (uint16)MAX(( ( double)img->image[windata->page_to_show].fd_w * 0.5 + 0.5), 1.0);
			height = (uint16)MAX(( ( double)img->image[windata->page_to_show].fd_h * 0.5 + 0.5), 1.0);		
			break;				
			
		case 25:
			width  = (uint16)MAX(( ( double)img->image[windata->page_to_show].fd_w * 0.25 + 0.5), 1.0);
			height = (uint16)MAX(( ( double)img->image[windata->page_to_show].fd_h * 0.25 + 0.5), 1.0);		
			break;						
		
		default:
			width  = (uint16)MAX(( (( double)img->image[windata->page_to_show].fd_w * ( double)windata->zoom_level) / 100.0 + 0.5), 1.0);
			height = (uint16)MAX(( (( double)img->image[windata->page_to_show].fd_h * ( double)windata->zoom_level) / 100.0 + 0.5), 1.0);		
			break;
	}						

	
	if(( windata->zoom_level == 100) || ( width < 16 && height < 16))
	{
		EvntRedraw( win);
		return;	
	}
		
	windata->zoom_picture.fd_w = width;
	windata->zoom_picture.fd_h = height;		
			
			
	if( !pic_resize( &img->image[windata->page_to_show], &windata->zoom_picture))
	{
		errshow( "", ENOMEM);
		windata->zoom_level = 100;
	}

	win -> ypos_max = ( int32)( ( picture->fd_h + 1) >> 3);
	win -> xpos_max = ( int32)( ( picture->fd_w) >> 3);
	win -> ypos 	= 0;
	win -> xpos 	= 0;

	EvntRedraw( win);
}


/*==================================================================================*
 * WindViewTool:																	*
 *		Toolbar binding.															*
 *----------------------------------------------------------------------------------*
 * input:																			*
 * 		win		-> Pointer to the target window										*
 *----------------------------------------------------------------------------------*
 * returns: 																		*
 *      --																			*
 *==================================================================================*/

static void WindViewTool( WINDOW *win)
{
	WINDATA	*windata = ( WINDATA *)DataSearch( win, WD_DATA);	
	int16 zoom;
	
	switch( evnt.buff[4])
	{
		case VIEWTOOLBAR_SMALL:
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
		
			WindViewZoom( win); 

			if( zoom >= 200 && windata->zoom_level < 200)
				ObjcDraw( OC_TOOLBAR, win, VIEWTOOLBAR_BIG, 1); 	 
			break;

		case VIEWTOOLBAR_BIG:
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
																										
			WindViewZoom( win);

			if( zoom <= 25 && windata->zoom_level > 25)
				ObjcDraw( OC_TOOLBAR, win, VIEWTOOLBAR_SMALL, 1); 				
			break;

		case VIEWTOOLBAR_OPEN:
			Menu_open_image();
			break;	

		case VIEWTOOLBAR_FULLSCREEN:
			show_fullscreen( windata);
			break;				

		case VIEWTOOLBAR_SAVE:
			save_dialog( windata->name);
			break;							

		case VIEWTOOLBAR_INFO:
			infobox();
			break;				
		
		default:
			break;
	}

	ObjcChange( OC_TOOLBAR, win, evnt.buff[4], NORMAL, 1); 
}



static void WindViewTop( WINDOW *win) 
{
	OBJECT *menu = get_tree( MENU_BAR);

	menu_ienable( menu, MENU_BAR_SHOW_FULLSCREEN, 1); 
	menu_ienable( menu, MENU_BAR_INFORMATION, 1); 
	menu_ienable( menu, MENU_BAR_SAVE, 1);
	menu_ienable( menu, MENU_BAR_DELETE, 0);
	WindSet( win, WF_TOP, win->handle, 0, 0, 0);
	wglb.appfront = wglb.front = win;	
}


static void WindViewKeyb( WINDOW *win) 
{ 
	WINDATA	*windata = ( WINDATA *)DataSearch( win, WD_DATA);
	int16 zoom;

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
		
			WindViewZoom( win); 
			ObjcDraw( OC_TOOLBAR, win, VIEWTOOLBAR_SMALL, 1); 

			if( zoom >= 200 && windata->zoom_level < 200)
				ObjcDraw( OC_TOOLBAR, win, VIEWTOOLBAR_BIG, 1); 	 
			
			break;

		case SC_PLUS:
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
																										
			WindViewZoom( win);
			ObjcDraw( OC_TOOLBAR, win, VIEWTOOLBAR_BIG, 1); 

			if( zoom <= 25 && windata->zoom_level > 25)
				ObjcDraw( OC_TOOLBAR, win, VIEWTOOLBAR_SMALL, 1); 						
			
			break;

		case SC_SPACE:
			if( win->status & WS_ICONIFY)
				return;
		
			if( windata->pause == FALSE) windata->pause = TRUE;
			else windata->pause = FALSE;
			break;

		default:
			break;					
	}
}


static void WindViewRedraw( WINDOW *win)
{
	int16	xw, yw, ww, hw, tmp, xy[8], pxy[4], page;
	WINDATA	*windata = ( WINDATA *)DataSearch( win, WD_DATA);
    MFDB 	*picture, screen = {0};

	page = windata->page_to_show;

	WindGet( win, WF_WORKXYWH, &xw, &yw, &ww, &hw);	

	pxy[0] = xw;
	pxy[1] = yw;
	pxy[2] = pxy[0] + ww - 1;
	pxy[3] = yw;

	vsl_color( win->graf.handle, BLACK);
	v_pline( win->graf.handle, 2, pxy);

	yw++;

	/* if the picture or the zoomed picture don't exist, we draw a gray rectangle and we exit the function*/
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
	
	tmp = MIN( ww, ( picture->fd_w - xy[0]));
	
	xy[2] = xy[0] + tmp - 1;
	xy[6] = xy[4] + tmp - 1;

	
	xy[1] = ( int16)win->ypos * win->h_u;
	xy[5] = yw;

	tmp = MIN( hw, ( picture->fd_h - xy[1]));
			
	xy[3] = xy[1] + tmp - 1;
	xy[7] = xy[5] + tmp - 1;

	if(( xy[6] < ( xw + ww)) || ( xy[7] < ( yw + hw)))
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


static void WindViewAnim( WINDOW *win)
{
	int16 x, y, w, h, xy[8], tmp;
	GRECT r1, rect;
	WINDATA	*windata = ( WINDATA *)DataSearch( win, WD_DATA);
	uint16 page_to_show;
	clock_t current_t = clock();
	clock_t relative_t;
	MFDB 	*picture, screen = {0};

	if( ( windata->pause == TRUE) || ( full_screen == TRUE))
		return;

	page_to_show = windata->page_to_show + 1;

	if( page_to_show >= windata->img.page - 1)
		page_to_show = 0;
	
	relative_t = current_t - windata->chrono_value;
	
	if( relative_t < windata->img.delay[page_to_show])
		return;

	windata->chrono_value = current_t;
	
	windata->page_to_show = page_to_show;	

	WindGet ( win, WF_WORKXYWH, &x, &y, &w, &h);
	
	y++;
	h--;
		
	picture = &windata->img.image[page_to_show];

	xy[0] = ( int16)win->xpos * win->w_u;
	xy[4] = x;
	
	tmp = MIN( w, ( picture->fd_w - xy[0]));
	
	xy[2] = xy[0] + tmp - 1;
	xy[6] = xy[4] + tmp - 1;
	
	xy[1] = ( int16)win->ypos * win->h_u;
	xy[5] = y;

	tmp = MIN( h, ( picture->fd_h - xy[1]));
			
	xy[3] = xy[1] + tmp - 1;
	xy[7] = xy[5] + tmp - 1;

	while( !wind_update(BEG_UPDATE));
	graf_mouse( M_OFF, 0L);		
	rc_set( &rect, x, y, w, h);
	wind_get( win -> handle, WF_FIRSTXYWH, &r1.g_x, &r1.g_y, &r1.g_w, &r1.g_h);

	while (r1.g_w && r1.g_h) 
	{
		if( rc_intersect( &rect, &r1)) 
		{
			rc_clip_on( win->graf.handle, &r1);
		
			if ( picture->fd_nplanes == 1)
			{
				int16	color[2] = { BLACK, WHITE};

				vrt_cpyfm( win->graf.handle, MD_REPLACE, xy, picture, &screen, color);
			}
			else
				vro_cpyfm( win->graf.handle, S_ONLY, xy, picture, &screen);

			rc_clip_off( win->graf.handle);
		}

		wind_get( win -> handle, WF_NEXTXYWH, &r1.g_x, &r1.g_y, &r1.g_w, &r1.g_h);
	}

	graf_mouse( M_ON, 0L);
	wind_update(END_UPDATE);
	
//	zdebug("frame = %d, delay = %d ms", page_to_show, windata->img.delay[page_to_show]);
}


static void WindViewClose( WINDOW *win)
{
	WINDATA	*windata = ( WINDATA *)DataSearch( win, WD_DATA);
    IMAGE 	*img 	 = &windata->img;

	if( ( img->page > 1) && ( img->delay[0] > 0))
		TimerDelete( win);
 
	delete_txt_data( img);	
	delete_mfdb( img->image, img->page);

	if( windata->icon.fd_addr != NULL)
		gfree( windata->icon.fd_addr);

	if( windata->zoom_picture.fd_addr != NULL)
		gfree( windata->zoom_picture.fd_addr);

	gfree( windata);

	DataDelete( win, WD_DATA);	
	WindDelete( win);

	if( wglb.first)
	{
		snd_msg( wglb.first, WM_TOPPED, 0, 0, 0, 0);
	}
	else
	{
		menu_ienable( get_tree( MENU_BAR), MENU_BAR_SHOW_FULLSCREEN, 0);
		menu_ienable( get_tree( MENU_BAR), MENU_BAR_SAVE, 0);
		menu_ienable( get_tree( MENU_BAR), MENU_BAR_INFORMATION, 0);
	}		
}


static void WindViewIconify( WINDOW *win) 
{
	WINDATA	*windata = ( WINDATA *)DataSearch( win, WD_DATA);
//	zdebug( "iconify");
	windata->pause   = TRUE;
}


static void WindViewUniconify( WINDOW *win) 
{
	WINDATA	*windata = ( WINDATA *)DataSearch( win, WD_DATA);
//	zdebug( "uniconify");	
	windata->pause = FALSE;
}


/*==================================================================================*
 * void Win_VSlide:																	*
 *		this function handle the WM_VSLID event for the main frame in the			* 
 *		catalog ( the entries).														*
 *----------------------------------------------------------------------------------*
 * input:																			*
 * 		win			-> 	The target window.											*
 *----------------------------------------------------------------------------------*
 * returns: 																		*	
 *		--																			*
 *==================================================================================*/

void Win_VSlide( WINDOW *win) 
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
		WINDATA	*windata = ( WINDATA *)DataSearch( win, WD_DATA);
		move_main_work( win, x, y, w, h, 0, dy, windata->frame_width, windata->border_width);
	}
}


/*==================================================================================*
 * void Win_HSlide:																	*
 *		this function handle the WM_HSLID event for the main frame in the			* 
 *		catalog ( the entries).														*
 *----------------------------------------------------------------------------------*
 * input:																			*
 * 		win			-> 	The target window.											*
 *----------------------------------------------------------------------------------*
 * returns: 																		*	
 *		--																			*
 *==================================================================================*/

void Win_HSlide( WINDOW *win) 
{
	int32 	pos;
	int16	x, y, w, h, dx;
	int16 	old_xpos = win->xpos;

	WindGet( win, WF_WORKXYWH, &x, &y, &w, &h);

	pos = ( int32)( win->xpos_max - w / win->w_u) * ( int32)evnt.buff[4] / 1000L;

	if ( pos < 0) 
		pos = 0;

	dx = ( int16)(( pos - win->xpos) * win->w_u);
	win->xpos =  ( int16) pos;

	if( dx && ( old_xpos != win->xpos)) 
	{
		WINDATA	*windata = ( WINDATA *)DataSearch( win, WD_DATA);
		move_main_work( win, x, y, w, h, dx, 0, windata->frame_width, windata->border_width);
	}
}



/*==================================================================================*
 * void Win_DownPage:																*
 *		this function handle the WM_DNPAGE event for the main frame in the			* 
 *		window ( the picture's frame).												* 
 *----------------------------------------------------------------------------------*
 * input:																			*
 * 		win			-> 	The target window.											*
 *----------------------------------------------------------------------------------*
 * returns: 																		*	
 *		--																			*
 *==================================================================================*/

void Win_DownPage( WINDOW *win) 
{
	int16	page, x, y, w, h, dy;
	int32	old_pos = win -> ypos;

	WindGet( win, WF_WORKXYWH, &x, &y, &w, &h);

	page = h / win -> h_u;

	if ( win -> ypos < win -> ypos_max - page) 
	{
		WINDATA	*windata = ( WINDATA *)DataSearch( win, WD_DATA);	
		win -> ypos = MIN( win->ypos_max, win->ypos) + page;
		win -> ypos = MIN( win -> ypos, win -> ypos_max - page);
		dy = ( int16) (( win->ypos - old_pos) * win->h_u);
		move_main_work( win, x, y, w, h, 0, dy, windata->frame_width, windata->border_width);
	}
}


/*==================================================================================*
 * void Win_RightPage:																*
 *		this function handle the WM_RTPAGE event for the main frame in the			* 
 *		window ( the picture's frame).												* 
 *----------------------------------------------------------------------------------*
 * input:																			*
 * 		win			-> 	The target window.											*
 *----------------------------------------------------------------------------------*
 * returns: 																		*	
 *		--																			*
 *==================================================================================*/

void Win_RightPage( WINDOW *win) 
{
	int16	page, x, y, w, h, dx;
	int32	old_pos = win -> xpos;

	WindGet( win, WF_WORKXYWH, &x, &y, &w, &h);

	page = w / win -> w_u;

	if ( win -> xpos < win -> xpos_max - page) 
	{
		WINDATA	*windata = ( WINDATA *)DataSearch( win, WD_DATA);	
		win -> xpos = MIN( win->xpos_max, win->xpos) + page;
		win -> xpos = MIN( win -> xpos, win -> xpos_max - page);
		dx = ( int16) (( win->xpos - old_pos) * win->w_u);
		move_main_work( win, x, y, w, h, dx, 0, windata->frame_width, windata->border_width);
	}
}



/*==================================================================================*
 * void Win_LeftPage:																*
 *		this function handle the WM_LFPAGE event for the main frame in the			* 
 *		window ( the picture's frame).												* 
 *----------------------------------------------------------------------------------*
 * input:																			*
 * 		win			-> 	The target window.											*
 *----------------------------------------------------------------------------------*
 * returns: 																		*	
 *		--																			*
 *==================================================================================*/

void Win_LeftPage( WINDOW *win) 
{
	int32	pos;
	int16	x, y, w, h, dx;

	if ( win -> xpos > 0L) 
	{
		WINDATA	*windata = ( WINDATA *)DataSearch( win, WD_DATA);
		WindGet( win, WF_WORKXYWH, &x, &y, &w, &h);
		pos = MAX( 0L, win->xpos - w / win->w_u);
		dx = ( int16) (( pos - win->xpos) * win->w_u);
		win->xpos = pos;
		move_main_work( win, x, y, w, h, dx, 0, windata->frame_width, windata->border_width);
	}
}



/*==================================================================================*
 * void Win_UpPage:																	*
 *		this function handle the WM_UPPAGE event for the main frame in the			* 
 *		window ( the picture's frame).												* 
 *----------------------------------------------------------------------------------*
 * input:																			*
 * 		win			-> 	The target window.											*
 *----------------------------------------------------------------------------------*
 * returns: 																		*	
 *		--																			*
 *==================================================================================*/

void Win_UpPage( WINDOW *win) 
{
	int32	pos;
	int16	x, y, w, h, dy;

	if ( win -> ypos > 0L) 
	{
		WINDATA	*windata = ( WINDATA *)DataSearch( win, WD_DATA);
		WindGet( win, WF_WORKXYWH, &x, &y, &w, &h);
		pos = MAX( 0L, win->ypos - h / win->h_u);
		dy = ( int16) (( pos - win->ypos) * win->h_u);
		win->ypos = pos;
		move_main_work( win, x, y, w, h, 0, dy, windata->frame_width, windata->border_width);
	}
}


/*==================================================================================*
 * void Win_UpLine:																	*
 *		this function handle the WM_UPLINE event for the main frame in the			* 
 *		window ( the picture's frame).												* 
 *----------------------------------------------------------------------------------*
 * input:																			*
 * 		win			-> 	The target window.											*
 *----------------------------------------------------------------------------------*
 * returns: 																		*	
 *		--																			*
 *==================================================================================*/

void Win_UpLine( WINDOW *win) 
{
	int16	x, y, w, h;	
	
	if ( win -> ypos > 0L) 
	{
		WINDATA	*windata = ( WINDATA *)DataSearch( win, WD_DATA);
		win->ypos --;
		WindGet( win, WF_WORKXYWH, &x, &y, &w, &h);
		move_main_work( win, x, y, w, h, 0, -win->h_u, windata->frame_width, windata->border_width);
	}
}

/*==================================================================================*
 * void Win_LeftLine:																*
 *		this function handle the WM_LFLINE event for the main frame in the			* 
 *		window ( the picture's frame).												* 
 *----------------------------------------------------------------------------------*
 * input:																			*
 * 		win			-> 	The target window.											*
 *----------------------------------------------------------------------------------*
 * returns: 																		*	
 *		--																			*
 *==================================================================================*/

void Win_LeftLine( WINDOW *win) 
{
	int16	x, y, w, h;	
	
	if ( win -> xpos > 0L) 
	{
		WINDATA	*windata = ( WINDATA *)DataSearch( win, WD_DATA);
		win->xpos --;
		WindGet( win, WF_WORKXYWH, &x, &y, &w, &h);		
		move_main_work( win, x, y, w, h, -win->w_u, 0, windata->frame_width, windata->border_width);
	}
}


/*==================================================================================*
 * void Win_RightLine:																*
 *		this function handle the WM_RTLINE event for the main frame in the			* 
 *		window ( the picture's frame).												* 
 *----------------------------------------------------------------------------------*
 * input:																			*
 * 		win			-> 	The target window.											*
 *----------------------------------------------------------------------------------*
 * returns: 																		*	
 *		--																			*
 *==================================================================================*/

void Win_RightLine( WINDOW *win) 
{
	int16	x, y, w, h;	

	WindGet( win, WF_WORKXYWH, &x, &y, &w, &h);	

	if (( win -> xpos < win -> xpos_max - w / win -> w_u) && ( win -> xpos_max > w / win -> w_u )) 
	{
		WINDATA	*windata = ( WINDATA *)DataSearch( win, WD_DATA);
		win -> xpos++;
		move_main_work( win, x, y, w, h, win->w_u, 0, windata->frame_width, windata->border_width);
	}
}

/*==================================================================================*
 * void Win_DownLine:																*
 *		this function handle the WM_DNLINE event for the main frame in the			* 
 *		window ( the picture's frame).												* 
 *----------------------------------------------------------------------------------*
 * input:																			*
 * 		win			-> 	The target window.											*
 *----------------------------------------------------------------------------------*
 * returns: 																		*	
 *		--																			*
 *==================================================================================*/

void Win_DownLine( WINDOW *win) 
{
	int16	x, y, w, h;	

	WindGet( win, WF_WORKXYWH, &x, &y, &w, &h);	

	if (( win -> ypos < win -> ypos_max - h / win -> h_u) && ( win -> ypos_max > h / win -> h_u )) 
	{
		WINDATA	*windata = ( WINDATA *)DataSearch( win, WD_DATA);
		win -> ypos ++;
		move_main_work( win, x, y, w, h, 0, win->h_u, windata->frame_width, windata->border_width);
	}
}


/*==================================================================================*
 * void Win_Arrow:																	*
 *		this function handle the WM_ARROWED event for the main frame in the			* 
 *		window ( the picture's frame).												*
 *----------------------------------------------------------------------------------*
 * input:																			*
 * 		win			-> 	The target window.											*
 *----------------------------------------------------------------------------------*
 * returns: 																		*	
 *		--																			*
 *==================================================================================*/

void Win_Arrow( WINDOW *win) 
{
	switch( evnt.buff[4]) 
	{
		case WA_UPPAGE:
			Win_UpPage( win);
			break;
		case WA_DNPAGE:
			Win_DownPage( win);
			break;
		case WA_LFPAGE:
			Win_LeftPage( win);
			break;
		case WA_RTPAGE:
			Win_RightPage( win);
			break;			
		case WA_UPLINE:
			Win_UpLine( win);
			break;
		case WA_DNLINE:
			Win_DownLine( win);
			break;
		case WA_LFLINE:
			Win_LeftLine( win);
			break;		
		case WA_RTLINE:
			Win_RightLine( win);
			break;					
		default:
			break;
	}
}




WINDOW *WindView( char *filename)
{
	WINDOW 		*winview = NULL;
	WINDATA		*windata;
	IMAGE 		*img;
	int16		w, h;
	char 		extention[4];

    graf_mouse( BUSYBEE, NULL);		
	
	if ( !icons_init())
	{
		graf_mouse( ARROW, NULL);
		errshow( "", NO_ICON);
		applexit();
	}	

	/* get the file extention */
	strcpy ( extention, filename + strlen( filename) - 3);
	str2upper( extention);
	    
	if( strncmp( "PDF", extention, 3) == 0)
		return WindPdf( filename);
	   
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

	if ( !pic_load( filename, extention, img))
	{
		gfree( windata);
		// errshow( "", IMG_NO_VALID);
		graf_mouse( ARROW, NULL);
		return NULL;
	}
	
	sprintf( windata->info, " %dx%dx%d | %ld colors | %s", img->img_w, img->img_h, img->bits, img->colors, img->info);

	
	if ( ( winview	= WindCreate( WAT_ALL, app.x, app.y, app.w, app.h)) == NULL)
	{
		delete_mfdb( img->image, img->page);
		gfree( windata);
		errshow( "", ALERT_WINDOW);		
		graf_mouse( ARROW, NULL);
		return NULL;
	}

    DataAttach( winview, WD_DATA, 	  windata);	
	EvntAttach( winview, WM_REDRAW,	  WindViewRedraw);
	EvntAttach( winview, WM_DESTROY,  WindViewClose);
	EvntAttach( winview, WM_XKEYBD,	  WindViewKeyb);
	EvntAttach( winview, WM_TOPPED,   WindViewTop);
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
	EvntAdd( 	winview, WM_ICONIFY,  WindViewIconify,   EV_TOP);
	EvntAdd( 	winview, WM_UNICONIFY,WindViewUniconify, EV_BOT);
		
	WindSetStr( winview, WF_NAME,	  windata->name);
	WindSetStr( winview, WF_ICONDRAW, WindViewIcon); 
	WindSetStr( winview, WF_INFO,	  windata->info);

	WindSetPtr( winview, WF_TOOLBAR,  get_tree( VIEWTOOLBAR), WindViewTool);

	WindCalc( WC_BORDER, winview, 0, 0, img->image[0].fd_w, img->image[0].fd_h + 1, &dum, &dum, &w, &h);

	RsrcUserDraw ( OC_TOOLBAR, winview, VIEWTOOLBAR_BIG, draw_icon_greater, windata);
	RsrcUserDraw ( OC_TOOLBAR, winview, VIEWTOOLBAR_SMALL, draw_icon_smaller, windata);
	RsrcUserDraw ( OC_TOOLBAR, winview, VIEWTOOLBAR_OPEN, draw_icon_open, NULL);
	RsrcUserDraw ( OC_TOOLBAR, winview, VIEWTOOLBAR_INFO, draw_icon_info, NULL);
	RsrcUserDraw ( OC_TOOLBAR, winview, VIEWTOOLBAR_SAVE, draw_icon_save, NULL);
	RsrcUserDraw ( OC_TOOLBAR, winview, VIEWTOOLBAR_FULLSCREEN, draw_icon_fullscreen, NULL);
	RsrcUserDraw ( OC_TOOLBAR, winview, VIEWTOOLBAR_PRINT, draw_icon_printer, NULL);
			
	w = MIN( w, app.w);
	h = MIN( h, app.h);

	winview -> h_min 	= 80;
	winview -> w_min 	= 80;
/*	winview -> h_max 	= MAX( h, winview -> h_min);
	winview -> w_max 	= MAX( w, winview -> w_min);
*/	winview -> h_u   	= 8;
	winview -> w_u   	= 8;
	winview -> ypos_max = ( int32)( ( img->image[0].fd_h + 1) >> 3);
	winview -> xpos_max = ( int32)( img->image[0].fd_w >> 3);

	if ( !( WindOpen( winview, -1, -1, w, h)))
	{
		WindViewClose( winview);
		errshow( "", ALERT_WINDOW);		
		graf_mouse( ARROW, NULL);
		return NULL;
	}

	menu_ienable( get_tree( MENU_BAR), MENU_BAR_INFORMATION, 1);
	menu_ienable( get_tree( MENU_BAR), MENU_BAR_SAVE, 1);
	menu_ienable( get_tree( MENU_BAR), MENU_BAR_SHOW_FULLSCREEN, 1);

	if( ( img->page > 1) && ( img->delay[0] > 0))
	{
		windata->chrono_value = clock();
		TimerAttach( winview, WindViewAnim);
	}

	graf_mouse( ARROW, NULL);		

	return winview;
}
