#include "general.h"
#include "prefs.h"
#include "pic_load.h"
#include "winimg.h"
#include "infobox.h"
#include "close_modal.h"
#include "pref_dialog.h"
#include "full_scr.h"
#include "file/sort.h"
#include "file/delete.h"
#include "catalog/catalog.h"
#include "catalog/catalog_entry.h"
#include "zedit/zedit.h"

/* Prototype */
static void MenuHandle( void);
static void Menu_about( void);
void Menu_open_image( void);
static void Menu_preference( void);
static void Menu_file_info( void);
static void Menu_close_win( void);
static void Menu_show_only_images( void);
static void Menu_sort_by_name( void);
static void Menu_sort_by_size( void);
static void Menu_sort_by_date( void);
static void Menu_thumb_size( void);
static void Menu_delete( void);
static void Menu_show_browser( void);
void MenuDesktop( void);

extern char	fullname[MAX_PATH+MAXNAMLEN];
extern void save_dialog( const char *fullfilename);


/*==================================================================================*
 * void MenuHandle:																	*
 *		Windom function to handle the TOS menubar statut ( selected or not).		*
 *----------------------------------------------------------------------------------*
 * input:																			*
 *		--																			*
 *----------------------------------------------------------------------------------*
 * returns: 																		*
 *      --																			*
 *==================================================================================*/
static void MenuHandle( void) 			 
{	
	MenuTnormal( NULL, evnt.buff[3], 1);
}


/*==================================================================================*
 * void Menu_about:																	*
 *		Show the about box.															*
 *----------------------------------------------------------------------------------*
 * input:																			*
 *		--																			*
 *----------------------------------------------------------------------------------*
 * returns: 																		*
 *      --																			*
 *==================================================================================*/
static void Menu_about( void)
{
	OBJECT *aboutbox = get_tree( ABOUT);

	WINDOW *winabout = FormCreate( aboutbox, NAME|MOVER|CLOSER, generic_form_event, get_string( ABOUT_TITLE), NULL, TRUE, FALSE);

	/* Set the window modal */
	WindSet( winabout, WF_BEVENT, BEVENT_MODAL, 0, 0, 0);
	/* New closer event function */
	EvntAttach( winabout, WM_CLOSED, close_modal);
	/* Disable the desktop menu */
	MenuDisable();
}



/*==================================================================================*
 * void Menu_open_image:															*
 *		Handle the "Open" option in the TOS menubar.								*
 *		It opens the file selector and after, it loads the wanted picture			*
 *----------------------------------------------------------------------------------*
 * input:																			*
 *		--																			*
 *----------------------------------------------------------------------------------*
 * returns: 																		*
 *      --																			*
 *==================================================================================*/
void Menu_open_image( void)
{
	static char path[MAX_PATH]	= "C:\\"; /* Fist usage : current directory */
    char 		name[MAXNAMLEN]	= ""; 
        
	if( FselInput( path, name, "*.*", get_string( LOAD_TITLE), NULL, NULL)) 
	{
		/* if a file is not selected, return */
		if( strcmp ( name, "") == 0)
			return;	

  		strcpy( fullname, path);
		strcat( fullname, name);
		WindView( fullname);
    }
}


/*==================================================================================*
 * void Menu_file_info:																*
 *		Handle the "Info" option in the TOS menubar.								*
 *		It opens the info's dialog boxe and show various information about the file	*
 *----------------------------------------------------------------------------------*
 * input:																			*
 *		--																			*
 *----------------------------------------------------------------------------------*
 * returns: 																		*
 *      --																			*
 *==================================================================================*/
static void Menu_file_info( void)
{
	infobox();
}


/*==================================================================================*
 * void Menu_preference:															*
 *		Handle the "Preference" option in the TOS menubar.							*
 *		It opens the preference's dialog boxe.										*
 *----------------------------------------------------------------------------------*
 * input:																			*
 *		--																			*
 *----------------------------------------------------------------------------------*
 * returns: 																		*
 *      --																			*
 *==================================================================================*/
static void Menu_preference( void)
{
	preference_dialog();
}



/*==================================================================================*
 * void Menu_file_save:																*
 *		Handle the "Save as" option in the TOS menubar.								*
 *		It opens the save's dialog boxe.											*
 *----------------------------------------------------------------------------------*
 * input:																			*
 *		--																			*
 *----------------------------------------------------------------------------------*
 * returns: 																		*
 *      --																			*
 *==================================================================================*/
static void Menu_file_save( void)
{
	WINDICON 	*wicones;
	WINDATA		*windata;
	
	if(( wicones = ( WINDICON *)DataSearch( wglb.front, WD_ICON)))	/* See if the target picture is in the catalog window */
	{
		if( !wicones->first_selected || wicones->first_selected->type != ET_IMAGE)
			return;
		
		if( wicones->edit)
			exit_edit_mode( win_catalog, wicones->first_selected);
		
		strcpy( fullname, wicones->directory);
		strcat( fullname, wicones->first_selected->name);
		save_dialog( fullname);
	}  
	else if(( windata = ( WINDATA *)DataSearch( wglb.front, WD_DATA)))		/* See if the picture is in a "normal" window */
	{ 
		save_dialog( windata->name);
	}
}


/*==================================================================================*
 * void Menu_close_win:																*
 *		Handle the "Close Window" option in the TOS menubar.						*
 *		It close the "focused" window.												*
 *----------------------------------------------------------------------------------*
 * input:																			*
 *		--																			*
 *----------------------------------------------------------------------------------*
 * returns: 																		*
 *      --																			*
 *==================================================================================*/
static void Menu_close_win( void)
{
	if( wglb.front) 
    {
		ApplWrite( app.id, WM_DESTROY, wglb.front->handle, 0, 0, 0, 0);
    }
}


/*==================================================================================*
 * void Menu_delete:																*
 *		Handle the "delete" option in the TOS menubar.								*
 *		This function delete all selected files and directories.					*
 *----------------------------------------------------------------------------------*
 * input:																			*
 *		--																			*
 *----------------------------------------------------------------------------------*
 * returns: 																		*
 *      --																			*
 *==================================================================================*/
static void Menu_delete( void)
{
	WINDICON *wicones;

	if ( !win_catalog)
		return;

	wicones = ( WINDICON *)DataSearch( win_catalog, WD_ICON);

	if( wicones->first_selected)
	{
		if( wicones->edit)
			exit_edit_mode( win_catalog, wicones->first_selected);

		delete_entry( win_catalog);
	}
}



/*==================================================================================*
 * void Menu_select_all:															*
 *		Handle the "Select All" option in the TOS menubar.							*
 *		This function mark as selected all file from the directory active.			*
 *----------------------------------------------------------------------------------*
 * input:																			*
 *		--																			*
 *----------------------------------------------------------------------------------*
 * returns: 																		*
 *      --																			*
 *==================================================================================*/
static void Menu_select_all( void)
{
	WINDICON *wicones;
	int16 i, x, y, w, h;

	if ( !win_catalog)
		return;

	wicones = ( WINDICON *)DataSearch( win_catalog, WD_ICON);

	if( wicones->edit)
		exit_edit_mode( win_catalog, wicones->first_selected);

	for ( i = 0 ; i < wicones->nbr_icons; i++)
	{
	        // only images?
	        if( show_only_images && wicones->entry[i].type != ET_IMAGE)
	                continue;
                
                // images, pdfs, folders only?
	        if( !show_unsupported && wicones->entry[i].type != ET_IMAGE && wicones->entry[i].type != ET_PDF && wicones->entry[i].type != ET_DIR)
	                continue;
	        
		if( !check_selected_entry( wicones, &wicones->entry[i]))
			add_selected_entry( wicones, &wicones->entry[i]);
	}

	WindGet( win_catalog, WF_WORKXYWH, &x, &y, &w, &h); 
	
	draw_page( win_catalog, x + browser_frame_width + border_size , y, w - ( browser_frame_width + border_size), h);
}

/*==================================================================================*
 * void Menu_show_only_images:							    *
 *	Handle the "Show Only Images" option in the TOS menubar.		    *
 *	This function shows only images; folders and unsupported files		    *
 *	are not shown. Unsupported files in from pref.c is preserved, though.	    *
 *----------------------------------------------------------------------------------*
 * input:																			*
 *		--																			*
 *----------------------------------------------------------------------------------*
 * returns: 																		*
 *      --																			*
 *==================================================================================*/
static void Menu_show_only_images( void)
{
	if ( !show_only_images)
	{
                MenuIcheck( NULL, MENU_BAR_SHOW_ONLY_IMAGES, 1);
		show_only_images = 1;
		
		if( win_catalog)
			WinCatalog_filelist_redraw();
	}
	else
	{
	        MenuIcheck( NULL, MENU_BAR_SHOW_ONLY_IMAGES, 0);
		show_only_images = 0;

		if( win_catalog)
			WinCatalog_filelist_redraw();

		WindMakePreview( win_catalog);
	}
}


/*==================================================================================*
 * void Menu_sort_by_name:															*
 *		Handle the "Sort by name" option in the TOS menubar.						*
 *		This function sorts the files in the catalog window in alphabetical order.	*
 *----------------------------------------------------------------------------------*
 * input:																			*
 *		--																			*
 *----------------------------------------------------------------------------------*
 * returns: 																		*
 *      --																			*
 *==================================================================================*/
static void Menu_sort_by_name( void)
{	
	if ( sortingmode != 0)
	{		
		WINDICON *wicones;

	    MenuIcheck( NULL, MENU_BAR_BY_NAME, 1);
		MenuIcheck( NULL, MENU_BAR_BY_SIZE, 0);
	    MenuIcheck( NULL, MENU_BAR_BY_DATE, 0);

		sortingmode = 0;

		if( !win_catalog)
			return;

		wicones = ( WINDICON *)DataSearch( win_catalog, WD_ICON);

		sort_entries( wicones);
		
		WinCatalog_filelist_redraw();
	}
}


/*==================================================================================*
 * void Menu_sort_by_size:															*
 *		Handle the "Sort by size" option in the TOS menubar.						*
 *		This function sorts the files by size in the catalog window.				*
 *----------------------------------------------------------------------------------*
 * input:																			*
 *		--																			*
 *----------------------------------------------------------------------------------*
 * returns: 																		*
 *      --																			*
 *==================================================================================*/
static void Menu_sort_by_size( void)
{	
	if ( sortingmode != 1)
	{		
		WINDICON *wicones;

		MenuIcheck( NULL, MENU_BAR_BY_SIZE, 1);
	    MenuIcheck( NULL, MENU_BAR_BY_DATE, 0);
	    MenuIcheck( NULL, MENU_BAR_BY_NAME, 0);

		sortingmode = 1;

		if( !win_catalog)
			return;

		wicones = ( WINDICON *)DataSearch( win_catalog, WD_ICON);

		sort_entries( wicones);
		
		WinCatalog_filelist_redraw();
	}
}


/*==================================================================================*
 * void Menu_sort_by_date:															*
 *		Handle the "Sort by date" option in the TOS menubar.						*
 *		This function sorts the files by date in the catalog window.				*
 *----------------------------------------------------------------------------------*
 * input:																			*
 *		--																			*
 *----------------------------------------------------------------------------------*
 * returns: 																		*
 *      --																			*
 *==================================================================================*/
static void Menu_sort_by_date( void)
{	
	if ( sortingmode != 2)
	{		
		WINDICON *wicones;

	    MenuIcheck( NULL, MENU_BAR_BY_NAME, 0);
		MenuIcheck( NULL, MENU_BAR_BY_SIZE, 0);
	    MenuIcheck( NULL, MENU_BAR_BY_DATE, 1);

		sortingmode = 2;

		if( !win_catalog)
			return;

		wicones = ( WINDICON *)DataSearch( win_catalog, WD_ICON);

		sort_entries( wicones);
		
		WinCatalog_filelist_redraw();
	}
}


/*==================================================================================*
 * void Menu_fullscreen:															*
 *		Show a picture in fullscreen mode.											*
 *----------------------------------------------------------------------------------*
 * input:																			*
 *		--																			*
 *----------------------------------------------------------------------------------*
 * returns: 																		*
 *      --																			*
 *==================================================================================*/
static void Menu_fullscreen( void)
{	
	WINDATA	*windata;

	if( !wglb.front)
		return;
		
	windata = ( WINDATA *)DataSearch( wglb.front, WD_DATA);
	
	if( !windata)
		return;

	show_fullscreen( windata);
}


/*==================================================================================*
 * void Menu_show_browser:															*
 *		Handle the "Show Browser" option in the TOS menubar.						*
 *		This function shows the folder's browser frame in the catalog window.		*
 *----------------------------------------------------------------------------------*
 * input:																			*
 *		--																			*
 *----------------------------------------------------------------------------------*
 * returns: 																		*
 *      --																			*
 *==================================================================================*/
static void Menu_show_browser( void)
{	
	if( browser_frame_width)
	{   
		old_browser_size = browser_frame_width;
		old_border_size = border_size;
		browser_frame_width = 0;
		border_size = 0;
		MenuIcheck( NULL, MENU_BAR_SHOW_BROWSER, 0);
	} 
	else
	{   					
		browser_frame_width = old_browser_size;
		border_size = old_border_size;				
		MenuIcheck( NULL, MENU_BAR_SHOW_BROWSER, 1);
	} 				   

	if( win_catalog)
	{
		int16	dum, xw, yw, hw, ww, x, y, w, h;
		WINDICON *wicones = (WINDICON *)DataSearch( win_catalog, WD_ICON); 

		WindGet ( win_catalog, WF_WORKXYWH, &x, &y, &w, &h); 

		WindCalc( WC_BORDER, win_catalog, 0, 0, wicones->case_w + border_size + browser_frame_width, wicones->case_h, &dum, &dum, &ww, &dum);

		win_catalog 	-> w_min 		= ww;

		if( win_catalog -> w_min > w)
		{   
			WindGet ( win_catalog, WF_CURRXYWH, &xw, &yw, &dum, &hw); 	
			wind_set( win_catalog -> handle, WF_CURRXYWH, xw, yw, ww, hw);
		}		
					
		win_catalog->ypos =  0;
				
		WinCatalog_Refresh( win_catalog);
						
		if ( wicones->nbr_icons && wicones->first_selected)
		{
			if( wicones->first_selected->txt_pos.y2 > h)
			{	
				while ( wicones->first_selected->txt_pos.y2 > h)
				{
					wicones->first_selected->txt_pos.y2 -= win_catalog->h_u;
					win_catalog->ypos++; 
				}  
			}
		}
		
		draw_page( win_catalog, x, y, w, h);
		WindMakePreview( win_catalog);				
	}
}


/*==================================================================================*
 * void Menu_show_preview_area:														*
 *		Handle the "Show Preview Area" option in the TOS menubar.					*
 *		This function shows the "preview" frame in the catalog window.				*
 *----------------------------------------------------------------------------------*
 * input:																			*
 *		--																			*
 *----------------------------------------------------------------------------------*
 * returns: 																		*
 *      --																			*
 *==================================================================================*/
/* static void Menu_show_preview_area( void)
{	
	if( preview_frame_height)
	{   
		old_preview_area_size = preview_frame_height;
		preview_frame_height = 0;
		MenuIcheck( NULL, MENU_BAR_SHOW_PREVIEW, 0);
	} 
	else
	{   					
		preview_frame_height = old_preview_area_size;			
		MenuIcheck( NULL, MENU_BAR_SHOW_PREVIEW, 1);
	}

	if( win_catalog && browser_frame_width)
	{
		int16	dum, x, y, h;

		WindGet ( win_catalog, WF_WORKXYWH, &x, &y, &dum, &h); 
		
		draw_page( win_catalog, x, y, browser_frame_width + border_size, h);
		WindMakePreview( win_catalog);				
	}
}
*/

/*==================================================================================*
 * void Menu_thumb_size:															*
 *		Set the thumbnails's size from 4:3 to 3:4 and from 3:4 to 4:3				*
 *----------------------------------------------------------------------------------*
 * input:																			*
 *		--																			*
 *----------------------------------------------------------------------------------*
 * returns: 																		*
 *      --																			*
 *==================================================================================*/
static void Menu_thumb_size( void)
{	
	if ( thumbnail_size > 3)
	{		
		if ( thumbnail_size == 4)	thumbnail_size = 0;
		else
		if ( thumbnail_size == 5)	thumbnail_size = 1;
		else
		if ( thumbnail_size == 6)	thumbnail_size = 2;
		else						thumbnail_size = 3;

		MenuIcheck( NULL, MENU_BAR_LARGE_THUMB, 1);
		MenuIcheck( NULL, MENU_BAR_LONG_THUMB, 0);
	}
	else
	{		
		if ( thumbnail_size == 0)	thumbnail_size = 4;
		else
		if ( thumbnail_size == 1)	thumbnail_size = 5;
		else
		if ( thumbnail_size == 2)	thumbnail_size = 6;
		else						thumbnail_size = 7;

		MenuIcheck( NULL, MENU_BAR_LONG_THUMB, 1);
		MenuIcheck( NULL, MENU_BAR_LARGE_THUMB, 0);
	}

	if( win_catalog)	
		WinCatalog_set_thumbnails_size( win_catalog); 

}


/*==================================================================================*
 * void Menu_show_help:															*
 *		Call StGuide and show "zview.hyp" with it.								*
 *----------------------------------------------------------------------------------*
 * input:																			*
 *		--																			*
 *----------------------------------------------------------------------------------*
 * returns: 																		*
 *      --																			*
 *==================================================================================*/
static void Menu_show_help( void)
{	
	strcpy( fullname, zview_path);
	strcat( fullname, "\\doc\\zview.hyp");

	CallStGuide( fullname);
}


/*==================================================================================*
 * void Menu_show_history:															*
 *		Call StGuide and show "history.txt" with it.								*
 *----------------------------------------------------------------------------------*
 * input:																			*
 *		--																			*
 *----------------------------------------------------------------------------------*
 * returns: 																		*
 *      --																			*
 *==================================================================================*/
static void Menu_show_history( void)
{	
	strcpy( fullname, zview_path);
	strcat( fullname, "\\doc\\history.txt");

	CallStGuide( fullname);
}


/*==================================================================================*
 * void Menu_sort_by_date:															*
 *		Bind the wanted function with the items in the TOS menubar.					*
 *----------------------------------------------------------------------------------*
 * input:																			*
 *		--																			*
 *----------------------------------------------------------------------------------*
 * returns: 																		*
 *      --																			*
 *==================================================================================*/
void MenuDesktop( void)
{
	MenuBar ( get_tree( MENU_BAR), 1);
	
	/* Attach Menu functions */
	ObjcAttach( OC_MENU, NULL, MENU_BAR_OPEN, BIND_FUNC, Menu_open_image);
	ObjcAttach( OC_MENU, NULL, MENU_BAR_ABOUT, BIND_FUNC, Menu_about);
	ObjcAttach( OC_MENU, NULL, MENU_BAR_QUIT, BIND_FUNC, applexit);
	ObjcAttach( OC_MENU, NULL, MENU_BAR_BROWSER, BIND_FUNC, WinCatalog);	
	ObjcAttach( OC_MENU, NULL, MENU_BAR_CLOSE, BIND_FUNC, Menu_close_win);
	ObjcAttach( OC_MENU, NULL, MENU_BAR_SHOW_ONLY_IMAGES, BIND_FUNC, Menu_show_only_images);
	ObjcAttach( OC_MENU, NULL, MENU_BAR_BY_NAME, BIND_FUNC, Menu_sort_by_name);
	ObjcAttach( OC_MENU, NULL, MENU_BAR_BY_SIZE, BIND_FUNC, Menu_sort_by_size);
	ObjcAttach( OC_MENU, NULL, MENU_BAR_BY_DATE, BIND_FUNC, Menu_sort_by_date);
	ObjcAttach( OC_MENU, NULL, MENU_BAR_SHOW_BROWSER, BIND_FUNC, Menu_show_browser);
	ObjcAttach( OC_MENU, NULL, MENU_BAR_SELECT_ALL, BIND_FUNC, Menu_select_all);
	ObjcAttach( OC_MENU, NULL, MENU_BAR_LARGE_THUMB, BIND_FUNC, Menu_thumb_size);
	ObjcAttach( OC_MENU, NULL, MENU_BAR_LONG_THUMB, BIND_FUNC, Menu_thumb_size);
	ObjcAttach( OC_MENU, NULL, MENU_BAR_INFORMATION, BIND_FUNC, Menu_file_info);
	ObjcAttach( OC_MENU, NULL, MENU_BAR_PREFERENCE, BIND_FUNC, Menu_preference);
	ObjcAttach( OC_MENU, NULL, MENU_BAR_SAVE, BIND_FUNC, Menu_file_save);
	ObjcAttach( OC_MENU, NULL, MENU_BAR_DELETE, BIND_FUNC, Menu_delete);
	ObjcAttach( OC_MENU, NULL, MENU_BAR_HISTORY, BIND_FUNC, Menu_show_history);
	ObjcAttach( OC_MENU, NULL, MENU_BAR_GUIDE, BIND_FUNC, Menu_show_help);
	ObjcAttach( OC_MENU, NULL, MENU_BAR_SHOW_FULLSCREEN, BIND_FUNC, Menu_fullscreen);

	if( browser_frame_width)
		MenuIcheck( NULL, MENU_BAR_SHOW_BROWSER, 1);

//	if( preview_frame_height)
//		MenuIcheck( NULL, MENU_BAR_SHOW_PREVIEW, 1);

	if ( show_only_images)
	    MenuIcheck( NULL, MENU_BAR_SHOW_ONLY_IMAGES, 1);

	if ( thumbnail_size > 3)
		MenuIcheck( NULL, MENU_BAR_LONG_THUMB, 1);
	else
		MenuIcheck( NULL, MENU_BAR_LARGE_THUMB, 1);

	switch ( sortingmode)
	{
		case 0:
			MenuIcheck( NULL, MENU_BAR_BY_NAME, 1);
			break;	
			
		case 1:
			MenuIcheck( NULL, MENU_BAR_BY_SIZE, 1);
			break;

		case 2:
			MenuIcheck( NULL, MENU_BAR_BY_DATE, 1);
			break;
	}


	EvntAttach( NULL, MN_SELECTED, MenuHandle);
}
