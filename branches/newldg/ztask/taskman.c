#include "general.h"
#include "win.h"
#include "process.h"
#include "gmem.h"
#include "string.h"
#include <math.h>
#include <signal.h>

WINDOW 	*taskman_win = NULL;
static void *thumb = NULL;
process *process_loop, *process_selected = NULL;
MFDB cpu_average_background = {0}, process_list_buffer = {0};
static int16 wanted_color[2] = { BLACK, LGREEN}, txt_color[2] = { BLACK, WHITE}, selected_txt_color[2] = { BLACK, LWHITE}, xy[8], x, i, ii, dum, res, cpu_history_point[202];
static OBJECT *dial = NULL;
uint32 old_stram, old_ttram, process_list_buffer_lenght;
int32 uptime, loadaverage[3], updays, tmpdays, uphours, tmphours, upmins, upsecs, ypos, ypos_max;
static MFDB screen = {0}; 
float slider_pos = 0.0, slider_step = 0.0;


/*==================================================================================*
 * int16 init_mfdb:																	*
 *				fit a MFDB with the parameters suppplied.							*
 *----------------------------------------------------------------------------------*
 * input:																			*
 * 		bm		-> 	The MFDB to fit.												*
 * 		width	-> 	Image's width. 													*
 * 		height	-> 	Image's height.										 			*
 * 		planes	->	Screen planes or 1 if the image is monochrome.					*
 *----------------------------------------------------------------------------------*
 * returns: 																		*	
 *		'0' if error or the memory size allocated.									*
 *==================================================================================*/
uint32 init_mfdb( MFDB *bm, int16 width, int16 height, int16 planes)
{
	uint32 length;

	bm->fd_w 		= width;
	bm->fd_h 		= height;
	bm->fd_wdwidth	= ( width >> 4) + (( width % 16) != 0);
	bm->fd_stand 	= 0;
	bm->fd_nplanes 	= planes;
	bm->fd_r1      	= 0;
	bm->fd_r2      	= 0;
	bm->fd_r3	  	= 0;

	length			= (( (( uint32)bm->fd_wdwidth << 1 ) * ( uint32)bm->fd_nplanes) * ( uint32)bm->fd_h) + 256L;
	bm->fd_addr 	= gmalloc( length);

	if( bm->fd_addr)
		return( length - 256);
	
	bm->fd_addr = NULL;

	return ( 0);
}


static void CDECL taskman_win_mouse_event( WINDOW *win, int16 buff[8])
{
	int16 ob_x, ob_y;
	
	if( FormThbGet( thumb, 0) != TASK_PROCESS)
		return;
		
	mt_objc_offset( FORM( win), TASK_PROCESS_LIST, &ob_x, &ob_y, app.aes_global);		
		
	if( !IS_IN( evnt.mx, evnt.my, ob_x, ob_y, dial[TASK_PROCESS_LIST].ob_width, dial[TASK_PROCESS_LIST].ob_height))
		return;

	evnt.my -= ( ob_y + 15);

	process_loop = process_root;	

	while( process_loop != NULL)
	{
		if(( process_loop->y_pos > evnt.my) && ( process_loop->y_pos < evnt.my + 13))
		{
			process_selected = process_loop;
			break;
		}
	
		process_loop = process_loop->next;
	}	

	if( process_selected)
	{
		ObjcDraw( OC_FORM, taskman_win, TASK_PROCESS_LIST, 1);
	
		if( dial[TASK_KILL].ob_state & DISABLED)
		{
			ObjcChange( OC_FORM, win, TASK_KILL, ~DISABLED, TRUE);
		}
	}
				
}


void CDECL draw_process_list( WINDOW *win, PARMBLK *pblk, void *data)
{
	memset( process_list_buffer.fd_addr, 0, process_list_buffer_lenght);

	i = 0;  
	process_loop = process_root;
	
	
	while( process_loop != NULL)
	{
		process_loop->y_pos = i - ( ypos * 13);
		
		if(( process_loop->y_pos >= pblk->pb_h - 12) || ( process_loop->y_pos < 0))
		{
			process_loop = process_loop->next;
			i += 13;
			continue;
		}
				
		draw_text_to_buffer( 2, process_loop->y_pos + 3, process_list_buffer.fd_wdwidth << 1, process_loop->name, &process_list_buffer);
		draw_text_to_buffer( 183, process_loop->y_pos + 3, process_list_buffer.fd_wdwidth << 1, process_loop->cpu_usage, &process_list_buffer);		
		draw_text_to_buffer( pblk->pb_w - ( process_loop->ram_usage_txt_width + 4), process_loop->y_pos + 3, process_list_buffer.fd_wdwidth << 1, process_loop->ram_usage, &process_list_buffer);
			
		i += 13;
		
		process_loop = process_loop->next;
	}	
		
	xy[0] = 0;
	xy[1] = 0;
	xy[2] = pblk->pb_w - 1;
	xy[3] = pblk->pb_h - 1;
	xy[4] = pblk->pb_x;
	xy[5] = pblk->pb_y;
	xy[6] = pblk->pb_x + pblk->pb_w - 1;
	xy[7] = pblk->pb_y + pblk->pb_h - 1;
		
	vrt_cpyfm( win->graf->handle, MD_REPLACE, xy, &process_list_buffer, &screen, txt_color);
	
	if( process_selected != NULL)
	{
		if(( process_selected->y_pos >= pblk->pb_h - 12) || ( process_selected->y_pos < 0))
			return;		
		
		xy[0] = 0;
		xy[1] = process_selected->y_pos;
		xy[2] = pblk->pb_w - 1;
		xy[3] = xy[1] + 12;
		xy[4] = pblk->pb_x;
		xy[5] = pblk->pb_y + process_selected->y_pos;
		xy[6] = pblk->pb_x + pblk->pb_w - 1;
		xy[7] = xy[5] + 12;
		vrt_cpyfm( win->graf->handle, MD_REPLACE, xy, &process_list_buffer, &screen, selected_txt_color);
	}	
}

static void calc_process_slider( WINDOW *win)
{
	int16 max_mover_size;
	int16 full_win_size = ypos_max * 13;

	max_mover_size 	= dial[TASK_VBACK].ob_height;

	dial[TASK_VMOVER].ob_y 	   = 0;
	dial[TASK_VMOVER].ob_height = max_mover_size;
		
	if ( dial[TASK_PROCESS_LIST].ob_height < full_win_size)
	{	
		float factor 			= ( float)dial[TASK_PROCESS_LIST].ob_height / ( float)full_win_size;
		float mover_size 		= MAX(( float)max_mover_size * factor, 6L);
		
		slider_pos = 0;
		slider_step = ( float)max_mover_size / ( float)ypos_max;
		dial[TASK_VMOVER].ob_height	= ( int16)( mover_size);
	}	
}




static void CDECL taskman_dialog_kill( WINDOW *win, int obj, int mode, void *data)
{
	if( process_selected == NULL)
		return;
		
	Pkill( process_selected->pid, SIGKILL);	
	ObjcChange( mode, win, obj, ~SELECTED, TRUE);
	ObjcChange( mode, win, obj, DISABLED, TRUE);
}



static void CDECL slid_up( WINDOW *win, int obj, int mode, void *data)
{
	ObjcChange( mode, win, obj, SELECTED, TRUE);
	
	do 
	{
		if ( ypos > 0L) 
		{
			ypos--;
			ObjcDraw( OC_FORM, win, TASK_PROCESS_LIST, 1);
			slider_pos -= slider_step;
			dial[TASK_VMOVER].ob_y	= MAX( 0, ( int16)floor( slider_pos));
			ObjcDraw( OC_FORM, win, TASK_VBACK, 2);
			mt_evnt_timer( 100L, app.aes_global);  
		}
		graf_mkstate( &dum, &dum, &res, &dum);

	} while( res);

	ObjcChange( mode, win, obj, ~SELECTED, TRUE);
}


static void CDECL slid_down( WINDOW *win, int obj, int mode, void *data) 
{
	ObjcChange( mode, win, obj, SELECTED, TRUE);

	do 
	{
		if (( ypos < ( ypos_max - dial[TASK_PROCESS_LIST].ob_height / 13)) && ( ypos_max  > dial[TASK_PROCESS_LIST].ob_height / 13)) 
		{
			ypos ++;
			ObjcDraw( OC_FORM, win, TASK_PROCESS_LIST, 1);
			slider_pos += slider_step;
			dial[TASK_VMOVER].ob_y	= ( int16)floor( slider_pos);
			ObjcDraw( OC_FORM, win, TASK_VBACK, 2);
			mt_evnt_timer( 100L, app.aes_global);  
		}
		graf_mkstate( &dum, &dum, &res, &dum);

	} while( res);

	ObjcChange( mode, win, obj, ~SELECTED, TRUE);
}


static void CDECL slid_vmover( WINDOW *win, int obj, int mode, void *data) 
{
	int32 old_ypos = ypos;

	ObjcChange( mode, win, obj, SELECTED, TRUE);

	graf_mouse( FLAT_HAND, NULL);

	while( !wind_update( BEG_MCTRL));

	res = graf_slidebox( dial, TASK_VBACK, TASK_VMOVER, 1);

	wind_update( END_MCTRL);

	ypos = MAX( 0, ( ypos_max - dial[TASK_PROCESS_LIST].ob_height / 13) * res / 1000L);

	if( old_ypos != ypos)
	{
		slider_pos = ypos * slider_step;
		dial[TASK_VMOVER].ob_y	= ( int16)floor( slider_pos);
		ObjcDraw( OC_FORM, win, TASK_VBACK, 2);	
		ObjcDraw( OC_FORM, win, TASK_PROCESS_LIST, 1);
	}

	graf_mouse( ARROW, NULL);

	ObjcChange( mode, win, obj, ~SELECTED, TRUE);
}




static void CDECL draw_cpu_average( WINDOW *win, PARMBLK *pblk, void *data) 
{
	xy[0] = xy[1] = 0;								
	xy[2] = 300;		
	xy[3] = 100;	
	xy[4] = pblk->pb_x;	
	xy[5] = pblk->pb_y;	
	xy[6] = xy[4] + 300;								
	xy[7] = xy[5] + 100;
	
	vrt_cpyfm( win->graf->handle, MD_REPLACE, xy, &cpu_average_background, &screen, wanted_color);

	x = pblk->pb_x;
			
	for( i = 0, ii = 0; i < 101; x += 3)
	{
		cpu_history_point[ii++] = x;
		cpu_history_point[ii++] = pblk->pb_y + cpu_history[i++];		
	}		
	
	vsl_color( win->graf->handle, CYAN);
	v_pline( win->graf->handle, 101, cpu_history_point);
}

static void CDECL draw_name_button( WINDOW *win, PARMBLK *pblk, void *data) 
{
	xy[0] = pblk->pb_x;	
	xy[1] = pblk->pb_y;	
	xy[2] = xy[0] + pblk->pb_w - 1;
	xy[3] = xy[1] + pblk->pb_h - 1;			
			
	vsf_color( win->graf->handle, LWHITE);
	v_bar( win->graf->handle, xy);		

	xy[0] = pblk->pb_x;	
	xy[1] = pblk->pb_y + pblk->pb_h - 1;	
	xy[2] = xy[0] + pblk->pb_w - 1;
	xy[3] = xy[1];	
	xy[4] = xy[2];
	xy[5] = pblk->pb_y;	
			
	vsl_color( win->graf->handle, BLACK);
	v_pline( win->graf->handle, 3, xy);
	
	xy[0] = pblk->pb_x;	
	xy[1] = pblk->pb_y + pblk->pb_h - 2;	
	xy[2] = xy[0] + pblk->pb_w - 2;
	xy[3] = xy[1];	
	xy[4] = xy[2];
	xy[5] = pblk->pb_y;	
			
	vsl_color( win->graf->handle, LBLACK);
	v_pline( win->graf->handle, 3, xy);	
	
	xy[0] = pblk->pb_x;	
	xy[1] = pblk->pb_y + pblk->pb_h - 2;	
	xy[2] = xy[0];
	xy[3] = pblk->pb_y;		
	xy[4] = xy[0] + pblk->pb_w - 2;
	xy[5] = pblk->pb_y;	
			
	vsl_color( win->graf->handle, WHITE);
	v_pline( win->graf->handle, 3, xy);	
	
	draw_text( win->graf->handle, pblk->pb_x + 5, pblk->pb_y + 3, BLACK, "Process name");	
	
}


static void CDECL draw_cpu_button( WINDOW *win, PARMBLK *pblk, void *data) 
{
	xy[0] = pblk->pb_x;	
	xy[1] = pblk->pb_y;	
	xy[2] = xy[0] + pblk->pb_w - 1;
	xy[3] = xy[1] + pblk->pb_h - 1;			
			
	vsf_color( win->graf->handle, LWHITE);
	v_bar( win->graf->handle, xy);		

	xy[0] = pblk->pb_x;	
	xy[1] = pblk->pb_y + pblk->pb_h - 1;	
	xy[2] = xy[0] + pblk->pb_w - 1;
	xy[3] = xy[1];	
	xy[4] = xy[2];
	xy[5] = pblk->pb_y;	
			
	vsl_color( win->graf->handle, BLACK);
	v_pline( win->graf->handle, 3, xy);
	
	xy[0] = pblk->pb_x;	
	xy[1] = pblk->pb_y + pblk->pb_h - 2;	
	xy[2] = xy[0] + pblk->pb_w - 2;
	xy[3] = xy[1];	
	xy[4] = xy[2];
	xy[5] = pblk->pb_y;	
			
	vsl_color( win->graf->handle, LBLACK);
	v_pline( win->graf->handle, 3, xy);	
	
	xy[0] = pblk->pb_x;	
	xy[1] = pblk->pb_y + pblk->pb_h - 2;	
	xy[2] = xy[0];
	xy[3] = pblk->pb_y;		
	xy[4] = xy[0] + pblk->pb_w - 2;
	xy[5] = pblk->pb_y;	
			
	vsl_color( win->graf->handle, WHITE);
	v_pline( win->graf->handle, 3, xy);	
	
	draw_text( win->graf->handle, pblk->pb_x + 25, pblk->pb_y + 3, BLACK, "CPU");	
	
}

static void CDECL draw_ram_button( WINDOW *win, PARMBLK *pblk, void *data) 
{
	xy[0] = pblk->pb_x;	
	xy[1] = pblk->pb_y;	
	xy[2] = xy[0] + pblk->pb_w - 1;
	xy[3] = xy[1] + pblk->pb_h - 1;			
			
	vsf_color( win->graf->handle, LWHITE);
	v_bar( win->graf->handle, xy);		

	xy[0] = pblk->pb_x;	
	xy[1] = pblk->pb_y + pblk->pb_h - 1;	
	xy[2] = xy[0] + pblk->pb_w - 1;
	xy[3] = xy[1];	
//	xy[4] = xy[2];
//	xy[5] = pblk->pb_y;	
			
	vsl_color( win->graf->handle, BLACK);
	v_pline( win->graf->handle, 2, xy);
	
	xy[0] = pblk->pb_x;	
	xy[1] = pblk->pb_y + pblk->pb_h - 2;	
	xy[2] = xy[0] + pblk->pb_w - 1;
	xy[3] = xy[1];	
	xy[4] = xy[2];
	xy[5] = pblk->pb_y;	
			
	vsl_color( win->graf->handle, LBLACK);
	v_pline( win->graf->handle, 3, xy);	
	
	xy[0] = pblk->pb_x;	
	xy[1] = pblk->pb_y + pblk->pb_h - 2;	
	xy[2] = xy[0];
	xy[3] = pblk->pb_y;		
	xy[4] = xy[0] + pblk->pb_w - 2;
	xy[5] = pblk->pb_y;	
			
	vsl_color( win->graf->handle, WHITE);
	v_pline( win->graf->handle, 3, xy);	
	
	draw_text( win->graf->handle, pblk->pb_x + 33, pblk->pb_y + 3, BLACK, "Memory Usage");	
	
}


static void CDECL taskman_dialog_close( WINDOW *win, int16 buff[8])
{
	ApplWrite( _AESapid, WM_DESTROY, win->handle, 0, 0, 0, 0);
	
//	zdebug( "close");
}

static void CDECL taskman_dialog_free_ressource( WINDOW *win, int16 buff[8])
{
	while( process_root != NULL)
		process_delete( process_root->pid); 

	if( process_list_buffer.fd_addr)
	{
		gfree( process_list_buffer.fd_addr);
		process_list_buffer.fd_addr = NULL;	
	}		

//	zdebug( "destroy");	
		
	taskman_win = NULL;
	thumb = NULL;		
}

void taskman_timer( void)
{	
	int old_process_nbr;

	
	switch( FormThbGet( thumb, 0))
	{
		case TASK_SYSTEM:
			if( old_stram != stram)
			{
				old_stram = stram;
				sprintf( ObjcString( dial, TASK_ST_FREE, NULL), "%ld", stram);
				ObjcDraw( OC_FORM, taskman_win, TASK_ST_FREE, 1); 	
			}		

			if( old_ttram != ttram)
			{
				old_ttram = ttram;
				sprintf( ObjcString( dial, TASK_TT_FREE, NULL), "%ld", ttram);
				ObjcDraw( OC_FORM, taskman_win, TASK_TT_FREE, 1); 	
			}	
		
			ObjcDraw( OC_FORM, taskman_win, TASK_CPU_AVERAGE, 1); 
			
			
			Suptime( &uptime, loadaverage);
			updays = uptime / 86400;
			tmpdays = updays * 86400;
			uphours = (uptime - tmpdays) / 3600;
			tmphours = uphours * 3600;
			upmins = (uptime - tmpdays - tmphours) / 60;
			upsecs = (uptime - tmpdays - tmphours - ( upmins * 60));
						
			sprintf( ObjcString( dial, TASK_UPTIME, NULL), "%ld days %ld hours %ld minutes %ld secs", updays, uphours, upmins, upsecs);
			ObjcDraw( OC_FORM, taskman_win, TASK_UPTIME, 1); 				
			break;

			
		case TASK_PROCESS:
			old_process_nbr  = process_nbr;
			scan_process();	
			
			if( old_process_nbr != process_nbr)
			{
				ypos 				= 0;
				ypos_max 			= process_nbr;
				calc_process_slider( taskman_win);
				ObjcDraw( OC_FORM, taskman_win, TASK_PROCESS_LIST, 1); 
				ObjcDraw( OC_FORM, taskman_win, TASK_VBACK, 2); 
			}	
			else		
				ObjcDraw( OC_FORM, taskman_win, TASK_PROCESS_LIST, 1); 
		
			if( dial[TASK_LED1].ob_state & SELECTED)
			{
				ObjcChange( OC_FORM, taskman_win, TASK_LED1, NORMAL, 1); 
				ObjcChange( OC_FORM, taskman_win, TASK_LED2, SELECTED, 1);
			}
			else if( dial[TASK_LED2].ob_state & SELECTED)
			{
				ObjcChange( OC_FORM, taskman_win, TASK_LED2, NORMAL, 1); 
				ObjcChange( OC_FORM, taskman_win, TASK_LED3, SELECTED, 1);
			}	
			else
			{
				ObjcChange( OC_FORM, taskman_win, TASK_LED1, SELECTED, 1); 
				ObjcChange( OC_FORM, taskman_win, TASK_LED3, NORMAL, 1);
			}						
			break;			
					
		default:
			break;	
	}
}


void taskman_dialog( void)
{
	BITBLK	*image;
	
	int	frms[] = { TASK_PANEL1,  TASK_PANEL2, TASK_PANEL3};
	int	buts[] = { TASK_PROCESS, TASK_SYSTEM, TASK_NETWORK};
	
	if( taskman_win != NULL)
	{
		return;
	}

	process_selected = NULL;	
			
	dial = get_tree( TASK);	

	sprintf( ObjcString( dial, TASK_ST_TOTAL, NULL), "%ld", total_stram);
	sprintf( ObjcString( dial, TASK_TT_TOTAL, NULL), "%ld", total_ttram);
	sprintf( ObjcString( dial, TASK_ST_FREE, NULL), "%ld", stram);
	sprintf( ObjcString( dial, TASK_TT_FREE, NULL), "%ld", ttram);

	old_stram = stram;
	old_ttram = ttram;

	Suptime( &uptime, loadaverage);
	updays = uptime / 86400;
	tmpdays = updays * 86400;
	uphours = (uptime - tmpdays) / 3600;
	tmphours = uphours * 3600;
	upmins = (uptime - tmpdays - tmphours) / 60;
	upsecs = (uptime - tmpdays - tmphours - ( upmins * 60));
	sprintf( ObjcString( dial, TASK_UPTIME, NULL), "%ld days %ld hours %ld minutes %ld secs", updays, uphours, upmins, upsecs);

	dial[TASK_NETWORK].ob_flags |= HIDETREE; 
	dial[TASK_KILL].ob_state |= DISABLED;
			
	if( ( taskman_win = FormCreate( dial, NAME|MOVER|CLOSER, NULL, get_string( S_TASKMAN), NULL, TRUE, FALSE)) == NULL)
		return;
		
	dial = FORM( taskman_win);	
		
	scan_process();	
		
	{
		ypos 				= 0;
		ypos_max 			= process_nbr;
		taskman_win->h_u 	= 13;
		
		calc_process_slider( taskman_win);

		ObjcAttachFormFunc( taskman_win, TASK_UP, slid_up, NULL);
		ObjcAttachFormFunc( taskman_win, TASK_DOWN, slid_down, NULL);
		ObjcAttachFormFunc( taskman_win, TASK_VMOVER, slid_vmover, NULL);
	}	
	
	thumb = FormThumb( taskman_win, frms, buts, 3);	

	
	process_list_buffer_lenght = init_mfdb( &process_list_buffer, dial[TASK_PROCESS_LIST].ob_width, dial[TASK_PROCESS_LIST].ob_height, 1);
	
	rsrc_gaddr( R_IMAGEDATA, CPU_AVERAGE, &image);
	
	cpu_average_background.fd_w 		= 304;
	cpu_average_background.fd_h 		= 101;
	cpu_average_background.fd_wdwidth	=  ( 304 >> 4) + (( 304 % 16) != 0);
	cpu_average_background.fd_stand 	= 0;
	cpu_average_background.fd_nplanes 	= 1;
	cpu_average_background.fd_r1      	= 0;
	cpu_average_background.fd_r2      	= 0;
	cpu_average_background.fd_r3	  	= 0;
	cpu_average_background.fd_addr 		= (void*)image->bi_pdata;

		
	RsrcUserDraw( OC_FORM, taskman_win, TASK_CPU_AVERAGE, draw_cpu_average, NULL);
	RsrcUserDraw( OC_FORM, taskman_win, TASK_PROCESS_LIST, draw_process_list, NULL);
	RsrcUserDraw( OC_FORM, taskman_win, TASK_C_NAME, draw_name_button, NULL);
	RsrcUserDraw( OC_FORM, taskman_win, TASK_C_CPU, draw_cpu_button, NULL);
	RsrcUserDraw( OC_FORM, taskman_win, TASK_C_RAM, draw_ram_button, NULL);
				
	EvntAttach( taskman_win, WM_CLOSED, taskman_dialog_close);
	EvntAdd( taskman_win, WM_XBUTTON, taskman_win_mouse_event, EV_BOT);
	EvntAdd( taskman_win, WM_DESTROY, taskman_dialog_free_ressource, EV_TOP);

	ObjcAttachFormFunc( taskman_win, TASK_KILL, taskman_dialog_kill, NULL);
}

