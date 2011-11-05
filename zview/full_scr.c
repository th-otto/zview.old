#include "general.h"
#include "pic_load.h"
#include "winimg.h"
#include "mfdb.h"
#include "pic_resize.h"

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
	v_show_c( app.handle, 0);
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

	vswr_mode( app.handle, MD_REPLACE);
	form_dial( FMD_START, 0, 0, 1 + app.work_out[0], 1 + app.work_out[1], 0, 0, 1 + app.work_out[0], 1 + app.work_out[1]);
	vsf_perimeter( app.handle, 0);
	vsf_interior( app.handle, FIS_SOLID);
	xy[0] = xy[1] = 0;
	xy[2] = app.work_out[0];
	xy[3] = app.work_out[1];
	v_hide_c( app.handle);
	vsf_color( app.handle, BLACK);
	vr_recfl( app.handle, xy);
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
	MFDB 	*out, resized_out = { NULL, app.work_out[0], app.work_out[1], 0, 0, 0, 0, 0, 0}, screen = {0};
	int16	posx, posy, xy[8];
	IMAGE 	*img = &windata->img;
/*	int16   dum;
	char	info[256];
 */

	full_screen = TRUE;
 
	out = &img->image[windata->page_to_show];

	if( out->fd_w > app.work_out[0] || out->fd_h > app.work_out[1])
	{
		if( pic_resize( &img->image[windata->page_to_show], &resized_out) == 0)
		{
			errshow( "", ENOMEM);
			full_screen = FALSE;
			return;
		}

		out = &resized_out;
	}

	clear_screen();
	wind_update(BEG_UPDATE);
 
	posx    = MAX( ( 1 + app.work_out[0] - out->fd_w) >> 1, 0);
	posy 	= MAX( ( 1 + app.work_out[1] - out->fd_h) >> 1, 0);

	if (posx == 0)
		xy[2] = app.work_out[0];
	else
		xy[2] = out->fd_w - 1;
  
	if (posy == 0)
		xy[3] = app.work_out[1];
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

		vrt_cpyfm( app.handle, MD_REPLACE, xy, out, &screen, color);
	}
	else
		vro_cpyfm( app.handle, S_ONLY, xy, out, &screen);
	
	/* and now the image's information */
	/* sprintf( info, "%dx%dx%d | %ld colors | %s", img->img_w, img->img_h, img->bits, img->colors, img->info);

	vst_alignment( app.handle, 2, 4, &dum, &dum);
	vst_color( app.handle, BLACK);
	vst_point( app.handle, 8, &dum, &dum, &dum, &dum);
	v_ftext( app.handle, app.work_out[0] - 4, app.work_out[1] - 4, info);
	*/

	/* We wait for a keyboard event and after, restore the screen */
	evnt_keybd();

	if( resized_out.fd_addr != NULL)
		gfree( resized_out.fd_addr);

	restore_screen();
	wind_update( END_UPDATE);
	full_screen = FALSE;
}
