#include "general.h"
#include "prefs.h"
#include "plugins.h"
#include "close_modal.h"
#include "ztext.h"
#include "pic_load.h"
#include "catalog/catalog.h"
#include "file/file.h"


void 	CDECL ( *encoder_quit)	 ( IMGINFO);
boolean	CDECL ( *encoder_write)	 ( IMGINFO, uint8 *);
boolean	CDECL ( *encoder_init)	 ( const char *, IMGINFO);
void 	CDECL ( *set_jpg_option) ( int16 set_quality, J_COLOR_SPACE set_color_space, boolean set_progressive) = NULL;
void 	CDECL ( *set_tiff_option)( int16 set_quality, uint16 set_encode_compression) = NULL;

/* extern function */
extern int16 pic_save( const char *in_file, const char *out_file);
extern void jpg_option_dialog( char *source_file);
extern void tiff_option_dialog( void);

int16 encoder_plugins_nbr = 0;
LDG *encoder[100];


/* Prototype */
void save_dialog( const char *fullfilename);


/* local variable */
static int last_choice = -1;
static OBJECT *save_dialog_content = NULL;

char source_file[MAXNAMLEN];


static boolean encoder_plugin_setup( WINDOW *win, int encoder_selected)
{
	encoder_init 		= ldg_find( "encoder_init", encoder[encoder_selected]);
	encoder_write 		= ldg_find( "encoder_write", encoder[encoder_selected]);
	encoder_quit 		= ldg_find( "encoder_quit", encoder[encoder_selected]);

	if ( !encoder_init || !encoder_write || !encoder_quit)
	{
		errshow( encoder[encoder_selected]->infos, ldg_error());
		return FALSE;
	}

	set_jpg_option 	= NULL;	
	set_tiff_option = NULL;
	
	if( strncmp( encoder[encoder_selected]->infos, "JPG", 3) == 0)
		set_jpg_option 	= ldg_find( "set_jpg_option", encoder[encoder_selected]); 
	else if( strncmp( encoder[encoder_selected]->infos, "TIF", 3) == 0)
		set_tiff_option = ldg_find( "set_tiff_option", encoder[encoder_selected]); 
	else 
		ObjcChange( OC_FORM, win, SAVE_DIAL_OPTIONS, DISABLED, 0);
		
	zstrncpy( save_dialog_content[SAVE_DIAL_FORMAT].ob_spec.free_string, encoder[encoder_selected]->infos, 4);

	if( set_jpg_option != NULL || set_tiff_option != NULL)
		ObjcChange( OC_FORM, win, SAVE_DIAL_OPTIONS, NORMAL, 0);

   	ObjcDraw( OC_FORM, win, SAVE_DIAL_FORMAT, 1);
   	ObjcDraw( OC_FORM, win, SAVE_DIAL_OPTIONS, 1);

	return TRUE;
}


static void format_popup( WINDOW *win, int obj_index) 
{
	char items[100][4];
	char *items_ptr[100];
	int16 i, x, y;
	int choice;

	for( i = 0; i < encoder_plugins_nbr; i++)
	{	
		zstrncpy( items[i], encoder[i]->infos, 4);
		items_ptr[i] = items[i];
	}

	objc_offset( FORM(win), obj_index, &x, &y);

	choice = MenuPopUp ( items_ptr, x, y, encoder_plugins_nbr, -1, last_choice, P_LIST + P_WNDW + P_CHCK);

	if( choice < 1)
		choice = 1;

	if( last_choice != choice)
		encoder_plugin_setup( win, choice - 1);

	last_choice = choice;

	if( last_choice)
		ObjcChange( OC_FORM, win, SAVE_DIAL_SAVE, NORMAL, TRUE);
	else		
		ObjcChange( OC_FORM, win, SAVE_DIAL_SAVE, DISABLED, TRUE);	
}


static void save_dialog_event( WINDOW *win) 
{
	char target_file[MAX_PATH+MAXNAMLEN];
	char target_file_path[MAX_PATH];
	char target_file_name[MAXNAMLEN];
	char extention[3];
	char file_mask[5];
	int16 object = evnt.buff[4], source_file_len, source_name_len, source_path_len, i;

	switch( object)
	{	   
		case SAVE_DIAL_FORMAT:
			format_popup( win, evnt.buff[4]);
			ObjcChange( OC_FORM, win, evnt.buff[4], NORMAL, TRUE);
			break;

		case SAVE_DIAL_OPTIONS:
			if( strncmp( encoder[last_choice-1]->infos, "JPG", 3) == 0)
			{
				ObjcChange( OC_FORM, win, object, NORMAL, TRUE);
				jpg_option_dialog( source_file);
			}
			else if( strncmp( encoder[last_choice-1]->infos, "TIF", 3) == 0)
			{
				ObjcChange( OC_FORM, win, object, NORMAL, TRUE);
				tiff_option_dialog();
			}
			break;

		case SAVE_DIAL_SAVE:
			source_file_len = ( int16)strlen( source_file);
			
			source_path_len = source_file_len;	
		
			while(( source_file[source_path_len] != '/') && ( source_file[source_path_len] != '\\'))
				source_path_len--;

			source_name_len = ( source_file_len - source_path_len);

			strncpy( target_file_path, source_file, source_path_len);

			target_file_path[source_path_len] = '\0';	

			source_path_len++;
							
			for ( i = 0; i < source_name_len; i++)	
				target_file_name[i] = source_file[source_path_len++];
	
			target_file_name[i] = '\0';

			/* copy the source's name in the target's name with the new extention for exemple ( "toto.gif" to "toto.jpg") */
			zstrncpy( extention, encoder[last_choice - 1]->infos, 4);
			str2lower( extention);
			strcpy ( target_file_name + strlen( target_file_name) - 3, extention);
				
			/* The file mask ( for exemple *.jpg) */
			strcpy( file_mask, "*.");
			strcat( file_mask, extention);

			ObjcChange( OC_FORM, win, evnt.buff[4], NORMAL, FALSE);
			close_modal( win);			

			if( FselInput( target_file_path, target_file_name, file_mask, get_string( SAVE_TITLE), NULL, NULL)) 
			{
				strcpy( target_file, target_file_path);

				/* workaround for a bug in tos fileselector, when we navigate through this one, the file name is deleted */
				if( strlen( target_file_name) < 5) 
				{
					source_path_len -= i;

					for ( i = 0; i < source_name_len; i++)	
						target_file_name[i] = source_file[source_path_len++];
	
						target_file_name[i] = '\0';
				}

				strcat( target_file, target_file_name);

				/* for be sure that the extention match with the encoder */
				strcpy ( target_file + strlen( target_file) - 4, file_mask + 1);

				/* with fat filesystem, the file selector return upper case name, some the strcmp() function doens't work */
				str2lower( source_file);
				str2lower( target_file);

				/* the source can't be the same that the destination */
				if( strcmp( source_file, target_file) == 0)
				{
					errshow( NULL, SOURCE_TARGET_SAME);
					break;
				}

				if( !pic_save( source_file, target_file))
					break;

				/* if the new file is added in the same dir that win_catalog, we refresh it */
				if( win_catalog)
				{
					WINDICON *wicones = (WINDICON *)DataSearch( win_catalog, WD_ICON);
					
					//strcat( target_file_path, "\\");
	
					if( strcmpi( target_file_path, wicones->directory) == 0)
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
				}
		    }	
			break;

		case SAVE_DIAL_ABORT:
			ObjcChange( OC_FORM, win, evnt.buff[4], NORMAL, FALSE);
			ApplWrite( app.id, WM_CLOSED, win->handle, 0, 0, 0, 0);
			break;
	}
}

/*==================================================================================*
 * void infobox:																	*
 *		Show the infobox for a image or a file.										*
 *----------------------------------------------------------------------------------*
 * input:																			*
 *		--																			*
 *----------------------------------------------------------------------------------*
 * returns: 																		*
 *      --																			*
 *==================================================================================*/
void save_dialog( const char *fullfilename)
{
	WINDOW 		*win_save_dialog;

	if( !encoder_plugins_nbr)
	{ 
		int16 i;

		for( i = 0; i < plugins_nbr; i++)
		{
			if( ldg_find( "encoder_init", codecs[i]))
			{
				encoder[encoder_plugins_nbr] = codecs[i];
				encoder_plugins_nbr++;
			}
		}
		
		if( !encoder_plugins_nbr)
			return;
	}

	strcpy( source_file, fullfilename);

	save_dialog_content = get_tree( SAVE_DIAL);

	win_save_dialog = FormCreate( save_dialog_content, NAME|MOVER, save_dialog_event, get_string( SAVE_TITLE), NULL, TRUE, FALSE);

	/* Make this window modal */
	WindSet( win_save_dialog, WF_BEVENT, BEVENT_MODAL, 0, 0, 0);

	EvntAttach( win_save_dialog, WM_CLOSED, close_modal);

	MenuDisable();

}

