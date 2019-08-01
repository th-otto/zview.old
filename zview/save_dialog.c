#include "general.h"
#include "prefs.h"
#include "plugins.h"
#include "close_modal.h"
#include "ztext.h"
#include "pic_load.h"
#include "catalog/catalog.h"
#include "file/file.h"
#include "pic_save.h"
#include "menu.h"
#include "plugins/common/zvplugin.h"


static int16 encoder_plugins_nbr = 0;
static CODEC *encoder[MAX_CODECS];


/* local variable */
static int last_choice = -1;
static OBJECT *save_dialog_content = NULL;

char source_file[MAXNAMLEN];


static boolean encoder_plugin_setup( WINDOW *win, int encoder_selected)
{
	boolean have_options = FALSE;

	curr_output_plugin = NULL;
	ldg_funcs.set_jpg_option = NULL;	
	ldg_funcs.set_tiff_option = NULL;

	switch (encoder[encoder_selected]->type)
	{
	case CODEC_LDG:
		{
			LDG *ldg;

			ldg = encoder[encoder_selected]->c.ldg;
			ldg_funcs.encoder_init 		= ldg_find( "encoder_init", ldg);
			ldg_funcs.encoder_write 		= ldg_find( "encoder_write", ldg);
			ldg_funcs.encoder_quit 		= ldg_find( "encoder_quit", ldg);
		
			if ( !ldg_funcs.encoder_init || !ldg_funcs.encoder_write || !ldg_funcs.encoder_quit)
			{
				errshow( encoder[encoder_selected]->extensions, LDG_ERR_BASE + ldg_error());
				return FALSE;
			}
			if( strncmp( encoder[encoder_selected]->extensions, "JPG", 3) == 0)
				ldg_funcs.set_jpg_option = ldg_find( "set_jpg_option", ldg);
			else if( strncmp( encoder[encoder_selected]->extensions, "TIF", 3) == 0)
				ldg_funcs.set_tiff_option = ldg_find( "set_tiff_option", ldg);
				
			have_options = ldg_funcs.set_jpg_option != NULL || ldg_funcs.set_tiff_option != NULL;
		}
		break;
	case CODEC_SLB:
		{
			SLB *slb;
			long err;
			
			slb = &encoder[encoder_selected]->c.slb;
			err = plugin_get_option(slb, OPTION_CAPABILITIES);
			if (err < 0 || !(err & CAN_ENCODE))
			{
				errshow( encoder[encoder_selected]->extensions, err);
				return FALSE;
			}
			err = plugin_get_option(slb, OPTION_QUALITY);
			have_options = err >= 0;
		}
		break;
	default:
		return FALSE;
	}

	if (have_options)
		ObjcChange( OC_FORM, win, SAVE_DIAL_OPTIONS, NORMAL, 0);
	else
		ObjcChange( OC_FORM, win, SAVE_DIAL_OPTIONS, DISABLED, 0);

	zstrncpy( save_dialog_content[SAVE_DIAL_FORMAT].ob_spec.free_string, encoder[encoder_selected]->extensions, 4);

   	ObjcDraw( OC_FORM, win, SAVE_DIAL_FORMAT, 1);
   	ObjcDraw( OC_FORM, win, SAVE_DIAL_OPTIONS, 1);

	return TRUE;
}


static void format_popup( WINDOW *win, int obj_index) 
{
	char items[MAX_CODECS][4];
	char *items_ptr[MAX_CODECS];
	int16 i, x, y;
	int choice;

	for( i = 0; i < encoder_plugins_nbr; i++)
	{	
		zstrncpy( items[i], encoder[i]->extensions, 4);
		items[i][3] = '\0';
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


static void __CDECL save_dialog_event( WINDOW *win EVNT_BUFF_PARAM)
{
	char target_file[MAX_PATH+MAXNAMLEN];
	char target_file_path[MAX_PATH];
	char target_file_name[MAXNAMLEN];
	char extension[MAXNAMLEN];
	char file_mask[MAXNAMLEN];
	int16 object = EVNT_BUFF[4];
	int16 source_file_len;
	int16 source_path_len;
	char *dot;

	switch (object)
	{	   
		case SAVE_DIAL_FORMAT:
			format_popup( win, object);
			ObjcChange( OC_FORM, win, object, NORMAL, TRUE);
			break;

		case SAVE_DIAL_OPTIONS:
			if( strncmp( encoder[last_choice-1]->extensions, "JPG", 3) == 0)
			{
				ObjcChange( OC_FORM, win, object, NORMAL, TRUE);
				jpg_option_dialog( source_file);
			}
			else if( strncmp( encoder[last_choice-1]->extensions, "TIF", 3) == 0)
			{
				ObjcChange( OC_FORM, win, object, NORMAL, TRUE);
				tiff_option_dialog();
			}
			break;

		case SAVE_DIAL_SAVE:
			source_file_len = ( int16)strlen( source_file);
			
			source_path_len = source_file_len;	
		
			while(source_path_len > 0 && source_file[source_path_len] != '/' && source_file[source_path_len] != '\\')
				source_path_len--;

			strcpy( target_file_path, source_file);

			target_file_path[source_path_len] = '\0';	

			source_path_len++;
							
			strcpy(target_file_name, source_file + source_path_len);
	
			/* copy the source's name in the target's name with the new extension for exemple ( "toto.gif" to "toto.jpg") */
			zstrncpy( extension, encoder[last_choice - 1]->extensions, 4);
			str2lower( extension);
			dot = strrchr(target_file_name, '.');
			if (dot)
				strcpy(dot + 1, extension);
				
			/* The file mask ( for exemple *.jpg) */
			strcpy( file_mask, "*.");
			strcat( file_mask, extension);

			ObjcChange( OC_FORM, win, object, NORMAL, FALSE);
			close_modal( win EVNT_BUFF_NULL);

			if( FselInput( target_file_path, target_file_name, file_mask, get_string( SAVE_TITLE), NULL, NULL)) 
			{
				strcpy( target_file, target_file_path);

				strcat( target_file, target_file_name);

				/* the source can't be the same that the destination */
				if( strcasecmp( source_file, target_file) == 0)
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
					
					if( strcasecmp( target_file_path, wicones->directory) == 0)
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
			ObjcChange( OC_FORM, win, object, NORMAL, FALSE);
			ApplWrite( _AESapid, WM_CLOSED, win->handle, 0, 0, 0, 0);
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
			switch (codecs[i].type)
			{
			case CODEC_LDG:
				if( ldg_find( "encoder_init", codecs[i].c.ldg))
				{
					encoder[encoder_plugins_nbr] = &codecs[i];
					encoder_plugins_nbr++;
				}
				break;
			case CODEC_SLB:
				{
					long err;
					err = plugin_get_option(&codecs[i].c.slb, OPTION_CAPABILITIES);
					if (err >= 0 && (err & CAN_ENCODE))
					{
						encoder[encoder_plugins_nbr] = &codecs[i];
						encoder_plugins_nbr++;
					}
				}
				break;
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
