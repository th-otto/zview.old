#include "general.h"
#include "pic_load.h"
#include "winimg.h"
#include "mfdb.h"
#include "pic_resize.h"
#include "full_scr.h"

int16 full_screen = FALSE;

/*==================================================================================*
 * void restore_screen:																*
 *		Close the form dialog created previously with clear_screen() function and	*
 *		restore the menubar and the mouse.											*
 *----------------------------------------------------------------------------------*
 * input:																			*
 *		--																			*
 *----------------------------------------------------------------------------------*
 * returns: 																		*
 *      --																			*
 *==================================================================================*/
void restore_screen( void)
{
	form_dial( FMD_FINISH, 0, 0, 1 + app.work_out[0], 1 + app.work_out[1], 0, 0, 1 + app.work_out[0], 1 + app.work_out[1]);
	menu_bar( app.menu, 1) ;
	v_show_c( APP_GRAF_HANDLE, 0);
}


/*==================================================================================*
 * void clear_screen:																*
 *		Open a full size form dialog and hide the mouse.. 							*
 *		It's the only way that I know for work in pseudo-fullscreen and remain 		*
 *		"GEM friendly".																*
 *----------------------------------------------------------------------------------*
 * input:																			*
 *		--																			*
 *----------------------------------------------------------------------------------*
 * returns: 																		*
 *      --																			*
 *==================================================================================*/
void clear_screen( void)
{
	int16 xy[8];

	vswr_mode( APP_GRAF_HANDLE, MD_REPLACE);
	form_dial( FMD_START, 0, 0, 1 + app.work_out[0], 1 + app.work_out[1], 0, 0, 1 + app.work_out[0], 1 + app.work_out[1]);
	vsf_perimeter( APP_GRAF_HANDLE, 0);
	vsf_interior( APP_GRAF_HANDLE, FIS_SOLID);
	xy[0] = xy[1] = 0;
	xy[2] = app.work_out[0];
	xy[3] = app.work_out[1];
	v_hide_c( APP_GRAF_HANDLE);
	vsf_color( APP_GRAF_HANDLE, BLACK);
	vr_recfl( APP_GRAF_HANDLE, xy);
}

/*==================================================================================*
 * void clear_screen:																*
 *		Show a MFDB in fullscreen.. We wait for a keyboard event before to release. *
 *----------------------------------------------------------------------------------*
 * input:																			*
 *		img			->		The MFDB to show.										*
 *----------------------------------------------------------------------------------*
 * returns: 																		*
 *      --																			*
 *==================================================================================*/
void show_fullscreen( WINDATA *windata)
{
	int16	screenw = app.work_out[0] + 1;	// bottom-right X
	int16	screenh = app.work_out[1] + 1;	// bottom-right Y
	MFDB 	*out, resized_out = { NULL, screenw, screenh, 0, 0, 0, 0, 0, 0}, screen = {0};
	int16	posx, posy, xy[8];
	IMAGE 	*img = &windata->img;
/*	int16   dum;
	char	info[256];
 */

	full_screen = TRUE;
 
	out = &img->image[windata->page_to_show];

	if( out->fd_w > screenw || out->fd_h > screenh)
	{
		if( pic_resize( &img->image[windata->page_to_show], &resized_out) == 0)
		{
			errshow(NULL, -ENOMEM);
			full_screen = FALSE;
			return;
		}

		out = &resized_out;
	}

	clear_screen();
	wind_update(BEG_UPDATE);
	
	posx    = MAX( ( 1 + screenw - out->fd_w) >> 1, 0);
	posy 	= MAX( ( 1 + screenh - out->fd_h) >> 1, 0);

	if (posx == 0)
		xy[2] = screenw - 1;
	else
		xy[2] = out->fd_w - 1;
  
	if (posy == 0)
		xy[3] = screenh - 1;
	else
		xy[3] = out->fd_h - 1;

	xy[0] = 0; 
	xy[1] = 0;
	xy[4] = posx; 
	xy[5] = posy;
	xy[6] = posx + xy[2];
	xy[7] = posy + xy[3];
	
	/* draw the image */
	if ( out->fd_nplanes == 1)
	{
		int16	color[2] = { BLACK, WHITE};

		vrt_cpyfm( APP_GRAF_HANDLE, MD_REPLACE, xy, out, &screen, color);
	}
	else
		vro_cpyfm( APP_GRAF_HANDLE, S_ONLY, xy, out, &screen);
	
	/* and now the image's information */
#if 0
	sprintf( info, "%dx%dx%d | %ld colors | %s", img->img_w, img->img_h, img->bits, img->colors, img->info);

	vst_alignment( APP_GRAF_HANDLE, 2, 4, &dum, &dum);
	vst_color( APP_GRAF_HANDLE, BLACK);
	vst_point( APP_GRAF_HANDLE, 8, &dum, &dum, &dum, &dum);
	v_ftext( APP_GRAF_HANDLE, app.work_out[0] - 4, app.work_out[1] - 4, info);
#endif

	/* We wait for a keyboard event and after, restore the screen */
	evnt_keybd();

	if( resized_out.fd_addr != NULL)
		free( resized_out.fd_addr);

	restore_screen();
	wind_update( END_UPDATE);
	full_screen = FALSE;
}
