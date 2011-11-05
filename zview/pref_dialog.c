#include "general.h"
#include "catalog/catalog.h"
#include "catalog/catalog_mini_entry.h"
#include "prefs.h"
#include "ztext.h"
#include "file/file.h"
#include "close_modal.h"

static OBJECT *pref_dialog ;

static int old_show_size				= 0;
static int old_show_hidden				= 0;
static int old_show_non_image			= 0;
static int old_show_read_progress_bar	= 0;
static int old_show_write_progress_bar	= 0;
static int old_pdf_fit_to_win			= 0;
static int old_pdf_aa					= 0;
static int old_smooth_thumbnail			= 0;

static char *items[] = { "None", "Triangle", "Blackman", "Gaussian", "Quadratic", "Cubic", "Lanczos"};
static int	drive[]  = { PREFS_A ,PREFS_B, PREFS_C, PREFS_D, PREFS_E, PREFS_F, PREFS_G, PREFS_H, PREFS_I, PREFS_J,
PREFS_K, PREFS_L, PREFS_M, PREFS_N, PREFS_O, PREFS_P, PREFS_Q, PREFS_R, PREFS_S, PREFS_T, PREFS_U, PREFS_V, PREFS_W,
PREFS_X, PREFS_Y, PREFS_Z};
static int i, drv;

static void format_popup( WINDOW *win, int obj_index) 
{
	int16 x, y;
	int choice;

	objc_offset( FORM(win), obj_index, &x, &y);

	choice = MenuPopUp ( items, x, y, 7, -1, smooth_thumbnail + 1, P_LIST + P_WNDW + P_CHCK);

	if( choice < 0)
		return;

	smooth_thumbnail = choice - 1;
	
	strcpy( pref_dialog[PREFS_SMOOTH_METHOD].ob_spec.free_string, items[smooth_thumbnail]);
   	ObjcDraw( OC_FORM, win, PREFS_SMOOTH_METHOD, 1);
}


/*==================================================================================*
 * void handle_preference:															*
 *		Handle OK and CANCEL buttons in the preference dialog.						*
 *----------------------------------------------------------------------------------*
 * input:																			*
 *		win 	->	Pointer to the preference window.								*
 *----------------------------------------------------------------------------------*
 * returns: 																		*
 *      --																			*
 *==================================================================================*/
static void handle_preference( WINDOW *win)
{
	switch( evnt.buff[4])
	{
		case PREFS_PDF_FIT_TO_WINDOW:
			pdf_fit_to_win = ( ( pref_dialog[PREFS_PDF_FIT_TO_WINDOW].ob_state & SELECTED) ? 1 : 0);
			break;

		case PREFS_PDF_ANTIALIAS:
			pdf_aa = ( ( pref_dialog[PREFS_PDF_ANTIALIAS].ob_state & SELECTED) ? 1 : 0);
			break;
		
		case PREFS_SHOW_SIZE:
			show_size = ( ( pref_dialog[PREFS_SHOW_SIZE].ob_state & SELECTED) ? 1 : 0);
			break;

		case PREFS_SHOW_HIDDEN:
			show_hidden = ( ( pref_dialog[PREFS_SHOW_HIDDEN].ob_state & SELECTED) ? 1 : 0);
			break;

		case PREFS_SHOW_NON_IMAGE:
			show_non_image = ( ( pref_dialog[PREFS_SHOW_NON_IMAGE].ob_state & SELECTED) ? 1 : 0);
			break;

		case PREFS_SHOW_READ_PROGRESS:
			show_read_progress_bar = ( ( pref_dialog[PREFS_SHOW_READ_PROGRESS].ob_state & SELECTED) ? 1 : 0);
			break;

		case PREFS_SHOW_WRITE_PROGRESS:
			show_write_progress_bar = ( ( pref_dialog[PREFS_SHOW_WRITE_PROGRESS].ob_state & SELECTED) ? 1 : 0);
			break;
			
		case PREFS_SMOOTH_METHOD:
			format_popup( win, evnt.buff[4]);
			ObjcChange( OC_FORM, win, evnt.buff[4], NORMAL, TRUE);
			break;
									
		case PREFS_CANCEL:
			smooth_thumbnail		= old_smooth_thumbnail;
			show_size				= old_show_size;
			show_hidden				= old_show_hidden;
			show_non_image			= old_show_non_image;
			show_read_progress_bar	= old_show_read_progress_bar;
			show_write_progress_bar	= old_show_write_progress_bar;
			pdf_fit_to_win			= old_pdf_fit_to_win;
			pdf_aa					= old_pdf_aa;			

			ObjcChange( OC_FORM, win, evnt.buff[4], ~SELECTED, TRUE) ;
			ApplWrite( app.id, WM_CLOSED, win->handle, 0, 0, 0, 0);
			break;

		case PREFS_OK:
			ObjcChange( OC_FORM, win, evnt.buff[4], ~SELECTED, TRUE);
			ApplWrite( app.id, WM_CLOSED, win->handle, 0, 0, 0, 0);

			skip_drive[0] = '\0';
			
			for( i = 0; i < 26; i++)
			{
				drv = 'A' + i;
				
				if( pref_dialog[drive[i]].ob_state & SELECTED)
				{
					char t[2];
					sprintf( t, "%c", drv);
					strcat( skip_drive, t);
				}
			}
			
			if( ( old_show_size			!= show_size)		||
				( old_show_hidden		!= show_hidden)		||
				( old_show_non_image	!= show_non_image))
			{
				int16 dum, w, h;

				if( win_catalog)
				{
					WINDICON *wicones = ( WINDICON *)DataSearch( win_catalog, WD_ICON);

					wicones -> case_h = thumbnail[thumbnail_size][1] + hcell + y_space + y_space + y_space;
	
					if ( show_size)
						wicones -> case_h  += hcell + y_space;

					win_catalog -> h_u	= wicones -> case_h;

					WindCalc( WC_BORDER, win_catalog, 0, 0, wicones->case_w + border_size + browser_frame_width, wicones->case_h, &dum, &dum, &w, &h);

					win_catalog -> h_min = h;
					win_catalog -> w_min = w;


					if( old_show_hidden != show_hidden)
					{
						( void)scan_dir( win_catalog, wicones->directory);
						win_catalog->ypos =  0;
						snd_rdw( win_catalog);
						WindMakePreview( win_catalog);
					}
					else
						snd_rdw( win_catalog);
				}
			}
			break;

		default:
			break;
	}

	/* a wait loop while the mouse button is pressed */		
	while(( evnt.mbut == 1) || ( evnt.mbut == 2))
		graf_mkstate( &evnt.mx, &evnt.my, &evnt.mbut, &evnt.mkstate); 	
}


/*==================================================================================*
 * void preference_dialog:															*
 *		Show the preference dialog.													*
 *----------------------------------------------------------------------------------*
 * input:																			*
 *		--																			*
 *----------------------------------------------------------------------------------*
 * returns: 																		*
 *      --																			*
 *==================================================================================*/
void preference_dialog( void)
{
	WINDOW	*win;
	int	frms[] = { PREFS_PANEL1,  PREFS_PANEL2,  PREFS_PANEL3};
	int	buts[] = { PREFS_GENERAL, PREFS_BROWSER, PREFS_PDF};

	old_show_size				= show_size;
	old_show_hidden				= show_hidden;
	old_show_non_image			= show_non_image;
	old_show_read_progress_bar	= show_read_progress_bar;
	old_show_write_progress_bar	= show_write_progress_bar;
	old_pdf_fit_to_win			= pdf_fit_to_win;
	old_pdf_aa					= pdf_aa;
	old_smooth_thumbnail		= smooth_thumbnail;
	
	pref_dialog = get_tree( PREFS);

	pref_dialog[PREFS_SHOW_SIZE].ob_state 			|= ( show_size ? SELECTED : NORMAL);
	pref_dialog[PREFS_SHOW_HIDDEN].ob_state 		|= ( show_hidden ? SELECTED : NORMAL);
	pref_dialog[PREFS_SHOW_NON_IMAGE].ob_state 	 	|= ( show_non_image ? SELECTED : NORMAL);
	pref_dialog[PREFS_SHOW_READ_PROGRESS].ob_state  |= ( show_read_progress_bar ? SELECTED : NORMAL);
	pref_dialog[PREFS_SHOW_WRITE_PROGRESS].ob_state |= ( show_write_progress_bar ? SELECTED : NORMAL);
	pref_dialog[PREFS_PDF_ANTIALIAS].ob_state  		|= ( pdf_aa ? SELECTED : NORMAL);
	pref_dialog[PREFS_PDF_FIT_TO_WINDOW].ob_state 	|= ( pdf_fit_to_win ? SELECTED : NORMAL);
	strcpy( pref_dialog[PREFS_SMOOTH_METHOD].ob_spec.free_string, items[smooth_thumbnail]);	

	for( i = 0; i < 26; i++)
	{
		drv = 'A' + i;

		pref_dialog[drive[i]].ob_state |= ( strchr( skip_drive, drv) ? SELECTED : NORMAL);
	}
	
	win = FormCreate( pref_dialog, NAME|MOVER, handle_preference, "Options", NULL, TRUE, FALSE);

	FormThumb( win, frms, buts, 3);	
	
	WindSet( win, WF_BEVENT, BEVENT_MODAL, 0, 0, 0);

	EvntAttach( win, WM_CLOSED, close_modal);

	MenuDisable();
}

