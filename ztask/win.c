#include "general.h"
#include "app.h"
#include "cpu.h"
#include "popup.h"
#include "string.h"
#include "taskman.h"

static clock_t chrono_value, update_time = ( clock_t)( 100L);
int16 cpu_usage 				= 0;
int32 total_stram				= 0;
int32 total_ttram				= 0;
uint32 stram					= 0;
uint32 ttram					= 0;
int16 stram_percent				= 0;
int16 ttram_percent				= 0;
int16 bar_width					= 6;
int16 geek_area_width			= 64;
int16 cpu_history[101];
int16 x_pos, w_pos, y_pos, h_pos, geek_area_x, clock_x, cpu_x, st_x, tt_x; 
int16 hcell = 8, dum, y_text_pos, x_space = 5, menu_enabled = FALSE;
char xbios_time[15];

WINDOW *app_bar;
OBJECT *icons = NULL;
static void win_mouse_event( WINDOW *win, int16 buff[8]);
static void win_redraw_event( WINDOW *win, int16 buff[8]);


static void CDECL timer_function( WINDOW *win, int16 buff[8])
{
	app_data *tmp;	
	clock_t current_t;
	clock_t relative_t;
	time_t curtime;
	struct tm *loctime;
	int16 old_app_nbr = app_nbr;
	static int skip = 0;

	if( popup.win)
	{
		if( IS_IN( evnt.mx, evnt.my, popup.x_pos, popup.y_pos, popup.w_pos, popup.h_pos + 2))
		{		
			popup.selected = popup_item_under_mouse();
			
			if( popup.selected != popup.old_selected)
			{		
				if( popup.selected >= 0)
					draw_page( popup.win, popup.item_pos[popup.selected].x1, popup.item_pos[popup.selected].y1, popup.item_pos[popup.selected].x2, popup.item_pos[popup.selected].y2);
				
				if( popup.old_selected >= 0)
					draw_page( popup.win, popup.item_pos[popup.old_selected].x1, popup.item_pos[popup.old_selected].y1, popup.item_pos[popup.old_selected].x2, popup.item_pos[popup.old_selected].y2);
	
				popup.old_selected = popup.selected;
			}	
		}	
		else if( menu_enabled == TRUE)
		{
			if( !IS_IN( evnt.mx, evnt.my, root->x_pos - 24, root->y_pos, 24, 23))
				popup_delete( popup.win);
		}		
		else if( !IS_IN( evnt.mx, evnt.my, popup.entry->x_pos, popup.entry->y_pos, app_width, 23))
			popup_delete( popup.win);
	}

	current_t = clock();
	
	relative_t = current_t - chrono_value;

	if( relative_t < update_time)
		return;

	chrono_value = current_t;

	cpu_usage = cpu_get_usage();
	
	if( skip == 0)
	{
		memcpy( cpu_history, &cpu_history[1], 200);
		cpu_history[100] = 100 - cpu_usage;
		
		if( show_clock == TRUE)
		{
			curtime = time( NULL);
			loctime = localtime( &curtime);

			if( clock_us == TRUE)
				strftime( xbios_time, 12, "%I:%M:%S %p", loctime);
			else
				strftime( xbios_time, 12, "%H:%M:%S", loctime);
		}
		
		if( show_system_info)
		{
			get_free_ram( &stram, &ttram);
			stram_percent = (int16)(( 100 * stram) / total_stram);

			if( total_ttram != 0)
				ttram_percent = (int16)(( 100 * ttram) / total_ttram);
			
		}

		skip = 1;

		tmp = selected;
		
		app_data_search();

		if( old_app_nbr == app_nbr)	
		{
			draw_page( win, geek_area_x + 4, app.y + app.h - 19, geek_area_width - 9, 15);
		
			if( selected != tmp)
			{
				if( selected != NULL)
					draw_page( win, selected->x_pos, selected->y_pos, app_width, h_pos);
				
				if( old_selected != NULL)
					draw_page( win, old_selected->x_pos, old_selected->y_pos, app_width, h_pos);
			}
		}
		else
		{
			w_pos = geek_area_width + ( app_nbr * app_width) + 24;

			while( w_pos > app.w)
				w_pos -= app_width;

			x_pos = app.x + ( app.w - w_pos);
			w_pos -= 1;
						
			WindSet( win, WF_WORKXYWH, x_pos, y_pos, w_pos, h_pos);
			
			// Xaaes bug... with this AES, WindSet( win, WF_CURRXYWH,... doesn't redraw the taskbar.
			draw_page( win, x_pos, y_pos, w_pos, h_pos);
		}
		
		if( taskman_win != NULL)
			taskman_timer();
		
	}
	else
	{
		draw_page( win, geek_area_x + 4, app.y + app.h - 19, geek_area_width - 9, 15);
		skip = 0;
	}	
}



static void CDECL win_mouse_event( WINDOW *win, int16 buff[8])
{
	app_data *scan = root, *tmp = NULL;
	
//	evnt.nb_click = 0;

	while( scan != NULL)
	{	
		if( scan->x_pos == -1) 
			continue;

		if( IS_IN( evnt.mx, evnt.my, scan->x_pos, scan->y_pos, app_width, h_pos))
		{
			tmp = scan;
			break;
		}

		scan = scan->next;
	}	


	if( tmp != NULL)
	{
		int16 ap_cout;
	
		old_selected = selected;
		selected = tmp;
		
		if( old_selected != selected)
		{
			mt_appl_control( selected->id, APC_INFO, &ap_cout, app.aes_global);

			if( ap_cout & APCI_HIDDEN)
				mt_appl_control( selected->id, APC_SHOW, &ap_cout, app.aes_global);

			mt_appl_control( selected->id, APC_TOP, &ap_cout, app.aes_global);

			draw_page( win, selected->x_pos, selected->y_pos, app_width, h_pos);
			
			if( old_selected != NULL)
				draw_page( win, old_selected->x_pos, old_selected->y_pos, app_width, h_pos);
		}
	}

	if( popup.win)
		return;

	/* if we are on the menu icon or on an application item, we open a popup */				
	if(( tmp != NULL) && ( evnt.mbut == 2))
	{
		char dummy[MAXNAMLEN];
		uint32 dummy_var;

		popup.item_nbr = 0;	

		get_info_by_pid((int)tmp->pid, dummy, popup.item_name[popup.item_nbr], &dummy_var);

		popup.selectable[popup.item_nbr] = FALSE;
		popup.function[popup.item_nbr]   = 0;		
		popup.icon[popup.item_nbr++]     = ICONS_MEM;

		popup.selectable[popup.item_nbr]   = FALSE;
		popup.function[popup.item_nbr]     = 0;			
		popup.item_name[popup.item_nbr][0] = '\0';
		popup.icon[popup.item_nbr++]       = -1;

/*		popup.selectable[popup.item_nbr] = TRUE;
		popup.function[popup.item_nbr]   = F_HIDE;									
		strcpy( popup.item_name[popup.item_nbr], get_string( S_HIDE));
		popup.icon[popup.item_nbr++] = -1;
*/
		popup.selectable[popup.item_nbr] = TRUE;	
		popup.function[popup.item_nbr]   = F_QUIT;			
		strcpy( popup.item_name[popup.item_nbr], get_string( S_QUIT));
		popup.icon[popup.item_nbr++] = -1;				

		popup.selectable[popup.item_nbr] = TRUE;	
		popup.function[popup.item_nbr]   = F_KILL;											
		strcpy( popup.item_name[popup.item_nbr], get_string( S_KILL));
		popup.icon[popup.item_nbr++] = -1;

		open_popup( tmp);
	}
	else if(( tmp == NULL) && ( evnt.mx < root->x_pos) && ( evnt.mbut == 1))
	{
		int16 x, y;

		popup.item_nbr = 0;	

		popup.selectable[popup.item_nbr] = TRUE;
		popup.function[popup.item_nbr]   = F_TASKMANAGER;						
		strcpy( popup.item_name[popup.item_nbr], get_string( S_TASKMAN));
		popup.icon[popup.item_nbr++] = ICONS_TASK;

		popup.selectable[popup.item_nbr] = TRUE;	
		popup.function[popup.item_nbr]   = F_PREFERENCES;					
		strcpy( popup.item_name[popup.item_nbr], get_string( S_PREFS));
		popup.icon[popup.item_nbr++] = ICONS_CONFIGURE;		
		
		popup.selectable[popup.item_nbr] = FALSE;		
		popup.function[popup.item_nbr]   = 0;		
		popup.item_name[popup.item_nbr][0] = '\0';
		popup.icon[popup.item_nbr++] = -1;		

		popup.selectable[popup.item_nbr] = TRUE;	
		popup.function[popup.item_nbr]   = F_QUIT_ZBAR;			
		strcpy( popup.item_name[popup.item_nbr], get_string( S_QUIT));
		popup.icon[popup.item_nbr++] = ICONS_ARROW;				
		
		popup.selectable[popup.item_nbr] = TRUE;
		popup.function[popup.item_nbr]   = F_SHUTDOWN;				
		strcpy( popup.item_name[popup.item_nbr], get_string( S_SHUTDOWN));
		popup.icon[popup.item_nbr++] = ICONS_SHUTDOWN;	

		menu_enabled = TRUE;
		WindGet( win, WF_WORKXYWH, &x, &y, &dum, &dum);
		draw_page( win, x, y, 25, 24);

		open_popup( tmp);		
	}
}


static void CDECL win_redraw_event( WINDOW *win, int16 buff[8])
{
	int16 x_text_pos, xw, pxy[10], background, top_line, lower_line, title_shadow, title_color;
	app_data *scan = root;

	xw = x_pos;

	geek_area_x = xw + w_pos - geek_area_width;

	/* we draw the menu area */	
	pxy[0] = xw;
	pxy[1] = y_pos;
	pxy[2] = pxy[0] + 23;
	pxy[3] = pxy[1] + h_pos - 1;			
			
	if( menu_enabled == TRUE)
	{
		background	= ( int16)button_on_background;		
		top_line	= ( int16)button_on_dark_color;				
		lower_line	= ( int16)button_on_light_color;
		title_color = ( int16)button_on_text_color;
		title_shadow = ( int16)button_on_text_shadow_color;
	}
	else
	{
		background	= ( int16)button_off_background;		
		top_line	= ( int16)button_off_light_color;
		lower_line	= ( int16)button_off_dark_color;
		title_color = ( int16)button_off_text_color;
		title_shadow = ( int16)button_off_text_shadow_color;				
	}
			
	vsf_color( win->graf->handle, background);
	v_bar( win->graf->handle, pxy);			

	pxy[1] = pxy[3];
	pxy[2] = pxy[0];
	pxy[3] = y_pos;
	pxy[4] = pxy[0] + 22;
	pxy[5] = y_pos;

	vsl_color( win->graf->handle, top_line);
	v_pline( win->graf->handle, 3, pxy);
			
	pxy[0] = pxy[4] + 1;
	pxy[1] = y_pos;
	pxy[2] = pxy[0];
	pxy[3] = pxy[1] + h_pos - 1;
	pxy[4] = xw;
	pxy[5] = pxy[3];

	vsl_color( win->graf->handle, lower_line);
	v_pline( win->graf->handle, 3, pxy);

	icons[ICONS_MENU].ob_x = xw + 4;
	icons[ICONS_MENU].ob_y = y_pos + 2;

	mt_objc_draw( icons, ICONS_MENU, 1, win->graf->clip.g_x, win->graf->clip.g_y, win->graf->clip.g_w, win->graf->clip.g_h, app.aes_global);

	xw += 24;

	
	/* We draw the application list */
	while( scan != NULL)
	{
		if(( pxy[0] + app_width - 2) > geek_area_x)
		{
			scan->x_pos = -1;
			scan->y_pos = -1;
		}
		else
		{
			scan->x_pos = xw;
			scan->y_pos = y_pos;
			xw += app_width;

			pxy[0] = scan->x_pos;
			pxy[1] = y_pos;
			pxy[2] = pxy[0] + app_width - 1;
			pxy[3] = pxy[1] + h_pos - 1;			
			
			if( scan == selected)
			{
				background	= ( int16)button_on_background;		
				top_line	= ( int16)button_on_dark_color;				
				lower_line	= ( int16)button_on_light_color;
				title_color = ( int16)button_on_text_color;
				title_shadow = ( int16)button_on_text_shadow_color;
			}
			else
			{
				background	= ( int16)button_off_background;		
				top_line	= ( int16)button_off_light_color;
				lower_line	= ( int16)button_off_dark_color;
				title_color = ( int16)button_off_text_color;
				title_shadow = ( int16)button_off_text_shadow_color;				
			}			
			
			vsf_color( win->graf->handle, background);
			v_bar( win->graf->handle, pxy);			

			pxy[1] = pxy[3];
			pxy[2] = pxy[0];
			pxy[3] = y_pos;
			pxy[4] = pxy[0] + app_width - 2;
			pxy[5] = y_pos;

			vsl_color( win->graf->handle, top_line);
			v_pline( win->graf->handle, 3, pxy);
			
			pxy[0] = pxy[4] + 1;
			pxy[1] = y_pos;
			pxy[2] = pxy[0];
			pxy[3] = pxy[1] + h_pos - 1;
			pxy[4] = scan->x_pos;
			pxy[5] = pxy[3];
				
			vsl_color( win->graf->handle, lower_line);
			v_pline( win->graf->handle, 3, pxy);	

			x_text_pos = scan->x_pos + (( app_width - scan->name_shown_width) >> 1);

			draw_text( win->graf->handle, x_text_pos + 1, y_text_pos + 1, ( int16)title_shadow, scan->name_shown);
			draw_text( win->graf->handle, x_text_pos, y_text_pos, ( int16)title_color, scan->name_shown);
			
		}
		
		scan = scan->next;
	}	

	/* draw the "geek" area */
	pxy[0] = xw + 2;
	pxy[1] = y_pos + 2;
	pxy[2] = pxy[0] + geek_area_width - 6;
	pxy[3] = pxy[1] + h_pos - 5;

	vsf_color( win->graf->handle, geek_area_color);
	v_bar( win->graf->handle, pxy);

	pxy[0] = xw;
	pxy[1] = y_pos;
	pxy[2] = pxy[0];
	pxy[3] = pxy[1] + h_pos - 1;	
	pxy[4] = pxy[2] + geek_area_width - 2;
	pxy[5] = pxy[3];
	pxy[6] = pxy[4];
	pxy[7] = pxy[1];	
	pxy[8] = pxy[0];
	pxy[9] = pxy[1];
				
	vsl_color( win->graf->handle, ( int16)button_off_background);
	v_pline( win->graf->handle, 5, pxy);	

	pxy[0]++;
	pxy[1] = pxy[3] - 1;
	pxy[2] = pxy[0];
	pxy[3] = y_pos + 1;	
	pxy[4]--;
	pxy[5] = pxy[3];
				
	vsl_color( win->graf->handle, ( int16)geek_area_dark_line);
	v_pline( win->graf->handle, 3, pxy);		
	
	pxy[2] = pxy[4];
	pxy[3] = pxy[1];	
	pxy[5] = y_pos + 2;
				
	vsl_color( win->graf->handle, ( int16)geek_area_light_line);
	v_pline( win->graf->handle, 3, pxy);	
	
	
	/* draw the cpu % in the "geek" area */
	if( show_system_info)
	{
		/* the CPU usage */
		pxy[0] = cpu_x + geek_area_x;
		pxy[1] = y_pos + 3;
		pxy[2] = pxy[0] + bar_width - 1;
		pxy[3] = pxy[1] + 14;

		vsf_color( win->graf->handle, BLACK);
		v_bar( win->graf->handle, pxy);
			
		pxy[0] = pxy[0] + 1;
		pxy[2] = pxy[2] - 1;
		pxy[3] = y_pos + 16;
		
		if( cpu_usage >= 8)
		{
			if( cpu_usage >= 99)
				pxy[1] = y_pos + 4;
			else if( cpu_usage >= 91)
				pxy[1] = y_pos + 5;	
			else if( cpu_usage >= 83)
				pxy[1] = y_pos + 6;	
			else if( cpu_usage >= 75)
				pxy[1] = y_pos + 7;					
			else if( cpu_usage >= 66)
				pxy[1] = y_pos + 8;			
			else if( cpu_usage >= 58)
				pxy[1] = y_pos + 9;			
			else if( cpu_usage >= 50)
				pxy[1] = y_pos + 10;
			else if( cpu_usage >= 41)
				pxy[1] = y_pos + 11;			
			else if( cpu_usage >= 33)
				pxy[1] = y_pos + 12;
			else if( cpu_usage >= 25)
				pxy[1] = y_pos + 13;
			else if( cpu_usage >= 16)
				pxy[1] = y_pos + 14;				
			else 
				pxy[1] = y_pos + 15;

			vsf_color( win->graf->handle, ( int16)cpu_bar_color);
			v_bar( win->graf->handle, pxy);			
		}

		/* the STRAM usage */	
		pxy[0] = st_x + geek_area_x;
		pxy[1] = y_pos + 3;
		pxy[2] = pxy[0] + bar_width - 1;
		pxy[3] = pxy[1] + 14;

		vsf_color( win->graf->handle, BLACK);
		v_bar( win->graf->handle, pxy);	
				
		
		pxy[0] = pxy[0] + 1;
		pxy[2] = pxy[2] - 1;
		pxy[3] = y_pos + 16;
		
		if( stram_percent >= 8)
		{
			if( stram_percent >= 99)
				pxy[1] = y_pos + 4;
			else if( stram_percent >= 91)
				pxy[1] = y_pos + 5;	
			else if( stram_percent >= 83)
				pxy[1] = y_pos + 6;	
			else if( stram_percent >= 75)
				pxy[1] = y_pos + 7;					
			else if( stram_percent >= 66)
				pxy[1] = y_pos + 8;			
			else if( stram_percent >= 58)
				pxy[1] = y_pos + 9;			
			else if( stram_percent >= 50)
				pxy[1] = y_pos + 10;
			else if( stram_percent >= 41)
				pxy[1] = y_pos + 11;			
			else if( stram_percent >= 33)
				pxy[1] = y_pos + 12;
			else if( stram_percent >= 25)
				pxy[1] = y_pos + 13;
			else if( stram_percent >= 16)
				pxy[1] = y_pos + 14;				
			else 
				pxy[1] = y_pos + 15;

			vsf_color( win->graf->handle, ( int16)st_bar_color);
			v_bar( win->graf->handle, pxy);			
		}		

		/* the TTRAM usage */	
		
		if( total_ttram != 0)		
		{
			pxy[0] = tt_x + geek_area_x;
			pxy[1] = y_pos + 3;
			pxy[2] = pxy[0] + bar_width - 1;
			pxy[3] = pxy[1] + 14;

			vsf_color( win->graf->handle, BLACK);
			v_bar( win->graf->handle, pxy);	
				
		
			pxy[0] = pxy[0] + 1;
			pxy[2] = pxy[2] - 1;
			pxy[3] = y_pos + 16;
		
			if( ttram_percent >= 8)
			{
				if( ttram_percent >= 99)
					pxy[1] = y_pos + 4;
				else if( ttram_percent >= 91)
					pxy[1] = y_pos + 5;	
				else if( ttram_percent >= 83)
					pxy[1] = y_pos + 6;	
				else if( ttram_percent >= 75)	
					pxy[1] = y_pos + 7;					
				else if( ttram_percent >= 66)
					pxy[1] = y_pos + 8;			
				else if( ttram_percent >= 58)
					pxy[1] = y_pos + 9;			
				else if( ttram_percent >= 50)
					pxy[1] = y_pos + 10;
				else if( ttram_percent >= 41)
					pxy[1] = y_pos + 11;			
				else if( ttram_percent >= 33)
					pxy[1] = y_pos + 12;
				else if( ttram_percent >= 25)
					pxy[1] = y_pos + 13;
				else if( ttram_percent >= 16)
					pxy[1] = y_pos + 14;				
				else 
					pxy[1] = y_pos + 15;
	
				vsf_color( win->graf->handle, ( int16)tt_bar_color);
				v_bar( win->graf->handle, pxy);			
			}
		}						
	}

	
	/* draw the clock in the "geek" area */
	if( show_clock == TRUE)
	{
		draw_text( win->graf->handle, clock_x + geek_area_x, y_text_pos, (int16)geek_area_text_color, xbios_time);
	}
}


void set_component_position( void)
{
	int16 text_width, x = x_space + 2;

	if( show_system_info)
	{
		cpu_x = x;
		x += ( bar_width + 2);

		if( total_ttram != 0)	
		{
			st_x = x;
			x += ( bar_width + 2);
		
			tt_x = x;
			x += ( bar_width + x_space);
		}
		else	
		{
			st_x = x;
			x += ( bar_width + x_space);
		
			tt_x = x;
		}		
	}

	if( show_clock == TRUE)
	{
		clock_x = x;

		if( clock_us == FALSE)
			text_width = get_text_width( "00:00:00");
		else
			text_width = get_text_width( "00:00:00 PM");

		x += ( text_width + x_space);
	}

	geek_area_width = x + 1;
	y_text_pos  = app.y + app.h - 14;
}


void main_win( void)
{
	chrono_value = clock();
	
	rsrc_gaddr( 0, ICONS, &icons);
	popup.win = NULL;
	
	app_bar = WindCreate( 0, app.x, app.y, app.w, app.h);

	EvntAttach( app_bar, WM_REDRAW,	win_redraw_event);
	EvntAttach( app_bar, WM_XBUTTON, win_mouse_event);

	set_component_position();
	app_data_search();

	w_pos = geek_area_width + ( app_nbr * app_width) + 24;

	while( w_pos > app.w)
		w_pos -= app_width;

	x_pos = app.x + ( app.w - w_pos); 
	w_pos -= 1;
	y_pos = app.y + app.h - 21;

	
	WindCalc( WC_BORDER, app_bar, x_pos, y_pos, w_pos, 22, &x_pos, &y_pos, &w_pos, &h_pos);
	
	WindSet( app_bar, WF_BEVENT, BEVENT_WORK, 0, 0, 0);
	WindOpen( app_bar, x_pos, y_pos, w_pos, h_pos);
	EvntAttach( app_bar, WM_XTIMER, timer_function);
	WindGet( app_bar, WF_WORKXYWH, &x_pos, &y_pos, &w_pos, &h_pos);
	
	h_pos = 21;
}

