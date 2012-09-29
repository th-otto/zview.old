#include "general.h"
#include "app.h"
#include "string.h"
#include "win.h"
#include "taskman.h"
#include <signal.h>

popup_data popup;
extern WINDOW *app_bar;


void CDECL popup_delete( WINDOW *win, int16 buff[8])
{	
	WindDelete( win);
	popup.win = NULL;
	
	if( menu_enabled == TRUE)
	{
		menu_enabled = FALSE;
		draw_page( app_bar, root->x_pos - 24, root->y_pos, 24, 23);
	}
}


void process_popup_item( WINDOW *win, int16 buff[8])
{	
	if( popup.selected < 0)
		return;

	switch( popup.function[popup.selected])
	{ 
		case F_SHUTDOWN:
			shutdown_dialog();
			break;	

		case F_TASKMANAGER:
			taskman_dialog();			
			break;			
			
		case F_QUIT_ZBAR:
			quit_dialog();
			break;						

		case F_KILL:
			Pkill( popup.entry->pid, SIGKILL);
			mt_evnt_timer( 100L, app.aes_global); 	
			break;
			
		case F_PREFERENCES:
			pref_dialog();
			break;
			
		case F_QUIT:
			ApplWrite( popup.entry->id, AP_TERM, 0, 0, 0, 0, 0);
			mt_evnt_timer( 100L, app.aes_global); 	
			break;		
									
		default:
			break;		
	}

	popup_delete( win, buff);		
}


int16 popup_item_under_mouse( void)
{
	int16 i, ok, result = -1;

	for( i = 0; i < popup.item_nbr; i++)
	{
		ok = IS_IN( evnt.mx, evnt.my, popup.item_pos[i].x1, popup.item_pos[i].y1, popup.item_pos[i].x2, popup.item_pos[i].y2);

		if( ok)
		{
			if( popup.selectable[i] == TRUE)
				result = i;

			break;
		}
	}

	return result;
}



static void CDECL popup_redraw( WINDOW *win, int16 buff[8])
{	
	int16 pxy[6], i, y, title_color, title_shadow;

	pxy[0] = popup.x_pos;
	pxy[1] = popup.y_pos;
	pxy[2] = pxy[0] + popup.w_pos - 1;
	pxy[3] = pxy[1] + popup.h_pos - 1;

	vsf_color( win->graf->handle, button_off_background);
	v_bar( win->graf->handle, pxy);

	pxy[5] = pxy[1];
	pxy[1] = pxy[3];
	pxy[4] = pxy[2];

	vsl_color( win->graf->handle, button_off_dark_color);
	v_pline( win->graf->handle, 3, pxy);	

	pxy[4] = pxy[2];
	pxy[2] = pxy[0];	
	pxy[3] = popup.y_pos;	
	pxy[5] = pxy[3];

	vsl_color( win->graf->handle, button_off_light_color);
	v_pline( win->graf->handle, 3, pxy);	

	pxy[0] = popup.x_pos + 1;
	pxy[1] = popup.y_pos + 1;
	pxy[2] = pxy[0] + popup.w_pos - 3;

	for( i = 0, y = popup.y_pos + 7; i < popup.item_nbr; i++, y += 18, pxy[1] += 18)
	{	
		popup.item_pos[i].x1 = pxy[0];
		popup.item_pos[i].x2 = popup.w_pos;
		popup.item_pos[i].y1 = pxy[1];

		if( popup.item_name[i][0] == '\0')
		{
			int16 xy[4] = { pxy[0] + 1, pxy[1] + 5, pxy[2] - 1, pxy[1] + 5};

			v_pline( win->graf->handle, 2, xy);		

			xy[1]--;
			xy[3]--;

			vsl_color( win->graf->handle, button_off_dark_color);
			v_pline( win->graf->handle, 2, xy);	

			y -= 9; 
			pxy[1] -= 9;
			popup.item_pos[i].y2 = 9;

			continue;
		}

		popup.item_pos[i].y2 = 18;

		if( i == popup.selected)
		{
			pxy[3] = pxy[1] + 17;
			vsf_color( win->graf->handle, button_on_background);
			v_bar( win->graf->handle, pxy);			
			title_color = button_on_text_color;
			title_shadow = button_on_text_shadow_color;			
		}
		else
		{
			title_color = button_off_text_color;
			title_shadow = button_off_text_shadow_color;				
		}			

		if( popup.icon[i] > -1) 
		{
			icons[popup.icon[i]].ob_x = popup.x_pos + x_space;
			icons[popup.icon[i]].ob_y = pxy[1] + 2;
		
			mt_objc_draw( icons, popup.icon[i], 1, win->graf->clip.g_x, win->graf->clip.g_y, win->graf->clip.g_w, win->graf->clip.g_h, app.aes_global);

			draw_text( win->graf->handle, icons[popup.icon[i]].ob_x + 17 + x_space, y + 1, title_shadow, popup.item_name[i]);
			draw_text( win->graf->handle, icons[popup.icon[i]].ob_x + 16 + x_space, y, title_color, popup.item_name[i]);
		}
		else
		{
			draw_text( win->graf->handle, popup.x_pos + x_space + 1, y + 1, title_shadow, popup.item_name[i]);				
			draw_text( win->graf->handle, popup.x_pos + x_space, y, title_color, popup.item_name[i]);	
		}	
	}
}


void open_popup( app_data *entry)
{
	int16 i, lenght, zbarx, zbary, dum;

	WindGet( app_bar, WF_CURRXYWH, &zbarx, &zbary, &dum, &dum); 
	
	if( entry)
		popup.x_pos = entry->x_pos - 1;	
	else
		popup.x_pos = zbarx;			
		
	popup.h_pos = 5;
	popup.selected = -1;
	popup.old_selected = -1;
	
	for( popup.w_pos = 0, i = 0; i < popup.item_nbr; i++)
	{	
		if( popup.item_name[i][0] == '\0')
		{
			popup.h_pos += 9;
			continue;
		}
	
		lenght = get_text_width( popup.item_name[i]);	

		if( popup.icon[i] > -1)
			lenght += 16 + x_space;

		if( lenght > popup.w_pos)
			popup.w_pos = lenght;
		
		popup.h_pos += 18;		
	}
	
	popup.w_pos += ( x_space * 3);
	popup.w_pos = MAX( popup.w_pos, app_width + 1);
	
	while( popup.x_pos + popup.w_pos > app.x + app.w)
		popup.x_pos--;

	popup.entry = entry;				
	popup.win = WindCreate( 0, app.x, app.y, app.w, app.h);

	EvntAttach( popup.win, WM_DESTROY, popup_delete);
	EvntAttach( popup.win, WM_REDRAW,  popup_redraw);
	EvntAttach( popup.win, WM_XBUTTON, process_popup_item);	
	
	popup.y_pos = zbary - popup.h_pos + 1;

	WindOpen( popup.win, popup.x_pos, popup.y_pos, popup.w_pos, popup.h_pos);
//	WindSet( popup.win, WF_BEVENT, BEVENT_MODAL, 0, 0, 0);
	WindGet( popup.win, WF_WORKXYWH, &popup.x_pos, &popup.y_pos, &popup.w_pos, &popup.h_pos);	
}


