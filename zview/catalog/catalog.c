#include "../general.h"
#include "../prefs.h"
#include "../file/file.h"
#include "../file/delete.h"
#include "catalog_entry.h"
#include "catalog_mini_entry.h"
#include "../zedit/zedit.h"
#include "../ztext.h"
#include "../mfdb.h"
#include "../infobox.h"
#include "../menu.h"
#include "../txt_data.h"
#include "../custom_font.h"
#include "catalog_icons.h"
#include "catalog_keyb.h"
#include "catalog_mouse.h"
#include "catalog_slider.h"
#include "catalog_size.h"
#include "catalog_iconify.h"
#include "catalog_other_event.h"

/* Global variable */
int16	border_size 			= 5;
int16	old_browser_size 		= 200;
int16	old_preview_area_size 	= 200;
int16	old_border_size 		= 5;
int16	x_space 				= 5;
int16	y_space 				= 4;
int16	hcell					= 8;
int16	need_frame_slider 		= 0;
int16	draw_frame_slider 		= 1;
OBJECT 	*frame_slider_root;

/* local variable */
static int16 		dum, xw, yw, ww, hw;


/* Prototype */
static void WinCatalog_Close( WINDOW *win);
static void WinCatalog_Redraw( WINDOW *wind);
static void WinCatalog_Tool( WINDOW *win);
void 	WinCatalog_Refresh( WINDOW *win);
int 	WindMakePreview_needed( WINDOW *win);
void 	WindMakePreview( WINDOW *win);
int 	WinCatalog( void);
void 	WinCatalog_filelist_redraw( void);
void 	WinCatalog_set_thumbnails_size( WINDOW *win);


extern void save_dialog( const char *fullfilename);

/*==================================================================================*
 * int WindMakePreview_needed:														*
 *		Look if a preview making is needed.											*
 *----------------------------------------------------------------------------------*
 * input:																			*
 * 		win		-> Pointer to the target window										*
 *----------------------------------------------------------------------------------*
 * returns: 																		*
 *      '1' if needed else '0'														*
 *==================================================================================*/

int WindMakePreview_needed( WINDOW *win)
{
	int16 i, h;
	WINDICON *wicones = ( WINDICON *)DataSearch( win, WD_ICON);

	WindGet ( win, WF_WORKXYWH, &dum, &dum, &dum, &h); 	

	for (i = 0 ; i < wicones->nbr_icons ; i++)	
	{
		if ( icon_is_visible( &wicones->entry[i], h) == FALSE)
			continue;

		if ( ( wicones->entry[i].type == ET_IMAGE) && ( wicones->entry[i].preview.page == 0))
			return( 1);
	}
	return ( 0);
}



/*==================================================================================*
 * void WindMakePreview:															*
 *		Create picture's preview for each visible picture files in the folder.		*
 *		If preview is already done, we skip it.										*
 *		If preview making is impossible, we change the file's type from ET_IMAGE	*
 *		to ET_FILE.																	*
 *----------------------------------------------------------------------------------*
 * input:																			*
 * 		win		-> Pointer to the target window										*
 *----------------------------------------------------------------------------------*
 * returns: 																		*
 *      --																			*
 *==================================================================================*/

void WindMakePreview( WINDOW *win)
{
	int16 	i, h;
	char 	extention[4];	
	WINDICON *wicones = ( WINDICON *)DataSearch( win, WD_ICON);

	draw_frame_slider = 0;

	WindGet ( win, WF_WORKXYWH, &dum, &dum, &dum, &h); 	

    graf_mouse( BUSYBEE, NULL);	

	for (i = 0 ; i < wicones->nbr_icons ; i++)	
	{
	
		if ( icon_is_visible( &wicones->entry[i], h) == FALSE)
			continue;

		if ( ( wicones->entry[i].type == ET_IMAGE) && ( wicones->entry[i].preview.page == 0))
		{			
			wicones->entry[i].preview.view_mode    = preview_mode;
			wicones->entry[i].preview.progress_bar = 0;

			// MenuDisable();

			/* get the file extention */
			strcpy ( extention, wicones->entry[i].name + strlen( wicones->entry[i].name) - 3);
			str2upper( extention);

			if( pic_load( wicones->entry[i].name, extention, &wicones->entry[i].preview))
				redraw_icon( win, &wicones->entry[i]);
			else
				wicones->entry[i].type = ET_FILE;
		}
		
		// evnt_timer( 100L);
	}

	draw_frame_slider = 1;
	graf_mouse( ARROW, NULL);
	// MenuEnable();
}



/*==================================================================================*
 * WINDOW *WinCatalog:																*
 *		Create the Window and bind the severals event for it.						*
 *----------------------------------------------------------------------------------*
 * input:																			*
 * 		--																			*
 *----------------------------------------------------------------------------------*
 * returns: 																		*
 *      '0' if error, else '1'.														*
 *==================================================================================*/

int WinCatalog( void)
{
	WINDICON 	*wicones;
	OBJECT		*menu;
	int16		w, h;

	if ( win_catalog)
		return( 0);

	if ( !icons_init())
	{
		errshow( "", NO_ICON);
		applexit();
	}
			
	if ( ( wicones = ( WINDICON *) gmalloc( sizeof( WINDICON))) == NULL)
	{
		errshow( "", ENOMEM);
		return( 0);
	}

	if ( ( win_catalog	= WindCreate( WINATTRIB, app.x, app.y, app.w, app.h)) == NULL)
	{
		gfree( wicones);
		errshow( "", ALERT_WINDOW);
		return( 0);
	}	


	/* Set some wicones's parameters */
	wicones -> case_h  		= thumbnail[thumbnail_size][1] + hcell + y_space + y_space + y_space;
	
	if ( show_size)
		wicones -> case_h   += hcell + y_space;

	wicones -> case_w 		= thumbnail[thumbnail_size][0] + ( x_space << 1);		
	wicones -> nbr_icons 	= 0;	
	wicones -> edit 		= NULL;
	wicones -> h_u   		= 18;	/* 1 + mini_entry's icon + 1 */
	wicones -> ypos   		= 0;
	wicones -> ypos_max   	= 1;

    DataAttach( win_catalog, 	WD_ICON,	wicones);

	if ( !scan_mini_drv( win_catalog))
	{
		zdebug( "can't scan the drive");
		gfree( wicones);
		DataDelete( win_catalog, WD_ICON);
		WindDelete( win_catalog);
		return( 0);
	}	
	
	check_mini_dir( wicones->nbr_child, wicones->root);
			
	if( !scan_dir( win_catalog, "C:\\"))
	{
		zdebug( "can't scan the dir");
		WinCatalog_Close( win_catalog);
		return( 0);		
	}		

	EvntAttach( win_catalog, WM_REDRAW,	 	WinCatalog_Redraw);
	EvntAttach( win_catalog, WM_DESTROY, 	WinCatalog_Close);
	EvntAttach( win_catalog, WM_SIZED,	 	WinCatalog_Size);
	EvntAttach( win_catalog, WM_ARROWED, 	WinCatalog_Arrow);
	EvntAttach( win_catalog, WM_UPPAGE,  	WinCatalog_UpPage);
	EvntAttach( win_catalog, WM_DNPAGE,  	WinCatalog_DownPage);
	EvntAttach( win_catalog, WM_UPLINE,  	WinCatalog_UpLine);
	EvntAttach( win_catalog, WM_DNLINE,  	WinCatalog_DownLine);
	EvntAttach( win_catalog, WM_VSLID ,  	WinCatalog_VSlide);
	EvntAttach( win_catalog, WM_FULLED,  	WinCatalog_Fulled);
//	EvntAttach( win_catalog, AP_DRAGDROP, 	WinCatalog_DragDrop);
	EvntAdd( 	win_catalog, WM_ICONIFY,  	catalog_iconify,   EV_TOP);
	EvntAdd( 	win_catalog, WM_UNICONIFY,	catalog_uniconify, EV_TOP);
	EvntAdd( 	win_catalog, WM_TOPPED,   	WinCatalog_top,    EV_BOT);
	EvntAdd( 	win_catalog, WM_BOTTOMED, 	WinCatalog_bottom, EV_BOT);

	EvntAttach( win_catalog, WM_XBUTTON, 	WinCatalog_Mouse);
	EvntAttach( win_catalog, WM_XKEYBD,	 	WinCatalog_Keyb);

	WindSetStr( win_catalog, WF_NAME,	 	wicones->directory);
	WindSetPtr( win_catalog, WF_TOOLBAR, 	get_tree( TOOLBAR), WinCatalog_Tool);
	WindSetStr( win_catalog, WF_ICONDRAW,	draw_window_iconified);

	WindCalc( WC_BORDER, win_catalog, 0, 0, wicones->case_w + border_size + browser_frame_width, wicones->case_h, &dum, &dum, &w, &h);

	/* Set win's parameters */
	win_catalog -> h_min 							= h;
	win_catalog -> w_min 							= w;
	win_catalog	-> h_max 							= app.h;
	win_catalog	-> w_max 							= app.w;
	win_catalog -> h_u   							= wicones -> case_h;
	win_catalog -> w_u   							= wicones -> case_w;
	win_catalog	-> tool.root[TOOLBAR_BACK].ob_width = app.w;

	/* install userdraw for the toolbar */
	RsrcUserDraw ( OC_TOOLBAR, win_catalog, TOOLBAR_UP, draw_icon_up, NULL);
	RsrcUserDraw ( OC_TOOLBAR, win_catalog, TOOLBAR_RELOAD, draw_icon_reload, NULL);
	RsrcUserDraw ( OC_TOOLBAR, win_catalog, TOOLBAR_BIG, draw_icon_greater, NULL);
	RsrcUserDraw ( OC_TOOLBAR, win_catalog, TOOLBAR_SMALL, draw_icon_smaller, NULL);
	RsrcUserDraw ( OC_TOOLBAR, win_catalog, TOOLBAR_PRINT, draw_icon_printer, NULL);
	RsrcUserDraw ( OC_TOOLBAR, win_catalog, TOOLBAR_OPEN, draw_icon_open, NULL);
	RsrcUserDraw ( OC_TOOLBAR, win_catalog, TOOLBAR_INFO, draw_icon_info, NULL);
	RsrcUserDraw ( OC_TOOLBAR, win_catalog, TOOLBAR_DELETE, draw_icon_delete, NULL);
	RsrcUserDraw ( OC_TOOLBAR, win_catalog, TOOLBAR_SAVE, draw_icon_save, NULL);
		
	/* get the frame slider's address */
	rsrc_gaddr( 0, SLIDERS, &frame_slider_root);
	
	if( !browser_w && !browser_h)
	{
		browser_x = app.x;
		browser_y = app.y;
		browser_w = app.w;
		browser_h = app.h;
	}

	if( !WindOpen( win_catalog, browser_x, browser_y, browser_w, browser_h))		
	{
		WinCatalog_Close( win_catalog);
		errshow( "", ALERT_WINDOW);
		return( 0);		
	}


	menu = get_tree( MENU_BAR);
	
	menu_ienable( menu, MENU_BAR_BROWSER, 0);
	menu_ienable( menu, MENU_BAR_CLOSE, 1);
	menu_ienable( menu, MENU_BAR_SELECT_ALL, 1);
	menu_ienable( menu, MENU_BAR_BY_NAME, 1);
	menu_ienable( menu, MENU_BAR_BY_SIZE, 1);
	menu_ienable( menu, MENU_BAR_BY_DATE, 1);
	menu_ienable( menu, MENU_BAR_SHOW_BROWSER, 1);
	menu_ienable( menu, MENU_BAR_SHOW, 1);
	menu_ienable( menu, MENU_BAR_LONG_THUMB, 1);
	menu_ienable( menu, MENU_BAR_LARGE_THUMB, 1);	
	menu_ienable( menu, MENU_BAR_INFORMATION, 0);
	menu_ienable( menu, MENU_BAR_SAVE, 0);

	if( WindMakePreview_needed( win_catalog))
	{
		EvntRedraw( win_catalog);
		WindMakePreview( win_catalog);
	}	

	return ( 1);
}

/*==================================================================================*
 * WinCatalog_Close:																*
 *		Destroy the WINDOW *win and free up memory.									*
 *----------------------------------------------------------------------------------*
 * input:																			*
 * 		win		-> Pointer to the target window										*
 *----------------------------------------------------------------------------------*
 * returns: 																		*
 *      --																			*
 *==================================================================================*/

static void WinCatalog_Close( WINDOW *win)
{
	int16 	i;
	OBJECT	*menu = get_tree( MENU_BAR);

	WINDICON *wicones = ( WINDICON *)DataSearch( win, WD_ICON);

	if( !( win->status & WS_ICONIFY))
	{
		WindGet ( win, WF_CURRXYWH, &xw, &yw, &ww, &hw); 	

		browser_x = xw;
		browser_y = yw;
		browser_w = ww;
		browser_h = hw;
	}

	for ( i = 0 ; i < wicones->nbr_icons ; i++)	
	{
		delete_txt_data( &wicones->entry[i].preview);
		delete_mfdb( wicones->entry[i].preview.image, 1);
	}

	if ( wicones->edit)
		edit_free( wicones->edit);

	/* free allocated data structures */
	if ( wicones->nbr_icons > 0)
		gfree( wicones->entry);

	if( wicones->root)
	{	   	
		for ( i = 0; i < wicones->nbr_child; i++)
		{
			if ( wicones->root[i].nbr_child)
				delete_mini_entry_child( &wicones->root[i]);
		}

		gfree( wicones->root);
	}

	gfree( wicones);

	DataDelete( win, WD_ICON);

	WindDelete( win);
	
	if( wglb.first)
	{
		menu_ienable( menu, MENU_BAR_CLOSE, 1);
		menu_ienable( menu, MENU_BAR_SAVE, 1);
	}
	else
	{
		menu_ienable( menu, MENU_BAR_SAVE, 0);
		menu_ienable( menu, MENU_BAR_INFORMATION, 0);
	}		

	menu_ienable( menu, MENU_BAR_SELECT_ALL, 0);
	menu_ienable( menu, MENU_BAR_BY_NAME, 0);
	menu_ienable( menu, MENU_BAR_BY_SIZE, 0);
	menu_ienable( menu, MENU_BAR_BY_DATE, 0);
	menu_ienable( menu, MENU_BAR_SHOW, 0);
	menu_ienable( menu, MENU_BAR_SHOW_BROWSER, 0);
	menu_ienable( menu, MENU_BAR_DELETE, 0);
	menu_ienable( menu, MENU_BAR_LONG_THUMB, 0);
	menu_ienable( menu, MENU_BAR_LARGE_THUMB, 0);
	menu_ienable( menu, MENU_BAR_BROWSER, 1);

	win_catalog = NULL;
}


/*==================================================================================*
 * WinCatalog_tool:																	*
 *		Toolbar binding.															*
 *----------------------------------------------------------------------------------*
 * input:																			*
 * 		win		-> Pointer to the target window										*
 *----------------------------------------------------------------------------------*
 * returns: 																		*
 *      --																			*
 *==================================================================================*/

static void WinCatalog_Tool( WINDOW *win)
{
	WINDICON *wicones = ( WINDICON *)DataSearch( win, WD_ICON);
	Mini_Entry	*old_selected;
	char 	 temp[MAX_PATH + 256];
	int16 	 i;

	WindGet ( win, WF_WORKXYWH, &xw, &yw, &ww, &hw); 	

	switch( evnt.buff[4])
	{
		case TOOLBAR_UP:
			if ( !dir_parent( wicones->directory, temp))
				break;

			old_selected = wicones->mini_selected;
			wicones->mini_selected = NULL;

			if ( old_selected && browser_frame_width)
				redraw_mini_entry( win, old_selected);

			( void)scan_dir( win, wicones->directory);
			WindSetStr ( win, WF_NAME, wicones->directory);				
				
			win->ypos =  0;
							
			for (i = 0 ; i < wicones->nbr_icons ; i++)	
			{
				if ( strcmp( wicones->entry[i].name, temp) == 0)
				{
					wicones->first_selected = &wicones->entry[i];
					break;
				}
			}
			
			WinCatalog_Refresh( win);
						
			if ( wicones->first_selected->txt_pos.y2 > hw)
			{	
				while ( wicones->first_selected->txt_pos.y2 > hw)
				{
					wicones->first_selected->txt_pos.y2 -= win->h_u;
					win->ypos++; 
				}  
			}

			draw_page( win, xw + browser_frame_width + border_size , yw, ww - ( browser_frame_width + border_size), hw);
			WindMakePreview( win);

			break;

		case TOOLBAR_SMALL:
			if ( thumbnail_size && thumbnail_size != 4)
			{
				thumbnail_size--;

				WinCatalog_set_thumbnails_size( win);				
								
				if( thumbnail_size == 6 || thumbnail_size == 2)
					ObjcDraw( OC_TOOLBAR, win, TOOLBAR_BIG, 1);		
			}
			break;

		case TOOLBAR_BIG:
			if ( thumbnail_size < 7 && thumbnail_size != 3)
			{
				thumbnail_size++;

				WinCatalog_set_thumbnails_size( win);	
				
				if( thumbnail_size == 1 || thumbnail_size == 5)
					ObjcDraw( OC_TOOLBAR, win, TOOLBAR_SMALL, 1);		
			}
			break;

		case TOOLBAR_RELOAD:
			( void)scan_dir( win, wicones->directory);
			win->ypos =  0;
			draw_page( win, xw + browser_frame_width + border_size , yw + 1, ww - ( browser_frame_width + border_size), hw - 1);
			WindMakePreview( win);
			break;

		case TOOLBAR_OPEN:
			Menu_open_image();
			break;			

		case TOOLBAR_INFO:
			if( wicones->first_selected)
				infobox();
			break;					

		case TOOLBAR_SAVE:
			if( wicones->first_selected && wicones->first_selected->type == ET_IMAGE)
			{		
				if( wicones->edit)
					exit_edit_mode( win, wicones->first_selected);
		
				strcpy( temp, wicones->directory);
				strcat( temp, wicones->first_selected->name);
				save_dialog( temp);
				break;					
			}
			break;			
		case TOOLBAR_DELETE:
			if( wicones->first_selected)
			{
				if( wicones->edit)
					exit_edit_mode( win, wicones->first_selected);

				delete_entry( win);

			}
			break;

		default:
			break;
	}

	ObjcChange( OC_TOOLBAR, win, evnt.buff[4], NORMAL, 1);
}


/*==================================================================================*
 * WinCatalog_Refresh:																*
 *		Perform a redraw without draw anything in screen.. usefull for refresh 		*
 *		icons positions ( need for various opertaions)								*
 *----------------------------------------------------------------------------------*
 * input:																			*
 * 		win		-> Pointer to the target window										*
 *----------------------------------------------------------------------------------*
 * returns: 																		*
 *      --																			*
 *==================================================================================*/

void WinCatalog_Refresh( WINDOW *win)
{
	int16 	icon_w, icon_h, i, x, y, ytext, xtext, lines = 0, width;

    WINDICON *wicones = ( WINDICON *)DataSearch( win, WD_ICON);

	WindGet ( win, WF_WORKXYWH, &xw, &yw, &ww, &hw);

	wicones -> columns 			= 0;
	wicones -> icons_last_line 	= 0;

	if( browser_frame_width) /* browser active ? */
	{	 
		for ( i = 0 ; i < wicones->nbr_child; i++)
		{	
			lines++;
			lines += calc_mini_entry_line( &wicones->root[i]);  
		}	

		wicones -> ypos_max = lines;

		/* compute the frame slider's coordinates even if we don't need it */
		frame_slider_root->ob_x 					= xw + browser_frame_width - 15;
		frame_slider_root->ob_y 					= yw + 2;
		frame_slider_root->ob_height  				= hw - 3;
		frame_slider_root[SLIDERS_BACK].ob_height 	= hw - 31;			
		frame_slider_root[SLIDERS_UP].ob_y 			= frame_slider_root->ob_height - 31;	
		frame_slider_root[SLIDERS_DOWN].ob_y 		= frame_slider_root->ob_height - 15;

		calc_mini_entry_slider( wicones, frame_slider_root);
	}

	lines 						= 1;
	x 							= xw + border_size + browser_frame_width; 
	y 							= yw - ( ( int16)win->ypos * wicones -> case_h);	

	for ( i = 0 ; i < wicones->nbr_icons; i++)	
	{  
		if ( ( !show_non_image) && ( wicones->entry[i].type != ET_IMAGE)) 
			continue;
			
		if ( x + wicones->case_w > xw + ww)					
		{
		 	x = xw + border_size + browser_frame_width;
			y += wicones -> case_h;				/* Jump to next line */
			lines++;
			wicones->icons_last_line = 1;
		}
		else
		{
			wicones -> icons_last_line++;	
			if ( wicones -> columns < wicones -> icons_last_line)
				wicones -> columns++;
		}

		icon_w = wicones->entry[i].preview.page ? wicones->entry[i].preview.image->fd_w : wicones->entry[i].icon->fd_w;
		icon_h = wicones->entry[i].preview.page ? wicones->entry[i].preview.image->fd_h : wicones->entry[i].icon->fd_h;


		ytext = y + thumbnail[thumbnail_size][1] + ( y_space << 1);

		/* save the case position in the structure */
		wicones->entry[i].case_pos.x1 = x - xw;
		wicones->entry[i].case_pos.y1 = y - yw;
		wicones->entry[i].case_pos.x2 = wicones->entry[i].case_pos.x1 + wicones->case_w;
		wicones->entry[i].case_pos.y2 = wicones->entry[i].case_pos.y1 + wicones->case_h;	

		if( wicones->edit && check_selected_entry( wicones, &wicones->entry[i]))
		{
			int16 txt_width = get_text_width( wicones->edit->top->buf);
			
			width =  MAX( txt_width, 16);
			
		}
		else
			width =  wicones->entry[i].icon_txt_w;


		xtext = x + (( wicones->case_w - 1 - width) >> 1);

		/* save the text position in the structure */
		wicones->entry[i].txt_pos.x1 = xtext - xw - 2;
		wicones->entry[i].txt_pos.y1 = ytext - 1 - yw;
		wicones->entry[i].txt_pos.x2 = wicones->entry[i].txt_pos.x1 + width + 4;
		wicones->entry[i].txt_pos.y2 = wicones->entry[i].txt_pos.y1 + hcell + 2;

		/* save the icon position in the structure */
		wicones->entry[i].icn_pos.x1 = ( x + (( wicones->case_w - icon_w) >> 1)) - xw;								
		wicones->entry[i].icn_pos.y1 = ( y + y_space + (( thumbnail[thumbnail_size][1] - icon_h) >> 1) + 1) - yw;								
		wicones->entry[i].icn_pos.x2 = wicones->entry[i].icn_pos.x1 + icon_w - 1;
		wicones->entry[i].icn_pos.y2 = wicones->entry[i].txt_pos.y1;

		x += wicones->case_w;
	}

	win -> ypos_max = lines;
	win -> xpos_max = wicones->columns; 
	
}


/*==================================================================================*
 * WinCatalog_Redraw:																*
 *		redraw event for the WINDOW *win.											*
 *----------------------------------------------------------------------------------*
 * input:																			*
 * 		win		-> Pointer to the target window										*
 *----------------------------------------------------------------------------------*
 * returns: 																		*
 *      --																			*
 *==================================================================================*/

static void WinCatalog_Redraw( WINDOW *wind)
{
	int16 		icon_w, icon_h, i, x, y, xtext, ytext, pxy[4], lines = 0, extent[10];
	RECT16		dst_rect;
    WINDICON 	*wicones = (WINDICON *)DataSearch( wind, WD_ICON);
	
	WindGet( wind, WF_WORKXYWH, &xw, &yw, &ww, &hw);

	wicones -> columns 			= 0;
	wicones -> icons_last_line 	= 0;

	if( browser_frame_width) /* browser active ? */
	{	 
		pxy[0] = xw;
		pxy[1] = yw;
		pxy[2] = pxy[0] + browser_frame_width;
		pxy[3] = pxy[1] + hw - 1;

		/* clean browser frame */	
		vsf_color( wind->graf.handle, WHITE);
		v_bar( wind->graf.handle, pxy);

		wicones -> border_position[0] = browser_frame_width;
		wicones -> border_position[1] = browser_frame_width + border_size;	


		/* Draw the 1st frame ( navigation folder) */
		y = 0 - ( ( int16)wicones -> ypos * wicones -> h_u);
		
		for ( i = 0 ; i < wicones->nbr_child; i++)
		{	
			wicones->root[i].arrow_position.x1 = 3;
			wicones->root[i].arrow_position.y1 = y + 3;
			wicones->root[i].arrow_position.x2 = wicones->root[i].arrow_position.x1 + 11;
			wicones->root[i].arrow_position.y2 = wicones->root[i].arrow_position.y1 + 11;

			wicones->root[i].icon_position.x1  = 15;
			wicones->root[i].icon_position.y1  = y + 1;
			wicones->root[i].icon_position.x2  = wicones->root[i].icon_position.x1 + 15 + x_space + wicones->root[i].icon_txt_w;
			wicones->root[i].icon_position.y2  = wicones->root[i].icon_position.y1 + 15;

			draw_mini_entry( wind, wicones->mini_selected, &wicones->root[i], xw, yw, hw);

			lines++;
			y += wicones -> h_u;

			dum = draw_mini_entry_child( wind, wicones->mini_selected, &wicones->root[i], 15, y, xw, yw, hw);  

			y += ( dum * wicones -> h_u);

			lines += dum;		
		}	

		wicones -> ypos_max = lines;

		/* Look if we need a frame slider */
		if( ( wicones -> ypos_max * wicones -> h_u) >= hw)
		{
			if ( draw_frame_slider)
			{
				frame_slider_root->ob_x 					= xw + browser_frame_width - 15;
				frame_slider_root->ob_y 					= yw + 2;
				frame_slider_root->ob_height  				= hw - 3;
				frame_slider_root[SLIDERS_BACK].ob_height 	= hw - 31;			
				frame_slider_root[SLIDERS_UP].ob_y 			= frame_slider_root->ob_height - 31;	
				frame_slider_root[SLIDERS_DOWN].ob_y 		= frame_slider_root->ob_height - 15;

				calc_mini_entry_slider( wicones, frame_slider_root);
				objc_draw( frame_slider_root, SLIDERS_BOX, 2, clip.g_x, clip.g_y, clip.g_w, clip.g_h);
			}

			need_frame_slider = 1;
		}	
		else
			need_frame_slider = 0;

		/* Draw the frame border */
		pxy[0] = pxy[2];
		vsl_color( wind->graf.handle, BLACK);
		v_pline( wind->graf.handle, 2, pxy);

		pxy[0]++;
		pxy[2] = pxy[0];
		vsl_color( wind->graf.handle, WHITE);
		v_pline( wind->graf.handle, 2, pxy);

		pxy[0]++;
		pxy[2] = pxy[0];
		vsl_color( wind->graf.handle, LWHITE);
		v_pline( wind->graf.handle, 2, pxy);

		pxy[0]++;
		pxy[2] = pxy[0];
		v_pline( wind->graf.handle, 2, pxy);	

		pxy[0]++;
		pxy[2] = pxy[0];
		vsl_color( wind->graf.handle, BLACK);
		v_pline( wind->graf.handle, 2, pxy);
		
		pxy[0]++;	

		/* Draw the 2nd frame ( the file) */	
		pxy[2] = pxy[0] + ww - ( browser_frame_width + border_size) - 1;
		vsf_color( wind->graf.handle, WHITE);
		v_bar( wind->graf.handle, pxy);

		pxy[3] = yw;
		v_pline( wind->graf.handle, 2, pxy);
		pxy[0] = xw;
		pxy[2] = pxy[0] + browser_frame_width;
		v_pline( wind->graf.handle, 2, pxy);
	}
	else
	{	 
		/* clean browser frame */	
		pxy[0] = xw;
		pxy[1] = yw;
		pxy[2] = pxy[0] + ww - 1;
		pxy[3] = pxy[1] + hw - 1;

		vsf_color( wind->graf.handle, WHITE);
		v_bar( wind->graf.handle, pxy);

		/* Draw a horizontal line at the window top */
		pxy[3] = pxy[1];
		vsl_color( wind->graf.handle, BLACK);
		v_pline( wind->graf.handle, 2, pxy);
	}

//	yw++;
//	hw--;
	
	/*----------------------------------------------------------------------*/
	/* 						MAKE THE FIRST PASS								*/
	/* WE DRAW ONLY ALL THE ICONS BUT NOT THE TEXT OF THE SELECTED'S ONE	*/
	/* IF WE DRAW ALL THE TEXTS NOW, THE MULTILINES TEXT AREN'T VISIBLE 	*/
	/*----------------------------------------------------------------------*/

	/* Draw the folder's content */
	x 							= xw + border_size + browser_frame_width;
	y 							= yw - ( ( int16)wind->ypos * wicones -> case_h);
	lines 						= 1;

	for ( i = 0 ; i < wicones->nbr_icons; i++)
	{
		if ( ( !show_non_image) && ( wicones->entry[i].type != ET_IMAGE)) 
			continue;
			
		if ( x + wicones->case_w > xw + ww)
		{
		 	x = xw + border_size + browser_frame_width;
			y += wicones -> case_h;				/* Jump to next line */
			lines++;
			wicones->icons_last_line = 1;
		}
		else
		{
			wicones -> icons_last_line++;

			if ( wicones -> columns < wicones -> icons_last_line)
				wicones -> columns++;
		}

		icon_w = wicones->entry[i].preview.page ? wicones->entry[i].preview.image->fd_w : wicones->entry[i].icon->fd_w;
		icon_h = wicones->entry[i].preview.page ? wicones->entry[i].preview.image->fd_h : wicones->entry[i].icon->fd_h;

		dst_rect.x1 = x + (( wicones->case_w - icon_w) >> 1);
		dst_rect.y1 = y + y_space + (( thumbnail[thumbnail_size][1] - icon_h) >> 1) + 1;
		dst_rect.x2 = dst_rect.x1 + icon_w - 1;
		dst_rect.y2 = dst_rect.y1 + icon_h - 1;

		/* save the case position in the structure */
		wicones->entry[i].case_pos.x1 = x - xw;
		wicones->entry[i].case_pos.y1 = y - yw;
		wicones->entry[i].case_pos.x2 = wicones->entry[i].case_pos.x1 + wicones->case_w;
		wicones->entry[i].case_pos.y2 = wicones->entry[i].case_pos.y1 + wicones->case_h;	
	
		xtext = x + (( wicones->case_w - 1 - wicones->entry[i].icon_txt_w) >> 1);
		ytext = y + thumbnail[thumbnail_size][1] + y_space + y_space;

		/* save the text position in the structure */
		wicones->entry[i].txt_pos.x1 = xtext - xw - 2;
		wicones->entry[i].txt_pos.y1 = ytext - 1 - yw;
		wicones->entry[i].txt_pos.x2 = wicones->entry[i].txt_pos.x1 + wicones->entry[i].icon_txt_w + 4;
		wicones->entry[i].txt_pos.y2 = wicones->entry[i].txt_pos.y1 + hcell + 2;

		/* save the icon position in the structure */
		wicones->entry[i].icn_pos.x1 = dst_rect.x1 - xw;								
		wicones->entry[i].icn_pos.y1 = dst_rect.y1 - yw;								
		wicones->entry[i].icn_pos.x2 = wicones->entry[i].icn_pos.x1 + icon_w - 1;
		wicones->entry[i].icn_pos.y2 = wicones->entry[i].txt_pos.y1;

		if( y < yw || y >= yw + hw)
		{
			x += wicones->case_w;
			continue;
		}

		if ( !check_selected_entry( wicones, &wicones->entry[i]))
		{
			draw_icon( wind->graf.handle, &wicones->entry[i], FALSE, &dst_rect);
			draw_text( wind->graf.handle, xtext, ytext, BLACK, wicones->entry[i].name_shown);
			vsl_color( wind->graf.handle, LWHITE);
		}
		else
		{
			draw_icon( wind->graf.handle, &wicones->entry[i], TRUE, &dst_rect);
		}		

		/* Draw the case 'contour' */
		pxy[1] = y + 1;
		pxy[3] = y + wicones->case_h - 2;
		pxy[0] = x + 1;
		pxy[2] = x + wicones->case_w - 2;

		extent[0] = pxy[0];
		extent[1] = pxy[1];
		extent[2] = pxy[0];
		extent[3] = pxy[3];
		extent[4] = pxy[2];
		extent[5] = pxy[3];
		extent[6] = pxy[2];
		extent[7] = pxy[1];
		extent[8] = pxy[0];
		extent[9] = pxy[1];

		v_pline( wind->graf.handle, 5, extent);		
		vsl_color( wind->graf.handle, BLACK);
		
				
/*		if ( !check_selected_entry( wicones, &wicones->entry[i]))
		{
			draw_icon( wind->graf.handle, &wicones->entry[i], FALSE, &dst_rect);
			draw_text( wind->graf.handle, xtext, ytext, BLACK, wicones->entry[i].name_shown);
		}
		else
		{
			pxy[1] = y + 1;
			pxy[3] = y + wicones->case_h - 2;
			pxy[0] = x + 1;
			pxy[2] = x + wicones->case_w - 2;

			extent[0] = pxy[0];
			extent[1] = pxy[1];
			extent[2] = pxy[0];
			extent[3] = pxy[3];
			extent[4] = pxy[2];
			extent[5] = pxy[3];
			extent[6] = pxy[2];
			extent[7] = pxy[1];
			extent[8] = pxy[0];
			extent[9] = pxy[1];

			v_pline( wind->graf.handle, 5, extent);
			
			draw_icon( wind->graf.handle, &wicones->entry[i], TRUE, &dst_rect);
		}
*/		
		
		if ( show_size)
		{
			if ( !S_ISDIR( wicones->entry[i].stat.st_mode))
			{
				int16 size_x_pos = x + (( wicones->case_w - 1 - get_text_width( wicones->entry[i].size))>> 1);
				draw_text( wind->graf.handle, size_x_pos, ytext + y_space + hcell, LBLACK, wicones->entry[i].size);
			}
		}

		x += wicones->case_w;
	}


	/*----------------------------------------------------------------------*/
	/* 						MAKE THE SECOND PASS							*/
	/* WE DRAW THE TEXT OF THE SELECTED'S ICON AND THE ICONS IN EDIT MODE	*/
	/*----------------------------------------------------------------------*/

	/* Draw the folder's content */
	x 							= xw + border_size + browser_frame_width;
	y 							= yw - ( ( int16)wind->ypos * wicones -> case_h);


	for ( i = 0 ; i < wicones->nbr_icons; i++)
	{
		if ( ( !show_non_image) && ( wicones->entry[i].type != ET_IMAGE)) 
			continue;
			
		if ( x + wicones->case_w > xw + ww)
		{
		 	x = xw + border_size + browser_frame_width;
			y += wicones -> case_h;				/* Jump to next line */
		}

		if( y < yw || y >= yw + hw)
		{
			x += wicones->case_w;
			continue;
		}

		ytext = y + thumbnail[thumbnail_size][1] + ( y_space << 1);

		if ( check_selected_entry( wicones, &wicones->entry[i]))			/* Look if the icon is selected */
		{
			pxy[1] = wicones->entry[i].txt_pos.y1 + yw;
			pxy[3] = wicones->entry[i].txt_pos.y2 + yw;

			if( wicones->edit)
			{
				int16 hc, xc;
				int16 txt_width = get_text_width( wicones->edit->top->buf);
				int16 width =  MAX( txt_width, 16);

				xtext = x + (( wicones->case_w - 1 - txt_width) >> 1);				
				
				pxy[0] = x + (( wicones->case_w - 1 - width) >> 1) - 2;
				pxy[2] = pxy[0] + width + 4;
		   		vsf_color( wind->graf.handle, LWHITE);
				v_bar( wind->graf.handle, pxy);

				extent[0] = pxy[0];
				extent[1] = pxy[1];
				extent[2] = pxy[0];
				extent[3] = pxy[3];
				extent[4] = pxy[2];
				extent[5] = pxy[3];
				extent[6] = pxy[2];
				extent[7] = pxy[1];
				extent[8] = pxy[0];
				extent[9] = pxy[1];

				v_pline( wind->graf.handle, 5, extent);

				draw_text( wind->graf.handle, xtext, ytext, BLACK, wicones->edit->top->buf);

				cursor_position( wind, &xc, &dum, &dum, &hc);

				pxy[0] += 3 + xc - xw;
				pxy[2] = pxy[0];
				pxy[1] = ytext + 1;
				pxy[3] = pxy[1] + hc - 2;

				v_pline( wind->graf.handle, 2, pxy);

				/* save the text position in the structure */
				wicones->entry[i].txt_pos.x1 = xtext - 2 - xw;
				wicones->entry[i].txt_pos.y1 = ytext - 1 - yw;
				wicones->entry[i].txt_pos.x2 = wicones->entry[i].txt_pos.x1 + width + 4;
				wicones->entry[i].txt_pos.y2 = wicones->entry[i].txt_pos.y1 + hcell + 2;
			}
			else
			{
				xtext = x + (( wicones->case_w - 1 - wicones->entry[i].icon_txt_w)>> 1);
			
				pxy[0] = xtext - 1;
				pxy[2] = pxy[0] + wicones->entry[i].icon_txt_w + 2;

				vsf_color( wind->graf.handle, LBLACK);
				v_bar( wind->graf.handle, pxy);

				draw_text( wind->graf.handle, xtext, ytext, WHITE, wicones->entry[i].name_shown);
			}
		}

		x += wicones->case_w;
	}


	wind -> ypos_max = lines;
	wind -> xpos_max = wicones->columns;

	WindSlider ( wind, VSLIDER);
}



/*==================================================================================*
 * void WinCatalog_filelist_redraw:													*
 *		Deselect all the icons, exit edition mode and Redraw the entire window...	* 
 *		usefull, for exemple, after a WM_FULLED message.							*	
 *		We delete also the timer if exist.											*		
 *----------------------------------------------------------------------------------*
 * input:																			*
 * 		--																			*
 *----------------------------------------------------------------------------------*
 * returns: 																		*
 *      --																			*
 *==================================================================================*/

void WinCatalog_filelist_redraw( void) 
{
	WINDICON *wicones = (WINDICON *)DataSearch( win_catalog, WD_ICON);

	if ( wicones->edit)
		exit_edit_mode( win_catalog, wicones->first_selected);

	if ( wicones->first_selected)
	{
		while ( wicones->first_selected)
			remove_selected_entry( wicones, wicones->first_selected);  
	} 

	win_catalog->ypos = 0;

	WindGet ( win_catalog, WF_WORKXYWH, &xw, &yw, &ww, &hw);
	draw_page( win_catalog, xw + browser_frame_width + border_size , yw, ww - ( browser_frame_width + border_size), hw);
	WindMakePreview( win_catalog);

}

/*==================================================================================*
 * void WinCatalog_set_thumbnails_size:												*
 *		Set new thumbnails's size, resize the window if necessary and redraw it		*		
 *----------------------------------------------------------------------------------*
 * input:																			*
 * 		win		-> Pointer to the target window										*
 *----------------------------------------------------------------------------------*
 * returns: 																		*
 *      --																			*
 *==================================================================================*/

void WinCatalog_set_thumbnails_size( WINDOW *win) 
{
	int16 x, y, w, h;
	WINDICON *wicones = (WINDICON *)DataSearch( win_catalog, WD_ICON);

	Entry *entry	= wicones->first_selected;

	WindGet ( win, WF_WORKXYWH, &xw, &yw, &ww, &hw);

	if ( wicones->edit)
		exit_edit_mode( win, wicones->first_selected);

	wicones -> case_w	= thumbnail[thumbnail_size][0] + ( x_space << 1);
	wicones -> case_h	= thumbnail[thumbnail_size][1] + hcell + y_space + y_space + y_space;;

	if ( show_size)
		wicones -> case_h += hcell + y_space;

	WindCalc( WC_BORDER, win, xw, yw, wicones->case_w + border_size + browser_frame_width, wicones->case_h, &x, &y, &w, &h);

	win -> h_min = h;
	win -> w_min = w;
	win -> h_u   = wicones -> case_h;
	win -> w_u	 = wicones -> case_w;
	win -> ypos  = 0;

	( void)scan_dir( win, wicones->directory);
	wicones->first_selected = entry;

	if( ( w > ww) && ( h > hw))
	{
		wind_set( win->handle, WF_CURRXYWH, x, y, win -> w_min, win -> h_min);
		WindGet ( win, WF_WORKXYWH, &xw, &yw, &ww, &hw);
	}
	else if ( w > ww)
	{
		WindGet( win, WF_CURRXYWH, &dum, &dum, &dum, &h);
		wind_set( win->handle, WF_CURRXYWH, x, y, w, h);
		WindGet ( win, WF_WORKXYWH, &xw, &yw, &ww, &hw); 
	}
	else if ( h > hw)
	{
		WindGet( win, WF_CURRXYWH, &dum, &dum, &w, &dum);
		wind_set( win->handle, WF_CURRXYWH, x, y, w, h);
		WindGet ( win, WF_WORKXYWH, &xw, &yw, &ww, &hw); 
	}

	draw_page( win, xw + browser_frame_width + border_size , yw + 1, ww - ( browser_frame_width + border_size), hw - 1);

	WindMakePreview( win);
}
