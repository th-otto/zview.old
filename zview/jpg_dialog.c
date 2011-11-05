#include "general.h"
#include "prefs.h"
#include "ztext.h"
#include "pic_load.h"
#include "catalog/catalog.h"

OBJECT 	*jpg_option_content;
static 	int16 quality = 90;
static 	J_COLOR_SPACE color_space = JCS_RGB;
static	boolean progressive = FALSE;
static 	boolean show_preview = FALSE;
void 	*jpg_option_slider = NULL;

extern void CDECL ( *set_jpg_option)( int16 set_quality, J_COLOR_SPACE set_color_space, boolean set_progressive);


static void jpg_option_close( WINDOW *win) 
{
	free( jpg_option_slider);
	frm_cls( win);
}

static void jpg_option_gray_event( WINDOW *win, int16 obj_index)
{
	color_space = ( color_space == JCS_RGB ? JCS_GRAYSCALE : JCS_RGB);

	/* a wait loop while the mouse button is pressed */		
	while(( evnt.mbut == 1) || ( evnt.mbut == 2))
		graf_mkstate( &evnt.mx, &evnt.my, &evnt.mbut, &evnt.mkstate); 
}

static void jpg_option_progressive_event( WINDOW *win, int16 obj_index) 
{
	progressive = ( progressive == TRUE ? FALSE : TRUE);

	/* a wait loop while the mouse button is pressed */		
	while(( evnt.mbut == 1) || ( evnt.mbut == 2))
		graf_mkstate( &evnt.mx, &evnt.my, &evnt.mbut, &evnt.mkstate); 
}

static void jpg_option_show_preview( WINDOW *win, int16 obj_index) 
{
	show_preview = ( show_preview == TRUE ? FALSE : TRUE);

	/* a wait loop while the mouse button is pressed */		
	while(( evnt.mbut == 1) || ( evnt.mbut == 2))
		graf_mkstate( &evnt.mx, &evnt.my, &evnt.mbut, &evnt.mkstate); 
}

static void jpg_option_ok_event( WINDOW *win, int16 obj_index) 
{
	set_jpg_option( quality, color_space, progressive);

	ObjcChange( OC_FORM, win, obj_index, NORMAL, TRUE);
	ApplWrite( app.id, WM_CLOSED, win->handle, 0, 0, 0, 0);
}

static void jpg_option_slider_event( WINDOW *win, int mode, float value) 
{
	quality = ( int16)value;
	evnt_timer( 50);
	sprintf( ObjcString( FORM( win), JPGPREF_PERCENT, NULL), "%d", quality);
	ObjcDraw( mode, win, JPGPREF_PERCENT, TRUE);
}

/*==================================================================================*
 * void jpg_option_dialog:															*
 *		Open and handle JPG encoder option dialog.									*
 *----------------------------------------------------------------------------------*
 * input:																			*
 *		source_file:		the full path name of the source file.					*
 *----------------------------------------------------------------------------------*
 * returns: 																		*
 *      --																			*
 *==================================================================================*/
void jpg_option_dialog( char *source_file)
{
//	struct stat	file_stat;
	WINDOW 		*win;

	jpg_option_content = get_tree( JPGPREF);

//	stat( source_file, &file_stat);

//	size_to_text( ObjcString( jpg_option_content, JPGPREF_ORIGINAL_SIZE, NULL), ( float)file_stat.st_size);
//	sprintf( ObjcString( jpg_option_content, JPGPREF_PERCENT, NULL), "%d", quality);

	jpg_option_content[JPGPREF_SL].ob_x = quality;

	if( ( win = FormCreate( jpg_option_content, NAME|MOVER, NULL, get_string( JPG_OPTION_TITLE), NULL, TRUE, FALSE)) == NULL)
	{
		WindClose( win);
		errshow( "", ALERT_WINDOW);	
		return;
	}

//	RsrcUserDraw ( OC_FORM, win, JPGPREF_ORIGINAL, draw_object_image, NULL);
//	RsrcUserDraw ( OC_FORM, win, JPGPREF_RESULT, draw_object_image, NULL);

	WindSet( win, WF_BEVENT, BEVENT_MODAL, 0, 0, 0);

	if( ( jpg_option_slider = SlidCreate( 1, 100, 1, 10, quality, SLD_HORI, SLD_IMME)) == NULL)
	{
		frm_cls( win);
		errshow( "", ENOMEM);
		return;	
	}	
	
	EvntAttach( win, WM_CLOSED, jpg_option_close);
	SlidAttach( jpg_option_slider, OC_FORM, win, JPGPREF_LF, JPGPREF_PG, JPGPREF_SL, JPGPREF_RT);
	SlidSetFunc( jpg_option_slider, jpg_option_slider_event, NULL);

//	ObjcAttach( OC_FORM, win, JPGPREF_PREVIEW, BIND_FUNC, jpg_option_show_preview);
	ObjcAttach( OC_FORM, win, JPGPREF_GRAY, BIND_FUNC, jpg_option_gray_event);
	ObjcAttach( OC_FORM, win, JPGPREF_OK, BIND_FUNC, jpg_option_ok_event);
	ObjcAttach( OC_FORM, win, JPGPREF_PROGRESSIVE, BIND_FUNC, jpg_option_progressive_event);
}

