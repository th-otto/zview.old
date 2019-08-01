#include "general.h"
#include "prefs.h"
#include "ztext.h"
#include "pic_load.h"
#include "catalog/catalog.h"
#include "plugins.h"

#define	    COMPRESSION_NONE		1		/* dump mode */
#define	    COMPRESSION_PACKBITS	32773	/* Macintosh RLE */
#define	    COMPRESSION_LZW			5       /* Lempel-Ziv  & Welch */
#define	    COMPRESSION_JPEG		7		/* JPEG DCT compression */
#define	    COMPRESSION_DEFLATE		32946	/* Deflate compression */

OBJECT 	*tiff_option_content;
void 	*tiff_option_slider = NULL;

static 	int16 	quality 			= 75;
static	uint16 	compression 		= COMPRESSION_NONE;
static  int		compression_button 	= 0;



static void __CDECL tiff_option_close( WINDOW *win EVNT_BUFF_PARAM)
{
	free( tiff_option_slider);
	frm_cls( win);
}

static void __CDECL tiff_option_ok_event( WINDOW *win, int obj_index, int mode, void *data)
{
	switch( compression_button)
	{ 
		case TIFFPREF_DEFLATE:
			compression = COMPRESSION_DEFLATE;
			break;	
		case TIFFPREF_JPEG:
			compression = COMPRESSION_JPEG;
			break;	
		case TIFFPREF_LZW:
			compression = COMPRESSION_LZW;
			break;	
		case TIFFPREF_RLE:
			compression = COMPRESSION_PACKBITS;
			break;	
		default:
			compression = COMPRESSION_NONE;
			break;		
	}

	ldg_funcs.set_tiff_option( quality, compression);

	ObjcChange( OC_FORM, win, obj_index, NORMAL, TRUE);
	ApplWrite( _AESapid, WM_CLOSED, win->handle, 0, 0, 0, 0);
}


static void tiff_option_slider_event( WINDOW *win, int mode,
#if __WINDOM_MAJOR__ >= 2
	int16 value,
#else
	float value,
#endif
	void *data)
{
	(void)data;
	quality = ( int16)value;
	evnt_timer( 50);
	sprintf( ObjcString( FORM( win), TIFFPREF_PERCENT, NULL), "%d", quality);
	ObjcDraw( mode, win, TIFFPREF_PERCENT, TRUE);
}

/*==================================================================================*
 * void tiff_option_dialog:															*
 *		Open and handle TIFF encoder option dialog.									*
 *----------------------------------------------------------------------------------*
 * input:																			*
 *		--																			*
 *----------------------------------------------------------------------------------*
 * returns: 																		*
 *      --																			*
 *==================================================================================*/
void tiff_option_dialog( void)
{
	WINDOW 	*win;

	tiff_option_content = get_tree( TIFFPREF);

	sprintf( ObjcString( tiff_option_content, TIFFPREF_PERCENT, NULL), "%d", quality);

	tiff_option_content[TIFFPREF_SL].ob_x = quality;

	if( ( win = FormCreate( tiff_option_content, NAME|MOVER, NULL, get_string( TIFF_OPTION_TITLE), NULL, TRUE, FALSE)) == NULL)
	{
		errshow( "", ALERT_WINDOW);	
		return;
	}

	WindSet( win, WF_BEVENT, BEVENT_MODAL, 0, 0, 0);

	if( ( tiff_option_slider = SlidCreate( 1, 100, 1, 10, quality, SLD_HORI, SLD_IMME)) == NULL)
	{
		WindClose( win);
		errshow( "", ENOMEM);
		return;	
	}	


	EvntAttach( win, WM_CLOSED, tiff_option_close);
	SlidAttach( tiff_option_slider, OC_FORM, win, TIFFPREF_LF, TIFFPREF_PG, TIFFPREF_SL, TIFFPREF_RT);
	SlidSetFunc( tiff_option_slider, tiff_option_slider_event, NULL);

	if( compression == COMPRESSION_NONE)
		ObjcChange( OC_FORM, win, TIFFPREF_None, SELECTED, TRUE);	
	
	ObjcAttachVar( OC_FORM, win, TIFFPREF_None, 	&compression_button, TIFFPREF_None);
	ObjcAttachVar( OC_FORM, win, TIFFPREF_RLE, 	&compression_button, TIFFPREF_RLE);
	ObjcAttachVar( OC_FORM, win, TIFFPREF_LZW, 	&compression_button, TIFFPREF_LZW);
	ObjcAttachVar( OC_FORM, win, TIFFPREF_JPEG, 	&compression_button, TIFFPREF_JPEG);
	ObjcAttachVar( OC_FORM, win, TIFFPREF_DEFLATE, &compression_button, TIFFPREF_DEFLATE);

	ObjcAttachFormFunc( win, TIFFPREF_OK, tiff_option_ok_event, NULL);
}
