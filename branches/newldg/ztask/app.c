#include "general.h"
#include "gmem.h"
#include "string.h"

app_data *root = NULL, *selected = NULL, *old_selected = NULL;
int16 app_nbr = 0;


int16 find_top_app_by_window( void)
{
	int16 win_handle, next_win_handle, aes_id, dum;

	if( mt_wind_get( 0, WF_TOP, &win_handle, &aes_id, &next_win_handle, &dum, app.aes_global) == 0)
		return -1;

	return aes_id;
}



/*==================================================================================*
 * app_data_attach:																	*
 *		attach a entry in the global application list.								*
 *----------------------------------------------------------------------------------*
 * input:																			*
 *		name:				The application name.									*
 *		name_shown:			The name to be shown in the taskbar.					*
 *		id:					The AES ID of the application.							*
 *		name_shown_width:	The text width of 'name_shown' in pixel.				* 
 *----------------------------------------------------------------------------------*
 * returns: 																		*
 *      TRUE if all is ok else FALSE												*
 *==================================================================================*/

int16 app_data_attach( char *name, char *name_shown, int16 id, int16 pid, int16 name_shown_width) 
{
	app_data *new = ( app_data *)gmalloc( sizeof(app_data));

	if( !new) 
		return FALSE;

	new -> id   			= id;
	new -> pid   			= pid;	
	new -> next 			= root;
	new -> name_shown_width = name_shown_width;

	strcpy( new -> name, name);
	strcpy( new -> name_shown, name_shown);

	root = new;

	app_nbr++;
	
	return TRUE;
}


/*==================================================================================*
 * app_data_find_parent:															*
 *		find the parent( previous) entry in the application list.					*
 *----------------------------------------------------------------------------------*
 * input:																			*
 *		child:			the function returns the parent of this entry.				*
 *----------------------------------------------------------------------------------*
 * returns: 																		*
 *      the parent entry else the root entry.										*
 *==================================================================================*/
app_data *app_data_find_parent( app_data *child) 
{
	app_data *scan = root;
	
	while( scan)
	{	
		if( scan->next != child)
		{
			scan = scan->next;
			continue;
		}	

		break;
	}	
	
	return( scan);
}


/*==================================================================================*
 * app_data_delete:																	*
 *		Delete an entry in the application list.									*
 *----------------------------------------------------------------------------------*
 * input:																			*
 *		id:		the AES id of the entry to delete.									*
 *----------------------------------------------------------------------------------*
 * returns: 																		*
 *      --																			*
 *==================================================================================*/
void app_data_delete( int16 id) 
{
	app_data *scan = root, *parent;
	
	while( scan)
	{
		if( scan->id != id)
		{
			scan = scan->next;
			continue;
		}	
		
		parent = app_data_find_parent( scan);
		
		if( parent == NULL)
			root = scan->next;
		else		
			parent->next = scan->next;

		gfree( scan); 
		app_nbr--;
		break;
	}	
}


/*==================================================================================*
 * app_data_find:																	*
 *		Find an entry in the application list.										*
 *----------------------------------------------------------------------------------*
 * input:																			*
 *		id:		the AES id of the entry to find.									*
 *----------------------------------------------------------------------------------*
 * returns: 																		*
 *      the entry or NULL if not found.												*
 *==================================================================================*/
app_data *app_data_find( int16 id) 
{
	app_data *scan = root, *result = NULL;
	
	while( scan)
	{	
		if( scan->id != id)
		{
			scan = scan->next;
			continue;
		}	
		
		result = scan;
		break;
	}	
	
	return( result);
}


/*==================================================================================*
 * app_have_wind:																	*
 *		Check if an application owns at least one window.							*
 *----------------------------------------------------------------------------------*
 * input:																			*
 *		id:		the AES id of the entry to check.									*
 *----------------------------------------------------------------------------------*
 * returns: 																		*
 *      1 if TRUE else 0.															*
 *==================================================================================*/
int16 app_have_wind( int16 id) 
{
	int16 result = 0, dum, aes_win, next = -1, status, aes_id;

	if( mt_wind_get( 0, WF_OWNER, &aes_id, &status, &aes_win, &dum, app.aes_global) == 0)
		return result;
	
	while( aes_win)
	{
		if( mt_wind_get( aes_win, WF_OWNER, &aes_id, &status, &next, &dum, app.aes_global) == 0)
			break;

		if( ( aes_id != id) || ( status == 0))
		{
			aes_win = next;
			continue;
		}	

		result = 1;
		break;
	}
	
	return result;
}



/*==================================================================================*
 * app_data_search:																	*
 *		Make a list of all the loaded application and add it in the global			*
 *		application list if necessary.												*
 *----------------------------------------------------------------------------------*
 * input:																			*
 *		--																			*
 *----------------------------------------------------------------------------------*
 * returns: 																		*
 *      --																			*
 *==================================================================================*/
void app_data_search( void) 
{
	char name[9], name_shown[MAXNAMLEN];
	app_data *scan = root, *tmp = NULL;
	int16 lenght, i, id, pid, type, count = 0, ap_cout, name_shown_width, old_id_by_win, old_id_by_menu;
	static int16 id_by_win = 0, id_by_menu = 0;
	
	for( i = 0; mt_appl_search( i, name, &type, &id, app.aes_global); i = 1)
	{
		// Is an accessory?
		if( show_acc == FALSE && ( type & APP_ACCESSORY))
			continue;

		// Is a system app or zTask itself?
		if(( type & APP_SYSTEM) || ( id == _AESapid))
			continue;

		count++;
		
		// Is already listed?
		if( app_data_find( id))
			continue;

		if( mt_appl_control( id, APC_INFO, &ap_cout, app.aes_global) == 0)
		{
			count--;
			continue;
		}

		// if it's not a accessory, if the application doesn't own a menu bar and if it doesn't own a window, we skip this id. 
		if(( type != APP_ACCESSORY) && !( ap_cout & APCI_HASMBAR) && !app_have_wind( id)) 
		{
			count--;
			continue;
		}

		// We setup the name to show in the case 
		mt_appl_search( -id, name_shown, &type, &pid, app.aes_global);

		trim_start(name_shown);
		trim_end( name_shown, strlen( name_shown) - 1);	

		if( strcmp( "XaSYS", name_shown) == 0)
		{
			count--;
			continue;
		}
		
		lenght = strlen( name_shown);
				
		if( lenght > 4)
		{
			char *extention = name_shown + lenght - 4;
		
			// If the application's name has an extention ( like "xxx.app"), we remove it
			if( *extention == '.')
				*extention = '\0';
		}

		name_shown_width = get_text_width( name_shown);

		if( name_shown_width > ( app_width - 6))
			name_shown_width = name_shorter( app_width - 6, name_shown);
		
		app_data_attach( name, name_shown, id, pid, name_shown_width);
	}	

	if( count != app_nbr)
	{
		while( scan)
		{
			if( mt_appl_find( scan->name, app.aes_global) == -1)
			{
				id	= scan->id;
				scan = scan->next;
				app_data_delete( id); 
				continue;
			}
		
			scan = scan->next;
		}
	}

	old_id_by_win = id_by_win;
	old_id_by_menu = id_by_menu;

	id_by_win = find_top_app_by_window();
	id_by_menu = mt_menu_bar( NULL, MENU_INQUIRE, app.aes_global);

	id = 0;

	if(( old_id_by_win != id_by_win) && ( old_id_by_menu == id_by_menu))
		id = id_by_win;
	else if(( old_id_by_win == id_by_win) && ( old_id_by_menu != id_by_menu))
		id = id_by_menu;
	else if(( old_id_by_win != id_by_win) && ( old_id_by_menu != id_by_menu))
		id = id_by_menu;

	if( id)
	{
		tmp = app_data_find( id);
		
		if( tmp != selected)
		{
			old_selected = selected;
			selected = tmp;
		}
	}			
}

