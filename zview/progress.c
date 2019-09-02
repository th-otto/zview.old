#include "general.h"
#include "ztext.h"
#include "progress.h"

/* Local variable */
WINDOW *win_read = NULL;


/*==================================================================================*
 * int16 win_read_progress_begin:													*
 *		Open the read's progress bar dialog ( if needed).							*
 *----------------------------------------------------------------------------------*
 * input:																			*
 * 		string	-> The text to show in the progress bar.							*
 *----------------------------------------------------------------------------------*
 * returns: 																		*
 *      --																			*
 *==================================================================================*/

void win_progress_begin( const char *string)
{
	OBJECT	*progress_bar = get_tree( SINGLE_PROGRESS);	
	TEDINFO *ted = progress_bar[SINGLE_PROGRESS_TXT].ob_spec.tedinfo;
	 
//	wind_update( BEG_MCTRL);

	zstrncpy( ted->te_ptext, string, ted->te_txtlen - 1);

	win_read = FormCreate( progress_bar, MOVER|NAME, NULL, "", NULL, 0, 0);

	/* form = ( W_FORM *)DataSearch( win_read, WD_WFRM); */

	vsf_color( WIN_GRAF_HANDLE(win_read), BLUE);
		
	EvntRedraw( win_read);
}


/*==================================================================================*
 * void win_read_progress:															*
 *		redraw the bar with the news coordinates computed.							*
 *----------------------------------------------------------------------------------*
 * input:																			*
 * 		new_bar_width -> the size of the progress bar.								*
 *----------------------------------------------------------------------------------*
 * returns: 																		*
 *      --																			*
 *==================================================================================*/

void win_progress( int16 new_bar_width)
{
	static int16 bar_width = 1;
	int16 	pxy[4];
	GRECT	rwin, raes;
	W_FORM  *form 			= ( W_FORM *)DataSearch( win_read, WD_WFRM);

	if ( bar_width == new_bar_width || !new_bar_width)
		return;

	WindGet( win_read, WF_WORKXYWH, &rwin.g_x, &rwin.g_y, &rwin.g_w, &rwin.g_h);

	bar_width = new_bar_width;

	pxy[0] = rwin.g_x + form->root[SINGLE_PROGRESS_CONTAINER].ob_x;
	pxy[1] = rwin.g_y + form->root[SINGLE_PROGRESS_CONTAINER].ob_y;
	pxy[2] = pxy[0] + bar_width - 1;
	pxy[3] = pxy[1] + form->root[SINGLE_PROGRESS_CONTAINER].ob_height - 1;

	wind_update( BEG_UPDATE);
	graf_mouse( M_OFF, 0L);
	wind_get( win_read->handle, WF_FIRSTXYWH, &raes.g_x, &raes.g_y, &raes.g_w, &raes.g_h);

	while ( raes.g_w && raes.g_h) 
	{
		if( rc_intersect( &rwin, &raes)) 
		{
			WinClipOn( win_read, &raes);

			v_bar( WIN_GRAF_HANDLE(win_read), pxy);

			WinClipOff( win_read);
		}
	
		wind_get( win_read->handle, WF_NEXTXYWH, &raes.g_x, &raes.g_y, &raes.g_w, &raes.g_h);
	}	
	
	graf_mouse( M_ON, 0L);

	wind_update( END_UPDATE);
}


/*==================================================================================*
 * void win_read_progress_end:														*
 *		Destroy the read's progress formular.										*
 *----------------------------------------------------------------------------------*
 * input:																			*
 * 		--																			*
 *----------------------------------------------------------------------------------*
 * returns: 																		*
 *      --																			*
 *==================================================================================*/

void win_progress_end( void)
{
	if ( win_read == NULL)
		return;

	snd_msg( win_read, WM_DESTROY, 0, 0, 0, 0);	

//	wind_update( END_MCTRL);

	win_read = NULL;
}
