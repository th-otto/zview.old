/*----------------------------------------------------------------------*/
/*							Rename functions							*/
/*----------------------------------------------------------------------*/
#include "../general.h"
#include "../catalog/catalog_icons.h"
#include "file.h"

/* prototype */
boolean rename_entry( WINDOW *win, const char *new_name);


/*==================================================================================*
 * rename_entry:																	*
 *		rename a file or directory. 												*
 *----------------------------------------------------------------------------------*
 *	returns: TRUE = success, FALSE = rename not complete.							*
 *----------------------------------------------------------------------------------*
 * 	old_name:	Pointer to the current file/directory name.							*		
 * 	new_name:	Pointer to the new file/directory name.								*
 *==================================================================================*/

boolean rename_entry( WINDOW *win, const char *new_name)
{
	int16 i;
	WINDICON *wicones = (WINDICON *)DataSearch( win, WD_ICON); 
	
	if( strcmp( wicones->first_selected->name, new_name) == 0)		/* if name is unchanged, do nothing */
		return FALSE;

	if( strchr( new_name, '/') || strchr( new_name, '\\'))			/* disallow attempts to use rename to move files */
	{	
		errshow( wicones->first_selected->name, INVAL);
		return FALSE;
	}

	for ( i = 0 ; i < wicones->nbr_icons ; i++)					
		if ( !strcmp( wicones->entry[i].name, new_name))
		{	
			errshow( "", E_NAMEEXIST);
			return FALSE;
		}

	
	if ( rename( wicones->first_selected->name, new_name) != 0)
	{
		errshow( wicones->first_selected->name, errno);
		return FALSE;
	}

	strcpy( wicones->first_selected->name, new_name);

	set_entry_icon( wicones->first_selected);

	return TRUE;
}
