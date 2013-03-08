#include "general.h"
#include "prefs.h"
#include "ztext.h"
#include "pic_load.h"
#include "catalog/catalog.h"

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


extern void CDECL ( *set_tiff_option)( int16 set_quality, uint16 set_encode_compression);


static void tiff_option_close( WINDOW *win) 
{
	free( tiff_option_slider);
	frm_cls( win);
}

static void tiff_option_ok_event( WINDOW *win, int16 obj_index) 
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

	set_tiff_option( quality, compression);

	ObjcChange( OC_FORM, win, obj_index, NORMAL, TRUE);
	ApplWrite( app.id, WM_CLOSED, win->handle, 0, 0, 0, 0);
}


static void tiff_option_slider_event( WINDOW *win, int mode, float value) 
{
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
	
	ObjcAttach( OC_FORM, win, TIFFPREF_None, 	BIND_VAR,	&compression_button);
	ObjcAttach( OC_FORM, win, TIFFPREF_RLE, 	BIND_VAR,	&compression_button);
	ObjcAttach( OC_FORM, win, TIFFPREF_LZW, 	BIND_VAR,	&compression_button);
	ObjcAttach( OC_FORM, win, TIFFPREF_JPEG, 	BIND_VAR,	&compression_button);
	ObjcAttach( OC_FORM, win, TIFFPREF_DEFLATE, BIND_VAR, 	&compression_button);

	ObjcAttach( OC_FORM, win, TIFFPREF_OK, BIND_FUNC, tiff_option_ok_event);
}

