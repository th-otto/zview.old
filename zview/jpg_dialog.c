#include "general.h"
#include "prefs.h"
#include "ztext.h"
#include "pic_load.h"
#include "catalog/catalog.h"
#include "plugins.h"
#include "plugins/common/zvplugin.h"

OBJECT 	*jpg_option_content;
static 	int16 quality = 90;
static 	J_COLOR_SPACE color_space = JCS_RGB;
static	boolean progressive = FALSE;
#if 0
static 	boolean show_preview = FALSE;
#endif
void 	*jpg_option_slider = NULL;


static void __CDECL jpg_option_close( WINDOW *win EVNT_BUFF_PARAM)
{
	free( jpg_option_slider);
	frm_cls( win);
}

static void jpg_option_gray_event( WINDOW *win, int obj_index, int mode, void *data)
{
	color_space = ( color_space == JCS_RGB ? JCS_GRAYSCALE : JCS_RGB);

	/* a wait loop while the mouse button is pressed */		
	while(( evnt.mbut == 1) || ( evnt.mbut == 2))
		graf_mkstate( &evnt.mx, &evnt.my, &evnt.mbut, &evnt.mkstate); 
}

static void jpg_option_progressive_event( WINDOW *win, int obj_index, int mode, void *data)
{
	progressive = ( progressive == TRUE ? FALSE : TRUE);

	/* a wait loop while the mouse button is pressed */		
	while(( evnt.mbut == 1) || ( evnt.mbut == 2))
		graf_mkstate( &evnt.mx, &evnt.my, &evnt.mbut, &evnt.mkstate); 
}

#if 0
static void jpg_option_show_preview( WINDOW *win, int obj_index, int mode, void *data)
{
	show_preview = ( show_preview == TRUE ? FALSE : TRUE);

	/* a wait loop while the mouse button is pressed */		
	while(( evnt.mbut == 1) || ( evnt.mbut == 2))
		graf_mkstate( &evnt.mx, &evnt.my, &evnt.mbut, &evnt.mkstate); 
}
#endif

static void jpg_option_ok_event( WINDOW *win, int obj_index, int mode, void *data)
{
	if (curr_output_plugin)
	{
		plugin_set_option(curr_output_plugin, OPTION_QUALITY, quality);
		plugin_set_option(curr_output_plugin, OPTION_COLOR_SPACE, color_space);
		plugin_set_option(curr_output_plugin, OPTION_PROGRESSIVE, progressive);
	} else
	{
		ldg_funcs.set_jpg_option( quality, color_space, progressive);
	}
	ObjcChange( OC_FORM, win, obj_index, NORMAL, TRUE);
	ApplWrite( _AESapid, WM_CLOSED, win->handle, 0, 0, 0, 0);
}

static void __CDECL jpg_option_slider_event( WINDOW *win, int mode,
#if __WINDOM_MAJOR__ >= 2
	short value,
#else
	float value,
#endif
	void *data)
{
	(void)data;
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
void jpg_option_dialog( const char *source_file)
{
	WINDOW 		*win;

	jpg_option_content = get_tree( JPGPREF);

//	size_to_text( ObjcString( jpg_option_content, JPGPREF_ORIGINAL_SIZE, NULL), ( float)file_stat.st_size);
//	sprintf( ObjcString( jpg_option_content, JPGPREF_PERCENT, NULL), "%d", quality);

	if (curr_output_plugin)
	{
		long value;
		
		if ((value = plugin_get_option(curr_output_plugin, OPTION_QUALITY)) >= 0)
			quality = value;
		if ((value = plugin_get_option(curr_output_plugin, OPTION_COLOR_SPACE)) >= 0)
			color_space = value;
		if ((value = plugin_get_option(curr_output_plugin, OPTION_PROGRESSIVE)) >= 0)
			progressive = value;
	}

	jpg_option_content[JPGPREF_SL].ob_x = quality;

	if( ( win = FormCreate( jpg_option_content, NAME|MOVER, NULL, get_string( JPG_OPTION_TITLE), NULL, TRUE, FALSE)) == NULL)
	{
		WindClose( win);
		errshow(NULL, ALERT_WINDOW);	
		return;
	}

#if 0
	RsrcUserDraw ( OC_FORM, win, JPGPREF_ORIGINAL, draw_object_image, NULL);
	RsrcUserDraw ( OC_FORM, win, JPGPREF_RESULT, draw_object_image, NULL);
#endif

	WindSet( win, WF_BEVENT, BEVENT_MODAL, 0, 0, 0);

	if( ( jpg_option_slider = SlidCreate( 1, 100, 1, 10, quality, SLD_HORI, SLD_IMME)) == NULL)
	{
		frm_cls( win);
		errshow(NULL, -ENOMEM);
		return;	
	}	
	
	EvntAttach( win, WM_CLOSED, jpg_option_close);
	SlidAttach( jpg_option_slider, OC_FORM, win, JPGPREF_LF, JPGPREF_PG, JPGPREF_SL, JPGPREF_RT);
	SlidSetFunc( jpg_option_slider, jpg_option_slider_event, NULL);

#if 0
	ObjcAttachFormFunc( win, JPGPREF_PREVIEW, jpg_option_show_preview, NULL);
#endif
	ObjcAttachFormFunc( win, JPGPREF_GRAY, jpg_option_gray_event, NULL);
	ObjcAttachFormFunc( win, JPGPREF_OK, jpg_option_ok_event, NULL);
	ObjcAttachFormFunc( win, JPGPREF_PROGRESSIVE, jpg_option_progressive_event, NULL);
}
