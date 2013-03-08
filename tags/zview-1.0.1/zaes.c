#include "general.h"
#include "prefs.h"
#include "infobox.h"
#include "pic_load.h"
#include "winimg.h"
#include "custom_font.h"
#include "catalog/catalog_icons.h"
#include "catalog/catalog.h"

/* the variable need to be global because this function is done by */ 
/* the AES in supervisor mode.. it's safe. */
static int16 	posx, posy, xy[10];
static MFDB  	screen = {0};
static int16	wanted_color[2] = { BLACK, WHITE};
static MFDB 	*source = NULL;
static WINDATA	*windata;
static WINDICON *wicones;


/*  *** workaround for TOS and its small supervisor stack ****
 *
 *  gemlib function that are stack hungry are replaced by the following
 *  function. These replacement are not safe thread but we don't care
 *  because this is for the non-shareable version of the LDG (for monoTOS)
 */

static short vdi_intin[VDI_INTINMAX];
static short vdi_intout[VDI_INTOUTMAX];
static short vdi_ptsin[VDI_PTSINMAX];
static short vdi_ptsout[VDI_PTSOUTMAX];
static short vdi_control[VDI_CNTRLMAX];

static VDIPB vdi_params = {
        &vdi_control[0],
        &vdi_intin[0],
        &vdi_ptsin[0],
        &vdi_intout[0],
        &vdi_ptsout[0] 
};



static int my_vsl_color (short handle, short idx) {
	vdi_intin[0] = idx;

	vdi_control[0] = 17;
	vdi_control[1] = vdi_control[5] = 0;
	vdi_control[3] = 1;
	vdi_control[6] = handle;

	vdi (&vdi_params);

	return ((int)vdi_intout[0]);
}

#define vsl_color my_vsl_color

/* *** end of the workaround for TOS *** */

void generic_form_event( WINDOW *win) 
{
	ObjcChange( OC_FORM, win, evnt.buff[4], NORMAL, FALSE);
	ApplWrite( app.id, WM_CLOSED, win->handle, 0, 0, 0, 0);
}


OBJECT *get_tree( int16 obj_index) 
{
	OBJECT *tree;
	RsrcGaddr( NULL, 0, obj_index, &tree);
	return tree;
}


/* Return a string from resource file */

char *get_string( int16 str_index) 
{
	char *txt;	
	rsrc_gaddr( 5, str_index,  &txt);
	return txt;
}


void CDECL draw_object_image( WINDOW *win, PARMBLK *pblk, void *data) 
{	
	source = ( MFDB*)data;
	
	if( source == NULL)
	{	
		xy[0] = pblk->pb_x;
		xy[1] = pblk->pb_y;
		xy[2] = xy[0] + 127;
		xy[3] = xy[1] + 127;

		vsf_color( win->graf.handle, WHITE);
		v_bar( win->graf.handle, xy);

		xy[0] = pblk->pb_x;
		xy[1] = pblk->pb_y;
		xy[2] = pblk->pb_x;
		xy[3] = pblk->pb_y + 127;
		xy[4] = pblk->pb_x + 127;
		xy[5] = xy[3];
		xy[6] = xy[4];
		xy[7] = pblk->pb_y;
		xy[8] = pblk->pb_x;
		xy[9] = pblk->pb_y;

		vsl_color( win->graf.handle, BLACK);
		v_pline( win->graf.handle, 5, xy);

		return;
	}
	else if(( source->fd_w < 128) || ( source->fd_h < 128))
	{	
		xy[0] = pblk->pb_x;
		xy[1] = pblk->pb_y;
		xy[2] = xy[0] + 127;
		xy[3] = xy[1] + 127;

		vsf_color( win->graf.handle, WHITE);
		v_bar( win->graf.handle, xy);
	}

	posx    = MAX( ( 128 - source->fd_w) >> 1, 0);
	posy 	= MAX( ( 128 - source->fd_h) >> 1, 0);

	if (posx == 0)
		xy[2] = 127;
	else
		xy[2] = source->fd_w - 1;
  
	if (posy == 0)
		xy[3] = 127;
	else
		xy[3] = source->fd_h - 1;

	xy[0] = 0; 
	xy[1] = 0;
	xy[4] = pblk->pb_x + posx; 
	xy[5] = pblk->pb_y + posy;
	xy[6] = xy[4] + xy[2];
	xy[7] = xy[5] + xy[3];

	if ( source->fd_nplanes == 1)
		vrt_cpyfm( win->graf.handle, MD_REPLACE, xy, source, &screen, wanted_color);
	else
		vro_cpyfm( win->graf.handle, S_ONLY, xy, source, &screen);
		
	xy[0] = pblk->pb_x;
	xy[1] = pblk->pb_y;
	xy[2] = pblk->pb_x;
	xy[3] = pblk->pb_y + 127;
	xy[4] = pblk->pb_x + 127;
	xy[5] = xy[3];
	xy[6] = xy[4];
	xy[7] = pblk->pb_y;
	xy[8] = pblk->pb_x;
	xy[9] = pblk->pb_y;

	vsl_color( win->graf.handle, BLACK);
	v_pline( win->graf.handle, 5, xy);
}


void CDECL draw_icon_up( WINDOW *win, PARMBLK *pblk, void *data) 
{	
	if( pblk->pb_currstate & SELECTED)
		source = &mini_up_on;
	else	
		source = &mini_up;

	xy[0] = xy[1] = 0;								
	xy[2] = 15;		
	xy[3] = 15;	
	xy[4] = pblk->pb_x;	
	xy[5] = pblk->pb_y;	
	xy[6] = xy[4] + 15;								
	xy[7] = xy[5] + 15;
	
	if ( source->fd_nplanes == 1)
		vrt_cpyfm( win->graf.handle, MD_REPLACE, xy, source, &screen, wanted_color);
	else
		vro_cpyfm( win->graf.handle, S_ONLY, xy, source, &screen);		
}

void CDECL draw_icon_find( WINDOW *win, PARMBLK *pblk, void *data) 
{	
//	if( pblk->pb_currstate & SELECTED)
		source = &mini_find_on;
//	else	
//		source = &mini_find;

	xy[0] = xy[1] = 0;								
	xy[2] = 15;		
	xy[3] = 15;	
	xy[4] = pblk->pb_x;	
	xy[5] = pblk->pb_y;	
	xy[6] = xy[4] + 15;								
	xy[7] = xy[5] + 15;

	if ( source->fd_nplanes == 1)
		vrt_cpyfm( win->graf.handle, MD_REPLACE, xy, source, &screen, wanted_color);
	else
		vro_cpyfm( win->graf.handle, S_ONLY, xy, source, &screen);	
}


void CDECL draw_icon_reload( WINDOW *win, PARMBLK *pblk, void *data) 
{	
	if( pblk->pb_currstate & SELECTED)
		source = &mini_reload_on;
	else	
		source = &mini_reload;

	xy[0] = xy[1] = 0;								
	xy[2] = 15;		
	xy[3] = 15;	
	xy[4] = pblk->pb_x;	
	xy[5] = pblk->pb_y;	
	xy[6] = xy[4] + 15;								
	xy[7] = xy[5] + 15;

	if ( source->fd_nplanes == 1)
		vrt_cpyfm( win->graf.handle, MD_REPLACE, xy, source, &screen, wanted_color);
	else
		vro_cpyfm( win->graf.handle, S_ONLY, xy, source, &screen);	
}

void CDECL draw_icon_greater( WINDOW *win, PARMBLK *pblk, void *data)   
{	
	windata = ( WINDATA *)data;

	if( windata && (( windata->zoom_level >= 200) || ( windata->img.delay[0] != 0))) 
	{
		source = &mini_big_on;
	}
	else if( !windata && ( thumbnail_size >= 7 || thumbnail_size == 3))
	{
		source = &mini_big_on;
	}	
	else	
	{
		if( pblk->pb_currstate & SELECTED)
			source = &mini_big_on;
		else	
			source = &mini_big;
	}

	xy[0] = xy[1] = 0;								
	xy[2] = 15;		
	xy[3] = 15;	
	xy[4] = pblk->pb_x;	
	xy[5] = pblk->pb_y;	
	xy[6] = xy[4] + 15;								
	xy[7] = xy[5] + 15;

	if ( source->fd_nplanes == 1)
		vrt_cpyfm( win->graf.handle, MD_REPLACE, xy, source, &screen, wanted_color);
	else
		vro_cpyfm( win->graf.handle, S_ONLY, xy, source, &screen);	
}

void CDECL draw_icon_smaller( WINDOW *win, PARMBLK *pblk, void *data)  
{	
	windata = ( WINDATA *)data;

	if( windata && (( windata->zoom_level <= 25) || ( windata->img.delay[0] != 0)))
	{
		source = &mini_small_on;
	}	
	else if( !windata && ( !thumbnail_size || thumbnail_size == 4))
	{
		source = &mini_small_on;
	}	
	else	
	{
		if( pblk->pb_currstate & SELECTED)
			source = &mini_small_on;
		else	
			source = &mini_small;
	}


	xy[0] = xy[1] = 0;								
	xy[2] = 15;		
	xy[3] = 15;	
	xy[4] = pblk->pb_x;	
	xy[5] = pblk->pb_y;	
	xy[6] = xy[4] + 15;								
	xy[7] = xy[5] + 15;

	if ( source->fd_nplanes == 1)
		vrt_cpyfm( win->graf.handle, MD_REPLACE, xy, source, &screen, wanted_color);
	else
		vro_cpyfm( win->graf.handle, S_ONLY, xy, source, &screen);	
}

void CDECL draw_icon_back( WINDOW *win, PARMBLK *pblk, void *data)  
{	
	windata = ( WINDATA *)data;

	if( windata->page_to_show == 0)
	{
		source = &mini_back_on;
	}	
	else	
	{
		if( pblk->pb_currstate & SELECTED)
			source = &mini_back_on;
		else	
			source = &mini_back;
	}

	xy[0] = xy[1] = 0;								
	xy[2] = 15;		
	xy[3] = 15;	
	xy[4] = pblk->pb_x;	
	xy[5] = pblk->pb_y;	
	xy[6] = xy[4] + 15;								
	xy[7] = xy[5] + 15;

	if ( source->fd_nplanes == 1)
		vrt_cpyfm( win->graf.handle, MD_REPLACE, xy, source, &screen, wanted_color);
	else
		vro_cpyfm( win->graf.handle, S_ONLY, xy, source, &screen);	
}

void CDECL draw_icon_forward( WINDOW *win, PARMBLK *pblk, void *data)  
{	
	windata = ( WINDATA *)data;

	if( windata->page_to_show == windata->img.page - 1)
	{
		source = &mini_forward_on;
	}	
	else	
	{
		if( pblk->pb_currstate & SELECTED)
			source = &mini_forward_on;
		else	
			source = &mini_forward;
	}

	xy[0] = xy[1] = 0;								
	xy[2] = 15;		
	xy[3] = 15;	
	xy[4] = pblk->pb_x;	
	xy[5] = pblk->pb_y;	
	xy[6] = xy[4] + 15;								
	xy[7] = xy[5] + 15;

	if ( source->fd_nplanes == 1)
		vrt_cpyfm( win->graf.handle, MD_REPLACE, xy, source, &screen, wanted_color);
	else
		vro_cpyfm( win->graf.handle, S_ONLY, xy, source, &screen);	
}

void CDECL draw_icon_printer( WINDOW *win, PARMBLK *pblk, void *data)  
{	
	source = &mini_printer_on;

	xy[0] = xy[1] = 0;								
	xy[2] = 15;		
	xy[3] = 15;	
	xy[4] = pblk->pb_x;	
	xy[5] = pblk->pb_y;	
	xy[6] = xy[4] + 15;								
	xy[7] = xy[5] + 15;

	if ( source->fd_nplanes == 1)
		vrt_cpyfm( win->graf.handle, MD_REPLACE, xy, source, &screen, wanted_color);
	else
		vro_cpyfm( win->graf.handle, S_ONLY, xy, source, &screen);	
}


void CDECL draw_icon_open( WINDOW *win, PARMBLK *pblk, void *data)  
{	
	if( pblk->pb_currstate & SELECTED)
		source = &mini_open_on;
	else	
		source = &mini_open;

	xy[0] = xy[1] = 0;								
	xy[2] = 15;		
	xy[3] = 15;	
	xy[4] = pblk->pb_x;	
	xy[5] = pblk->pb_y;	
	xy[6] = xy[4] + 15;								
	xy[7] = xy[5] + 15;

	if ( source->fd_nplanes == 1)
		vrt_cpyfm( win->graf.handle, MD_REPLACE, xy, source, &screen, wanted_color);
	else
		vro_cpyfm( win->graf.handle, S_ONLY, xy, source, &screen);	
}

void CDECL draw_icon_fullscreen( WINDOW *win, PARMBLK *pblk, void *data)  
{	
	if( pblk->pb_currstate & SELECTED)
		source = &mini_fullscreen_on;
	else	
		source = &mini_fullscreen;

	xy[0] = xy[1] = 0;								
	xy[2] = 15;		
	xy[3] = 15;	
	xy[4] = pblk->pb_x;	
	xy[5] = pblk->pb_y;	
	xy[6] = xy[4] + 15;								
	xy[7] = xy[5] + 15;

	if ( source->fd_nplanes == 1)
		vrt_cpyfm( win->graf.handle, MD_REPLACE, xy, source, &screen, wanted_color);
	else
		vro_cpyfm( win->graf.handle, S_ONLY, xy, source, &screen);	
}


void CDECL draw_icon_info( WINDOW *win, PARMBLK *pblk, void *data)  
{	
	if( win_catalog == win)
	{
		wicones = ( WINDICON *)DataSearch( win, WD_ICON);
		
		if( wicones->first_selected)
		{
			if( pblk->pb_currstate & SELECTED)
				source = &mini_info_on;
			else	
			source = &mini_info;
		}
		else source = &mini_info_on;			
	}
	else
	{
		if( pblk->pb_currstate & SELECTED)
			source = &mini_info_on;
		else	
			source = &mini_info;
	}
	
	xy[0] = xy[1] = 0;								
	xy[2] = 15;		
	xy[3] = 15;	
	xy[4] = pblk->pb_x;	
	xy[5] = pblk->pb_y;	
	xy[6] = xy[4] + 15;								
	xy[7] = xy[5] + 15;

	if ( source->fd_nplanes == 1)
		vrt_cpyfm( win->graf.handle, MD_REPLACE, xy, source, &screen, wanted_color);
	else
		vro_cpyfm( win->graf.handle, S_ONLY, xy, source, &screen);	
}


void CDECL draw_icon_save( WINDOW *win, PARMBLK *pblk, void *data)  
{	
	if( win_catalog == win)
	{
		wicones = ( WINDICON *)DataSearch( win, WD_ICON);
		
		if( wicones->first_selected && wicones->first_selected->type == ET_IMAGE)
		{
			if( pblk->pb_currstate & SELECTED)
				source = &mini_save_on;
			else	
			source = &mini_save;
		}
		else source = &mini_save_on;			
	}
	else
	{
		if( pblk->pb_currstate & SELECTED)
			source = &mini_save_on;
		else	
			source = &mini_save;
	}
	
	xy[0] = xy[1] = 0;								
	xy[2] = 15;		
	xy[3] = 15;	
	xy[4] = pblk->pb_x;	
	xy[5] = pblk->pb_y;	
	xy[6] = xy[4] + 15;								
	xy[7] = xy[5] + 15;

	if ( source->fd_nplanes == 1)
		vrt_cpyfm( win->graf.handle, MD_REPLACE, xy, source, &screen, wanted_color);
	else
		vro_cpyfm( win->graf.handle, S_ONLY, xy, source, &screen);	
}

void CDECL draw_icon_delete( WINDOW *win, PARMBLK *pblk, void *data)  
{	
	wicones = ( WINDICON *)DataSearch( win, WD_ICON);
		
	if( wicones->first_selected)
	{
		if( pblk->pb_currstate & SELECTED)
			source = &mini_delete_on;
		else	
		source = &mini_delete;
	}
	else source = &mini_delete_on;			
	
	xy[0] = xy[1] = 0;								
	xy[2] = 15;		
	xy[3] = 15;	
	xy[4] = pblk->pb_x;	
	xy[5] = pblk->pb_y;	
	xy[6] = xy[4] + 15;								
	xy[7] = xy[5] + 15;

	if ( source->fd_nplanes == 1)
		vrt_cpyfm( win->graf.handle, MD_REPLACE, xy, source, &screen, wanted_color);
	else
		vro_cpyfm( win->graf.handle, S_ONLY, xy, source, &screen);	
}

/*==================================================================================*
 * void draw_window_iconified:														*
 *		Function to handle iconfied windows drawing.								*
 *----------------------------------------------------------------------------------*
 * input:																			*
 *		win -> the window to handle.												*
 *----------------------------------------------------------------------------------*
 * returns: 																		*
 *      --																			*
 *==================================================================================*/

void draw_window_iconified( WINDOW *win)
{	
	int16 x, y, w, h, pxy[4];
	OBJECT *tree;

	rsrc_gaddr( 0, MINI_ICON, &tree);

	WindGet( win, WF_WORKXYWH, &x, &y, &w, &h);

	pxy[0] = x;
	pxy[1] = y;
	pxy[2] = pxy[0] + w - 1;
	pxy[3] = pxy[1] + h - 1;

	vsf_color( win->graf.handle, LWHITE);
	v_bar( win->graf.handle, pxy);

	tree->ob_x = x + (( w - 32) >> 1);
	tree->ob_y = y + (( h - 32) >> 1);

	objc_draw( tree, MINI_ICON_SNIPE, 0, clip.g_x, clip.g_y, clip.g_w, clip.g_h);	
}



void errshow( const char *name, int16 error)
{
  	switch( error)
  	{
		case ALERT_WINDOW:
			( void)FormAlert( 1 , get_string( ALERT_WINDOW));
			break;
		case ENAMETOOLONG:
			( void)FormAlert( 1 , get_string( NAMETOOLONG));
			break;
		case ENOENT:
			( void)FormAlert( 1 , get_string( NOENT), name);
			break;				
		case EACCES:
			( void)FormAlert( 1 , get_string( ACCES));
			break;		
		case ELOOP:
			( void)FormAlert( 1 , get_string( LOOP));
			break;		
		case ENOTDIR:
			( void)FormAlert( 1 , get_string( NOTDIR));
			break;								
		case E_NAMEEXIST:
			( void)FormAlert( 1 , get_string( NAMEALREADYUSED));
			break;		
		case EXDEV:
			( void)FormAlert( 1 , get_string( XDEV));
			break;		
		case ENOSPC:
			( void)FormAlert( 1 , get_string( NOSPC));
			break;					
		case EIO:
			( void)FormAlert( 1 , get_string( IO));
			break;		
		case EROFS:
			( void)FormAlert( 1 , get_string( ROFS));
			break;		
		case EFAULT:
			( void)FormAlert( 1 , get_string( FAULT));
			break;		
		case EINVAL:
			( void)FormAlert( 1 , get_string( INVAL));
			break;		
		case EBADF:
			( void)FormAlert( 1 , get_string( BADF));
			break;
		case ENOMEM:
			( void)FormAlert( 1 , get_string( NOMEM));
			break;
		case E_RSC:
 			( void)FormAlert( 1 , "[3][zview.rsc not found!][Quit]");
 			break;
		case NO_EDDI:
 			( void)FormAlert( 1 , "[3][zView needs a VDI|with EdDI standard.][Quit]");
              		break;
		case LDG_LIB_FULLED:
 			( void)FormAlert( 1 , get_string( LDG_LIB_FULLED), name);
              		break;
		case LDG_APP_FULLED:
 			( void)FormAlert( 1 , get_string( APP_FULLED), name);
              		break;
		case LDG_ERR_EXEC:
 			( void)FormAlert( 1 , get_string( ERR_EXEC), name);
              		break;
		case LDG_BAD_FORMAT:
 			( void)FormAlert( 1 , get_string( BAD_LFORMAT), name);
              		break;
		case LDG_LIB_LOCKED:
 			( void)FormAlert( 1 , get_string( LIB_LOCKED), name);
              		break;
		case LDG_NOT_FOUND:
 			( void)FormAlert( 1 , get_string( NOT_FOUND), name);
              		break;
		case LDG_NO_MEMORY:
 			( void)FormAlert( 1 , get_string( NO_MEMORY), name);
              		break;
		case LDG_NO_FUNC:
 			( void)FormAlert( 1 , get_string( NOFUNC), name);
              		break;
		case PLUGIN_DECODER_ABSENT: 
 			( void)FormAlert( 1 , get_string( PLUGIN_DECODER_ABSENT));
              		break;	
		case PLUGIN_ENCODER_ABSENT: 
 			( void)FormAlert( 1 , get_string( PLUGIN_ENCODER_ABSENT));
              		break;			
		case CANT_LOAD_IMG:
 			( void)FormAlert( 1 , get_string( CANT_LOAD_IMG), name);
              		break;
		case CANT_SAVE_IMG:
 			( void)FormAlert( 1 , get_string( CANT_SAVE_IMG));
              		break;
		case IMG_NO_VALID:
 			( void)FormAlert( 1 , get_string( IMG_NO_VALID));
              		break;	
		case SOURCE_TARGET_SAME:
 			( void)FormAlert( 1 , get_string( SOURCE_TARGET_SAME));
              		break;	
		case NOZCODECS:
 			( void)FormAlert( 1 , get_string( NOZCODECS));
              		break;	
		case NO_ICON:
 			( void)FormAlert( 1 , get_string( NOICONS));
              		break;	      		
		default:
			( void)FormAlert( 1 , get_string( GENERIC));
			break;				
	}
} 






