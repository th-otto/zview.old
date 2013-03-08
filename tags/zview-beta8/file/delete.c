#include "../general.h"
#include "../prefs.h"
#include "../ztext.h"
#include "../close_modal.h"
#include "../catalog/catalog.h"
#include "../catalog/catalog_entry.h"
#include "../catalog/catalog_mini_entry.h"
#include "file.h"
#include "count.h"


/* local variable */
static char		size_to_delete[16];
static uint32	size_deleted;
static uint32	item_deleted;
static uint32	item_to_delete;

static OBJECT *save_dialog; 

/* prototype */
boolean delete_file( const char *name, struct stat *file_stat);
boolean delete_dir( WINDOW *win, const char *path);
void delete_entry( WINDOW *win);
void delete_progress( WINDOW *win);


/*==================================================================================*
 * delete_file:																		*
 *		Delete a plain file. This is capable of removing anything that is not a		*
 *		directory.																	*
 *----------------------------------------------------------------------------------*
 * returns: TRUE = success, FALSE = error											*
 *==================================================================================*/

boolean delete_file( const char *name, struct stat *file_stat)
{
	if ( unlink( name) != 0)
	{   
		/* If possible, change the file permission for delete it */
		( void)chmod( name, S_IRWXU); 
		return unlink( name) == 0 ? TRUE : FALSE;
	}
	else
		return ( TRUE);
}


/*==================================================================================*
 * delete_dir:																		*
 *		Delete a directory.															*
 *----------------------------------------------------------------------------------*
 * returns: TRUE = success, FALSE = error											*
 *==================================================================================*/

boolean delete_dir( WINDOW *win, const char *path)
{
	char 			old_dir[MAX_PATH];
	DIR	 			*dir;
	struct dirent	*de;
	struct stat		file_stat;
	
	if ( rmdir( path) == 0)
	   return TRUE;										
	else
	{
		if ( dir_cd( path, old_dir, ( int16)( sizeof old_dir)))
		{
			if (( dir = opendir( ".")) != NULL)
			{
				while(( de = readdir( dir)) != NULL)
				{
					if (( strcmp( de->d_name, ".") == 0) || ( strcmp( de->d_name, "..") == 0))
						continue;	

					if ( lstat( de->d_name, &file_stat) != 0)
						break;
					
					if ( S_ISDIR( file_stat.st_mode))   
					{
						if ( delete_dir( win, de->d_name) == FALSE)
							break;
						else
						{
							item_deleted++;
							delete_progress( win);
						}   							
					}
					else
					{
						if ( delete_file( de->d_name, &file_stat) == FALSE)
							break;
						else
						{
							item_deleted++;
							size_deleted += file_stat.st_size;
							delete_progress( win);
						}   
					}
				}
				closedir( dir);
			}
			dir_cd( old_dir, NULL, 0);
		}

		return rmdir( path) == 0 ? TRUE : FALSE;
	}
}

void delete_progress( WINDOW *win)
{
	static int32 bar_width 	= 0L;
	int32	new_bar_width 	= ( ( item_deleted * 100L) / item_to_delete) * 3;
	int16 	pxy[4];			
	char	tmp[16];
	GRECT	rwin, raes;
	W_FORM  *form 			= ( W_FORM *)DataSearch( win, WD_WFRM);

	size_to_text( tmp, ( float)size_deleted);

	sprintf( save_dialog[OPERATION_DIAL_INFO].ob_spec.tedinfo->te_ptext, get_string( DELETE_INFO), item_to_delete - item_deleted);
	sprintf( save_dialog[OPERATION_DIAL_PROGRESS].ob_spec.tedinfo->te_ptext, get_string( PROGRESS_TXT), tmp, size_to_delete);

	ObjcDraw( OC_FORM, win, OPERATION_DIAL_INFO, 0);
	ObjcDraw( OC_FORM, win, OPERATION_DIAL_PROGRESS, 0);	

	if (( bar_width == new_bar_width) || !new_bar_width)
		return;

	WindGet( win, WF_WORKXYWH, &rwin.g_x, &rwin.g_y, &rwin.g_w, &rwin.g_h);

	bar_width = new_bar_width;

	pxy[0] = rwin.g_x + form->root[OPERATION_DIAL_BAR].ob_x;
	pxy[1] = rwin.g_y + form->root[OPERATION_DIAL_BAR].ob_y;
	pxy[2] = pxy[0] + bar_width;
	pxy[3] = pxy[1] + form->root[OPERATION_DIAL_BAR].ob_height;

	while( !wind_update( BEG_UPDATE));
	graf_mouse( M_OFF, 0L);
	wind_get( win->handle, WF_FIRSTXYWH, &raes.g_x, &raes.g_y, &raes.g_w, &raes.g_h);

	while ( raes.g_w && raes.g_h) 
	{
		if( rc_intersect( &rwin, &raes)) 
		{
			rc_clip_on( win->graf.handle, &raes);

			v_bar( win->graf.handle, pxy);

			rc_clip_off( win->graf.handle);
		}
	
		wind_get( win->handle, WF_NEXTXYWH, &raes.g_x, &raes.g_y, &raes.g_w, &raes.g_h);
	}	
	
	graf_mouse( M_ON, 0L);

	wind_update( END_UPDATE);
}



void delete_function( WINDOW *win)
{
	WINDICON 	*wicones 		= ( WINDICON *)DataSearch( win_catalog, WD_ICON);	
	Mini_Entry	*mini_entry;
	fileinfo	file;
	int16 		x, y, w, h, deleted_entry = 0;
	
	switch( evnt.buff[4])
	{	   
		case OPERATION_DIAL_CANCEL:
			ObjcChange( OC_FORM, win, OPERATION_DIAL_CANCEL, ~SELECTED, TRUE);
			ApplWrite( app.id, WM_CLOSED, win->handle, 0, 0, 0, 0);
			break;

		case OPERATION_DIAL_OK:
			ObjcChange( OC_FORM, win, OPERATION_DIAL_OK, ~SELECTED, TRUE);
			strcpy( save_dialog[OPERATION_DIAL_INFO].ob_spec.tedinfo->te_ptext, get_string( CALCUL));

			vsf_color( win->graf.handle, RED);
	
			if( !count_files( wicones, &file))
			{
				ApplWrite( app.id, WM_CLOSED, win->handle, 0, 0, 0, 0);
				errshow( "", errno);
				return;
			}

			item_to_delete  = file.item;

			size_deleted = 0L;
			item_deleted = 0L;

			size_to_text( size_to_delete, ( float)file.size);

			delete_progress( win);

			while ( wicones->first_selected)
			{
				if ( S_ISDIR( wicones->first_selected->stat.st_mode))
				{		
					if ( !delete_dir( win, wicones->first_selected->name))
					{   
				  		errshow( wicones->first_selected->name, errno);
						break;
					}
					else
					{
						deleted_entry++;
						item_deleted++;
				
						delete_progress( win);

						remove_selected_entry( wicones, wicones->first_selected);
   
						/* the mini_entry returned is the mini_entry parent of the directory deleted
						   because we give the current directory ( wicones->directory) as parameter */					   
						if ( ( mini_entry = find_mini_entry_by_path( wicones, wicones->directory)))
						{   																	
							int16 old_state = mini_entry->state;

							delete_mini_entry_child( mini_entry);											
							scan_mini_dir( win_catalog, mini_entry);
							check_mini_dir( mini_entry->nbr_child, mini_entry->child);

							mini_entry->state = old_state;

							if( mini_entry->state == OFF)
								delete_mini_entry_child( mini_entry);

							if ( browser_frame_width)
							{
								WindGet( win_catalog, WF_WORKXYWH, &x, &y, &w, &h); 
								draw_page( win_catalog, x, y + mini_entry->arrow_position.y1 - 1, browser_frame_width, h - mini_entry->arrow_position.y1);	
							}
						}
					}	
				}
				else 
				{   							
					if ( !delete_file( wicones->first_selected->name, &wicones->first_selected->stat))
					{
						errshow( wicones->first_selected->name, errno);
						break;
					}
					else	
					{
						deleted_entry++;
						item_deleted++;
						size_deleted += wicones->first_selected->stat.st_size;
				
						delete_progress( win);
						remove_selected_entry( wicones, wicones->first_selected);
					}
				}							
			}
			ObjcDraw( OC_FORM, win, OPERATION_DIAL_INFO, FALSE);

			close_modal( win);	

			if ( deleted_entry)
			{
				( void)scan_dir( win_catalog, wicones->directory);
				WinCatalog_filelist_redraw(); 
				ObjcDraw( OC_TOOLBAR, win_catalog, TOOLBAR_DELETE, 1);
				ObjcDraw( OC_TOOLBAR, win_catalog, TOOLBAR_INFO, 1);
				ObjcDraw( OC_TOOLBAR, win_catalog, TOOLBAR_SAVE, 1);
				menu_ienable( get_tree( MENU_BAR), MENU_BAR_DELETE, 0);
				menu_ienable( get_tree( MENU_BAR), MENU_BAR_INFORMATION, 0);
				menu_ienable( get_tree( MENU_BAR), MENU_BAR_SAVE, 0);					
			}
			
			break;

		default:
			break;
	}
}



/*==================================================================================*
 * void delete_entry:																*
 *		Delete all the selected entries in the WINDOW win.							*
 *		A modal dialog box is shown for approve it.									*
 *----------------------------------------------------------------------------------*
 * input:																			*
 *		win 	-> the window's adress to handle.									*
 *----------------------------------------------------------------------------------*
 * return:	 																		*
 *      entries deleted																*
 *==================================================================================*/

void delete_entry( WINDOW *win)
{
	WINDOW 		*win_save_dialog;

	save_dialog = get_tree( OPERATION_DIAL);

	zstrncpy( save_dialog[OPERATION_DIAL_INFO].ob_spec.tedinfo->te_ptext, get_string( DELETE_ASK), 48);
	strcpy( save_dialog[OPERATION_DIAL_PROGRESS].ob_spec.tedinfo->te_ptext, "");

	win_save_dialog = FormCreate( save_dialog, NAME|MOVER, delete_function, get_string( DELETE_TITLE), NULL, TRUE, FALSE);

	WindSet( win_save_dialog, WF_BEVENT, BEVENT_MODAL, 0, 0, 0);

	EvntAttach( win_save_dialog, WM_CLOSED, close_modal);

	MenuDisable();
}

