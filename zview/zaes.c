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


void __CDECL generic_form_event( WINDOW *win EVNT_BUFF_PARAM)
{
	ObjcChange( OC_FORM, win, EVNT_BUFF[4], NORMAL, FALSE);
	ApplWrite( _AESapid, WM_CLOSED, win->handle, 0, 0, 0, 0);
}


OBJECT *get_tree( int16 obj_index) 
{
	OBJECT *tree;
	RsrcGaddr( NULL, 0, obj_index, &tree);
	return tree;
}


/* Return a string from resource file */

__attribute__((noinline))
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

		vsf_color( WIN_GRAF_HANDLE(win), WHITE);
		v_bar( WIN_GRAF_HANDLE(win), xy);

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

		vsl_color( WIN_GRAF_HANDLE(win), BLACK);
		v_pline( WIN_GRAF_HANDLE(win), 5, xy);

		return;
	}
	else if(( source->fd_w < 128) || ( source->fd_h < 128))
	{	
		xy[0] = pblk->pb_x;
		xy[1] = pblk->pb_y;
		xy[2] = xy[0] + 127;
		xy[3] = xy[1] + 127;

		vsf_color( WIN_GRAF_HANDLE(win), WHITE);
		v_bar( WIN_GRAF_HANDLE(win), xy);
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
		vrt_cpyfm( WIN_GRAF_HANDLE(win), MD_REPLACE, xy, source, &screen, wanted_color);
	else
		vro_cpyfm( WIN_GRAF_HANDLE(win), S_ONLY, xy, source, &screen);
		
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

	vsl_color( WIN_GRAF_HANDLE(win), BLACK);
	v_pline( WIN_GRAF_HANDLE(win), 5, xy);
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
		vrt_cpyfm( WIN_GRAF_HANDLE(win), MD_REPLACE, xy, source, &screen, wanted_color);
	else
		vro_cpyfm( WIN_GRAF_HANDLE(win), S_ONLY, xy, source, &screen);		
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
		vrt_cpyfm( WIN_GRAF_HANDLE(win), MD_REPLACE, xy, source, &screen, wanted_color);
	else
		vro_cpyfm( WIN_GRAF_HANDLE(win), S_ONLY, xy, source, &screen);	
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
		vrt_cpyfm( WIN_GRAF_HANDLE(win), MD_REPLACE, xy, source, &screen, wanted_color);
	else
		vro_cpyfm( WIN_GRAF_HANDLE(win), S_ONLY, xy, source, &screen);	
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
		vrt_cpyfm( WIN_GRAF_HANDLE(win), MD_REPLACE, xy, source, &screen, wanted_color);
	else
		vro_cpyfm( WIN_GRAF_HANDLE(win), S_ONLY, xy, source, &screen);	
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
		vrt_cpyfm( WIN_GRAF_HANDLE(win), MD_REPLACE, xy, source, &screen, wanted_color);
	else
		vro_cpyfm( WIN_GRAF_HANDLE(win), S_ONLY, xy, source, &screen);	
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
		vrt_cpyfm( WIN_GRAF_HANDLE(win), MD_REPLACE, xy, source, &screen, wanted_color);
	else
		vro_cpyfm( WIN_GRAF_HANDLE(win), S_ONLY, xy, source, &screen);	
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
		vrt_cpyfm( WIN_GRAF_HANDLE(win), MD_REPLACE, xy, source, &screen, wanted_color);
	else
		vro_cpyfm( WIN_GRAF_HANDLE(win), S_ONLY, xy, source, &screen);	
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
		vrt_cpyfm( WIN_GRAF_HANDLE(win), MD_REPLACE, xy, source, &screen, wanted_color);
	else
		vro_cpyfm( WIN_GRAF_HANDLE(win), S_ONLY, xy, source, &screen);	
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
		vrt_cpyfm( WIN_GRAF_HANDLE(win), MD_REPLACE, xy, source, &screen, wanted_color);
	else
		vro_cpyfm( WIN_GRAF_HANDLE(win), S_ONLY, xy, source, &screen);	
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
		vrt_cpyfm( WIN_GRAF_HANDLE(win), MD_REPLACE, xy, source, &screen, wanted_color);
	else
		vro_cpyfm( WIN_GRAF_HANDLE(win), S_ONLY, xy, source, &screen);	
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
		vrt_cpyfm( WIN_GRAF_HANDLE(win), MD_REPLACE, xy, source, &screen, wanted_color);
	else
		vro_cpyfm( WIN_GRAF_HANDLE(win), S_ONLY, xy, source, &screen);	
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
		vrt_cpyfm( WIN_GRAF_HANDLE(win), MD_REPLACE, xy, source, &screen, wanted_color);
	else
		vro_cpyfm( WIN_GRAF_HANDLE(win), S_ONLY, xy, source, &screen);	
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
		vrt_cpyfm( WIN_GRAF_HANDLE(win), MD_REPLACE, xy, source, &screen, wanted_color);
	else
		vro_cpyfm( WIN_GRAF_HANDLE(win), S_ONLY, xy, source, &screen);	
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

	vsf_color( WIN_GRAF_HANDLE(win), LWHITE);
	v_bar( WIN_GRAF_HANDLE(win), pxy);

	tree->ob_x = x + (( w - 32) >> 1);
	tree->ob_y = y + (( h - 32) >> 1);

	objc_draw_grect( tree, MINI_ICON_SNIPE, 0, WIN_GRAF_CLIP(win));	
}



void errshow( const char *name, int16 error)
{
  	switch( error)
  	{
		case -ENAMETOOLONG:
			( void)FormAlert( 1 , get_string( NAMETOOLONG));
			break;
		case -ENOENT:
			( void)FormAlert( 1 , get_string( NOENT), name);
			break;				
		case -EACCES:
			( void)FormAlert( 1 , get_string( ACCES));
			break;		
		case -ELOOP:
			( void)FormAlert( 1 , get_string( LOOP));
			break;		
		case -ENOTDIR:
			( void)FormAlert( 1 , get_string( NOTDIR));
			break;								
		case -EXDEV:
			( void)FormAlert( 1 , get_string( XDEV));
			break;		
		case -ENOSPC:
			( void)FormAlert( 1 , get_string( NOSPC));
			break;					
		case -EIO:
			( void)FormAlert( 1 , get_string( IO));
			break;		
		case -EROFS:
			( void)FormAlert( 1 , get_string( ROFS));
			break;		
		case -EFAULT:
			( void)FormAlert( 1 , get_string( FAULT));
			break;		
		case -EINVAL:
			( void)FormAlert( 1 , get_string( INVAL));
			break;		
		case -EBADF:
			( void)FormAlert( 1 , get_string( BADF));
			break;
		case -ENOMEM:
			( void)FormAlert( 1 , get_string( NOMEM));
			break;
		case -EEXIST:
			( void)FormAlert( 1 , get_string( NAMEALREADYUSED));
			break;		

		case LDG_ERR_BASE + LDG_LIB_FULLED:
 			( void)FormAlert( 1 , get_string( LDG_LIB_FULLED), name);
              		break;
		case LDG_ERR_BASE + LDG_APP_FULLED:
 			( void)FormAlert( 1 , get_string( APP_FULLED), name);
              		break;
		case LDG_ERR_BASE + LDG_ERR_EXEC:
 			( void)FormAlert( 1 , get_string( ERR_EXEC), name);
              		break;
		case LDG_ERR_BASE + LDG_BAD_FORMAT:
 			( void)FormAlert( 1 , get_string( BAD_LFORMAT), name);
              		break;
		case LDG_ERR_BASE + LDG_LIB_LOCKED:
 			( void)FormAlert( 1 , get_string( LIB_LOCKED), name);
              		break;
		case LDG_ERR_BASE + LDG_NOT_FOUND:
 			( void)FormAlert( 1 , get_string( NOT_FOUND), name);
              		break;
		case LDG_ERR_BASE + LDG_NO_MEMORY:
 			( void)FormAlert( 1 , get_string( NO_MEMORY), name);
              		break;
		case LDG_ERR_BASE + LDG_NO_FUNC:
 			( void)FormAlert( 1 , get_string( NOFUNC), name);
              		break;

		case E_RSC:
 			( void)FormAlert( 1 , "[3][zview.rsc not found!][Quit]");
 			break;

		case ALERT_WINDOW:
		case PLUGIN_DECODER_ABSENT: 
		case PLUGIN_ENCODER_ABSENT: 
		case CANT_LOAD_IMG:
		case CANT_SAVE_IMG:
		case IMG_NO_VALID:
		case SOURCE_TARGET_SAME:
		case NOZCODECS:
		case NOICONS:
		case NO_EDDI:
			( void)FormAlert( 1 , get_string(error), name);
			break;

		default:
			( void)FormAlert( 1 , get_string( GENERIC));
			break;				
	}
}
