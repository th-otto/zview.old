#include "general.h"

static int16 xw, yw, ww, hw, color_by_line;
static int16 selected_color;
static int16 object_parent;

extern WINDOW *pref_dialog_win;
extern int tmp_button_off_background, tmp_button_off_light_color, tmp_button_off_dark_color, 
tmp_button_off_text_color, tmp_button_off_text_shadow_color, tmp_button_on_background,
tmp_button_on_light_color, tmp_button_on_dark_color, tmp_button_on_text_color,
tmp_button_on_text_shadow_color, tmp_geek_area_color, tmp_geek_area_dark_line, 
tmp_geek_area_light_line, tmp_app_width, tmp_cpu_bar_color, 
tmp_tt_bar_color, tmp_st_bar_color, tmp_geek_area_text_color;

static void CDECL timer_function( WINDOW *win, int16 buff[8])
{
	if( !IS_IN( evnt.mx, evnt.my, xw, yw, ww, hw))
		ApplWrite( _AESapid, WM_DESTROY, win->handle, 0, 0, 0, 0);
}	
	
	
static void CDECL win_mouse_event( WINDOW *win, int16 buff[8])
{
	int16 x, y, pxy[2];
	int color = 0;
	
	for( y = 0, pxy[1] = yw + 1; y < color_by_line; y++, pxy[1] += 11)
	{
		for( x = 0, pxy[0] = xw + 1; x < color_by_line; x++, pxy[0] += 11, color++)
		{
			if( IS_IN( evnt.mx, evnt.my, pxy[0], pxy[1], 9, 9))
			{
				switch( object_parent)
				{
					case PREFS_ON_BACK:
						tmp_button_on_background = color;
						break;

					case PREFS_ON_LINE1:
						tmp_button_on_light_color = color;
						break;
						
					case PREFS_ON_LINE2:
						tmp_button_on_dark_color = color;
						break;	
											
					case PREFS_ON_TEXT1:
						tmp_button_on_text_color = color;
						break;

					case PREFS_ON_TEXT2:
						tmp_button_on_text_shadow_color = color;
						break;

					case PREFS_OFF_BACK:
						tmp_button_off_background = color;
						break;

					case PREFS_OFF_LINE1:
						tmp_button_off_light_color = color;
						break;
						
					case PREFS_OFF_LINE2:
						tmp_button_off_dark_color = color;
						break;
						
					case PREFS_OFF_TEXT1:
						tmp_button_off_text_color = color;
						break;
						
					case PREFS_OFF_TEXT2:
						tmp_button_off_text_shadow_color = color;
						break;	
						
					case PREFS_GEEK_BACK:
						tmp_geek_area_color = color;
						break;
						
					case PREFS_GEEK_LINE1:
						tmp_geek_area_light_line = color;
						break;
						
					case PREFS_GEEK_LINE2:
						tmp_geek_area_dark_line = color;
						break;

					case PREFS_GEEK_RAM1:
						tmp_st_bar_color = color;
						break;
					
					case PREFS_GEEK_RAM2:
						tmp_tt_bar_color = color;
						break;	
						
					case PREFS_GEEK_CPU:
						tmp_cpu_bar_color = color;
						break;
						
					case PREFS_GEEK_TEXT:
						tmp_geek_area_text_color = color;
						break;
				}
				
				break;
			}
		}
	}
	
	ApplWrite( _AESapid, WM_DESTROY, win->handle, 0, 0, 0, 0);
	ObjcDraw( OC_FORM, pref_dialog_win, PREFS_PREVIEW, 1);
}


static void CDECL win_redraw_event( WINDOW *win, int16 buff[8])
{
	int16 x, y, pxy[4], xy[10], color = 0;

	pxy[0] = xw;
	pxy[1] = yw;
	pxy[2] = pxy[0] + ww - 1;
	pxy[3] = pxy[1] + hw - 1;

	vsf_color( win->graf->handle, WHITE);
	v_bar( win->graf->handle, pxy);	
	
	vsl_color( win->graf->handle, BLACK);
		
	for( y = 0, pxy[1] = yw + 1; y < color_by_line; y++, pxy[1] += 11)
	{
		for( x = 0, pxy[0] = xw + 1; x < color_by_line; x++, pxy[0] += 11, color++)
		{
			pxy[2] = pxy[0] + 9;
			pxy[3] = pxy[1] + 9;			

			vsf_color( win->graf->handle, color);
			v_bar( win->graf->handle, pxy);			

			xy[0] = pxy[0];
			xy[1] = pxy[1];
			xy[2] = xy[0];
			xy[3] = xy[1] + 9;
			xy[4] = xy[0] + 9;
			xy[5] = xy[3];
			xy[6] = xy[4];
			xy[7] = xy[1];
			xy[8] = xy[0];
			xy[9] = xy[1];

			v_pline( win->graf->handle, 5, xy);		
			
			if( color == selected_color)
			{
				xy[0] = pxy[0] - 1;
				xy[1] = pxy[1] - 1;
				xy[2] = xy[0];
				xy[3] = xy[1] + 11;
				xy[4] = xy[0] + 11;
				xy[5] = xy[3];
				xy[6] = xy[4];
				xy[7] = xy[1];
				xy[8] = xy[0];
				xy[9] = xy[1];

				v_pline( win->graf->handle, 5, xy);	
			}
		}
	}
}


void vdi_color_selector( int16 x, int16 y, int16 color_selected, int16 parent_object)
{	
	WINDOW *win = WindCreate( 0, app.x, app.y, app.w, app.h);

	EvntAttach( win, WM_REDRAW,	win_redraw_event);
	EvntAttach( win, WM_XBUTTON, win_mouse_event);

	color_by_line  = (( app.nplanes < 8) ? 4 : 16);
	selected_color = color_selected;
	object_parent  = parent_object;
		
	if( app.nplanes == 4)
		WindCalc( WC_BORDER, win, x, y, 45, 45, &xw, &yw, &ww, &hw);
	else
		WindCalc( WC_BORDER, win, x, y, 177, 177, &xw, &yw, &ww, &hw);
			
	WindSet( win, WF_BEVENT, BEVENT_MODAL, 0, 0, 0);
	EvntAttach( win, WM_XTIMER, timer_function);
	WindOpen( win, xw, yw, ww, hw);
	WindGet( win, WF_WORKXYWH, &xw, &yw, &ww, &hw);
}

