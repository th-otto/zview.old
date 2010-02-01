#include "general.h"
#include "win.h"
#include "string.h"

int tmp_button_off_background, tmp_button_off_light_color, tmp_button_off_dark_color, 
tmp_button_off_text_color, tmp_button_off_text_shadow_color, tmp_button_on_background,
tmp_button_on_light_color, tmp_button_on_dark_color, tmp_button_on_text_color,
tmp_button_on_text_shadow_color, tmp_geek_area_color, tmp_geek_area_dark_line, 
tmp_geek_area_light_line, tmp_app_width, tmp_cpu_bar_color, 
tmp_tt_bar_color, tmp_st_bar_color, tmp_show_clock, tmp_clock_us, 
tmp_show_system_info, tmp_show_acc;

int *tmp;
static int16 xy[10], tmp_cpu_x, tmp_tt_x, tmp_st_x, tmp_clock_x, tmp_geek_area_width, tmp_y_text_pos,
tmp_x_pos, tmp_y_pos, tmp_w_pos, tmp_geek_area_x, text_width, x;
static OBJECT *dial = NULL;


static void CDECL pref_dialog_cancel_event( WINDOW *win, int obj, int mode, void *data)
{
	ObjcChange( OC_FORM, win, obj, NORMAL, TRUE);
}

static void CDECL pref_dialog_ok_event( WINDOW *win, int obj, int mode, void *data)
{
	ObjcChange( OC_FORM, win, obj, NORMAL, TRUE);	
	ApplWrite( _AESapid, WM_DESTROY, win->handle, 0, 0, 0, 0);
}


static void CDECL draw_preview( WINDOW *win, PARMBLK *pblk, void *data) 
{
	xy[0] = pblk->pb_x;
	xy[1] = pblk->pb_y;
	xy[2] = pblk->pb_x + pblk->pb_w - 1;
	xy[3] = pblk->pb_y + pblk->pb_h - 1;

	vsf_color( win->graf->handle, LCYAN);
	v_bar( win->graf->handle, xy);

	xy[0] = pblk->pb_x;
	xy[1] = pblk->pb_y;
	xy[2] = pblk->pb_x;
	xy[3] = pblk->pb_y + pblk->pb_h - 1;
	xy[4] = pblk->pb_x + pblk->pb_w - 1;
	xy[5] = xy[3];
	xy[6] = xy[4];
	xy[7] = pblk->pb_y;
	xy[8] = pblk->pb_x;
	xy[9] = pblk->pb_y;

	vsl_color( win->graf->handle, BLACK);
	v_pline( win->graf->handle, 5, xy);

	x = x_space + 2;
	
	if( tmp_show_system_info)
	{
		tmp_cpu_x = x;
		x += ( bar_width + 2);

		tmp_st_x = x;
		x += ( bar_width + 2);
		
		tmp_tt_x = x;
		x += ( bar_width + x_space);
	}

	if( tmp_show_clock == TRUE)
	{
		tmp_clock_x = x;

		if( tmp_clock_us == FALSE)
			text_width = get_text_width( "22:37:05");
		else
			text_width = get_text_width( "10:37:05 PM");

		x += ( text_width + x_space);
	}

	tmp_geek_area_width = x + 1;

	tmp_y_text_pos  = pblk->pb_y + pblk->pb_h - 15;

	tmp_w_pos = tmp_geek_area_width + ( 2 * tmp_app_width) + 23;
	tmp_x_pos = pblk->pb_x + ( pblk->pb_w - tmp_w_pos) - 1; 
	tmp_y_pos = pblk->pb_y + pblk->pb_h - 22;		
	
		
	tmp_geek_area_x = tmp_x_pos + tmp_w_pos - tmp_geek_area_width;
	
	/* we draw the menu area */	
	xy[0] = tmp_x_pos;
	xy[1] = tmp_y_pos;
	xy[2] = xy[0] + 23;
	xy[3] = xy[1] + 20;
			
			
	vsf_color( win->graf->handle, ( int16)tmp_button_off_background);
	v_bar( win->graf->handle, xy);			

	xy[1] = xy[3];
	xy[2] = xy[0];
	xy[3] = tmp_y_pos;
	xy[4] = xy[0] + 22;
	xy[5] = tmp_y_pos;

	vsl_color( win->graf->handle, ( int16)tmp_button_off_light_color);
	v_pline( win->graf->handle, 3, xy);
			
	xy[0] = xy[4] + 1;
	xy[1] = tmp_y_pos;
	xy[2] = xy[0];
	xy[3] = xy[1] + 20;
	xy[4] = tmp_x_pos;
	xy[5] = xy[3];

	vsl_color( win->graf->handle, ( int16)tmp_button_off_dark_color);
	v_pline( win->graf->handle, 3, xy);

//	icons[ICONS_MENU].ob_x = xw + 4;
//	icons[ICONS_MENU].ob_y = y_pos + 2;

//	mt_objc_draw( icons, ICONS_MENU, 1, win->graf->clip.g_x, win->graf->clip.g_y, win->graf->clip.g_w, win->graf->clip.g_h, app.aes_global);

	tmp_x_pos += 24;

	xy[0] = tmp_x_pos;
	xy[1] = tmp_y_pos;
	xy[2] = xy[0] + tmp_app_width - 1;
	xy[3] = xy[1] + 20;			
			
	vsf_color( win->graf->handle, ( int16)tmp_button_on_background);
	v_bar( win->graf->handle, xy);			

	xy[1] = xy[3];
	xy[2] = xy[0];
	xy[3] = tmp_y_pos;
	xy[4] = xy[0] + tmp_app_width - 2;
	xy[5] = tmp_y_pos;

	vsl_color( win->graf->handle, ( int16)tmp_button_on_dark_color);
	v_pline( win->graf->handle, 3, xy);
			
	xy[0] = xy[4] + 1;
	xy[1] = tmp_y_pos;
	xy[2] = xy[0];
	xy[3] = xy[1] + 20;
	xy[4] = tmp_x_pos;
	xy[5] = xy[3];
				
	vsl_color( win->graf->handle, ( int16)tmp_button_on_light_color);
	v_pline( win->graf->handle, 3, xy);	

	draw_text( win->graf->handle, tmp_x_pos + 21, tmp_y_text_pos + 1, ( int16)tmp_button_on_text_shadow_color, "Dummy");
	draw_text( win->graf->handle, tmp_x_pos + 20, tmp_y_text_pos, ( int16)tmp_button_on_text_color, "Dummy");

	tmp_x_pos += tmp_app_width;

	xy[0] = tmp_x_pos;
	xy[1] = tmp_y_pos;
	xy[2] = xy[0] + tmp_app_width - 1;
	xy[3] = xy[1] + 20;			
			
	vsf_color( win->graf->handle, ( int16)tmp_button_off_background);
	v_bar( win->graf->handle, xy);			

	xy[1] = xy[3];
	xy[2] = xy[0];
	xy[3] = tmp_y_pos;
	xy[4] = xy[0] + tmp_app_width - 2;
	xy[5] = tmp_y_pos;

	vsl_color( win->graf->handle, ( int16)tmp_button_off_light_color);
	v_pline( win->graf->handle, 3, xy);
			
	xy[0] = xy[4] + 1;
	xy[1] = tmp_y_pos;
	xy[2] = xy[0];
	xy[3] = xy[1] + 20;
	xy[4] = tmp_x_pos;
	xy[5] = xy[3];
				
	vsl_color( win->graf->handle, ( int16)tmp_button_off_dark_color);
	v_pline( win->graf->handle, 3, xy);	

	draw_text( win->graf->handle, tmp_x_pos + 21, tmp_y_text_pos + 1, ( int16)tmp_button_off_text_shadow_color, "Dummy");
	draw_text( win->graf->handle, tmp_x_pos + 20, tmp_y_text_pos, ( int16)tmp_button_off_text_color, "Dummy");

	tmp_x_pos += tmp_app_width;
	
		
				
	/* draw the "geek" area */
	xy[0] = tmp_x_pos + 2;
	xy[1] = tmp_y_pos + 2;
	xy[2] = xy[0] + tmp_geek_area_width - 6;
	xy[3] = xy[1] + 16;

	vsf_color( win->graf->handle, ( int16)tmp_geek_area_color);
	v_bar( win->graf->handle, xy);

	xy[0] = tmp_x_pos;
	xy[1] = tmp_y_pos;
	xy[2] = xy[0];
	xy[3] = xy[1] + 20;	
	xy[4] = xy[2] + tmp_geek_area_width - 2;
	xy[5] = xy[3];
	xy[6] = xy[4];
	xy[7] = xy[1];	
	xy[8] = xy[0];
	xy[9] = xy[1];
				
	vsl_color( win->graf->handle, ( int16)tmp_button_off_background);
	v_pline( win->graf->handle, 5, xy);	

	xy[0]++;
	xy[1] = xy[3] - 1;
	xy[2] = xy[0];
	xy[3] = tmp_y_pos + 1;	
	xy[4]--;
	xy[5] = xy[3];
				
	vsl_color( win->graf->handle, ( int16)tmp_geek_area_dark_line);
	v_pline( win->graf->handle, 3, xy);		
	
	xy[2] = xy[4];
	xy[3] = xy[1];	
	xy[5] = tmp_y_pos + 2;
				
	vsl_color( win->graf->handle, ( int16)tmp_geek_area_light_line);
	v_pline( win->graf->handle, 3, xy);	
	
	
	/* draw the cpu % in the "geek" area */
	if( tmp_show_system_info)
	{
		/* the CPU usage */
		xy[0] = tmp_cpu_x + tmp_geek_area_x;
		xy[1] = tmp_y_pos + 3;
		xy[2] = xy[0] + bar_width - 1;
		xy[3] = xy[1] + 14;

		vsf_color( win->graf->handle, BLACK);
		v_bar( win->graf->handle, xy);
			
		xy[0] = xy[0] + 1;
		xy[2] = xy[2] - 1;
		xy[3] = tmp_y_pos + 16;
		xy[1] = tmp_y_pos + 9;	

		vsf_color( win->graf->handle, ( int16)tmp_cpu_bar_color);
		v_bar( win->graf->handle, xy);			

		/* the STRAM usage */	
		xy[0] = tmp_st_x + tmp_geek_area_x;
		xy[1] = tmp_y_pos + 3;
		xy[2] = xy[0] + bar_width - 1;
		xy[3] = xy[1] + 14;

		vsf_color( win->graf->handle, BLACK);
		v_bar( win->graf->handle, xy);	
				
		xy[0] = xy[0] + 1;
		xy[2] = xy[2] - 1;
		xy[3] = tmp_y_pos + 16;
		xy[1] = tmp_y_pos + 9;	
				
		vsf_color( win->graf->handle, ( int16)tmp_st_bar_color);
		v_bar( win->graf->handle, xy);			
	
		xy[0] = tmp_tt_x + tmp_geek_area_x;
		xy[1] = tmp_y_pos + 3;
		xy[2] = xy[0] + bar_width - 1;
		xy[3] = xy[1] + 14;

		vsf_color( win->graf->handle, BLACK);
		v_bar( win->graf->handle, xy);	
				
		
		xy[0] = xy[0] + 1;
		xy[2] = xy[2] - 1;
		xy[3] = tmp_y_pos + 16;
		xy[1] = tmp_y_pos + 9;			
	
		vsf_color( win->graf->handle, ( int16)tmp_tt_bar_color);
		v_bar( win->graf->handle, xy);			
	}

	if( tmp_show_clock == TRUE)
	{
		if( tmp_clock_us == FALSE)
			draw_text( win->graf->handle, tmp_clock_x + tmp_geek_area_x, tmp_y_text_pos, BLACK, "22:37:05");
		else
			draw_text( win->graf->handle, tmp_clock_x + tmp_geek_area_x, tmp_y_text_pos, BLACK, "10:37:05 PM");
	}	
}


static void CDECL draw_color_case( WINDOW *win, PARMBLK *pblk, void *data) 
{
	tmp = ( int*)data;
		
	xy[0] = pblk->pb_x;
	xy[1] = pblk->pb_y;
	xy[2] = xy[0] + 127;
	xy[3] = xy[1] + 127;

	vsf_color( win->graf->handle, ( int16)*tmp);
	v_bar( win->graf->handle, xy);

	xy[0] = pblk->pb_x;
	xy[1] = pblk->pb_y;
	xy[2] = pblk->pb_x;
	xy[3] = pblk->pb_y + pblk->pb_h - 1;
	xy[4] = pblk->pb_x + pblk->pb_w - 1;
	xy[5] = xy[3];
	xy[6] = xy[4];
	xy[7] = pblk->pb_y;
	xy[8] = pblk->pb_x;
	xy[9] = pblk->pb_y;

	vsl_color( win->graf->handle, BLACK);
	v_pline( win->graf->handle, 5, xy);
}


void pref_dialog( void)
{
	WINDOW 	*win;	
	int	frms[] = { PREFS_PANEL1, PREFS_PANEL2};
	int	buts[] = { PREFS_COLOR, PREFS_MISC};

	dial = get_tree( PREFS);	

	tmp_button_off_background			= button_off_background;
	tmp_button_off_light_color			= button_off_light_color;
	tmp_button_off_dark_color			= button_off_dark_color;
	tmp_button_off_text_color			= button_off_text_color;
	tmp_button_off_text_shadow_color	= button_off_text_shadow_color;
	tmp_button_on_background			= button_on_background;
	tmp_button_on_light_color			= button_on_light_color;
	tmp_button_on_dark_color 			= button_on_dark_color;
	tmp_button_on_text_color			= button_on_text_color;
	tmp_button_on_text_shadow_color		= button_on_text_shadow_color;
	tmp_geek_area_color 				= geek_area_color;
	tmp_geek_area_dark_line				= geek_area_dark_line;
	tmp_geek_area_light_line			= geek_area_light_line;
	tmp_app_width						= app_width;
	tmp_cpu_bar_color 					= cpu_bar_color;
	tmp_tt_bar_color 					= tt_bar_color;
	tmp_st_bar_color 					= st_bar_color;
	tmp_show_clock 						= show_clock;
	tmp_clock_us						= clock_us;
	tmp_show_system_info				= show_system_info;
	tmp_show_acc						= show_acc;	
	
				
	if( ( win = FormCreate( dial, NAME|MOVER, NULL, "Preferences", NULL, TRUE, FALSE)) == NULL)
		return;
		
	dial = FORM( win);	
	
	FormThumb( win, frms, buts, 2);	

	RsrcUserDraw( OC_FORM, win, PREFS_ON_BACK,  draw_color_case, &tmp_button_on_background);	
	RsrcUserDraw( OC_FORM, win, PREFS_ON_LINE1, draw_color_case, &tmp_button_on_light_color);		
	RsrcUserDraw( OC_FORM, win, PREFS_ON_LINE2, draw_color_case, &tmp_button_on_dark_color);	
	RsrcUserDraw( OC_FORM, win, PREFS_ON_TEXT1, draw_color_case, &tmp_button_on_text_color);	
	RsrcUserDraw( OC_FORM, win, PREFS_ON_TEXT2, draw_color_case, &tmp_button_on_text_shadow_color);	
	RsrcUserDraw( OC_FORM, win, PREFS_OFF_BACK, draw_color_case, &tmp_button_off_background);	
	RsrcUserDraw( OC_FORM, win, PREFS_OFF_LINE1, draw_color_case, &tmp_button_off_light_color);
	RsrcUserDraw( OC_FORM, win, PREFS_OFF_LINE2, draw_color_case, &tmp_button_off_dark_color);
	RsrcUserDraw( OC_FORM, win, PREFS_OFF_TEXT1, draw_color_case, &tmp_button_off_text_color);
	RsrcUserDraw( OC_FORM, win, PREFS_OFF_TEXT2, draw_color_case, &tmp_button_off_text_shadow_color);
	RsrcUserDraw( OC_FORM, win, PREFS_GEEK_BACK, draw_color_case, &tmp_geek_area_color);	
	RsrcUserDraw( OC_FORM, win, PREFS_GEEK_LINE1, draw_color_case, &tmp_geek_area_light_line);	
	RsrcUserDraw( OC_FORM, win, PREFS_GEEK_LINE2, draw_color_case, &tmp_geek_area_dark_line);
	RsrcUserDraw( OC_FORM, win, PREFS_GEEK_RAM1, draw_color_case, &tmp_st_bar_color);
	RsrcUserDraw( OC_FORM, win, PREFS_GEEK_RAM2, draw_color_case, &tmp_tt_bar_color);
	RsrcUserDraw( OC_FORM, win, PREFS_GEEK_CPU, draw_color_case, &tmp_cpu_bar_color);	
	RsrcUserDraw( OC_FORM, win, PREFS_PREVIEW, draw_preview, NULL);							
	
	WindSet( win, WF_BEVENT, BEVENT_MODAL, 0, 0, 0);
	ObjcAttachFormFunc( win, PREFS_OK, pref_dialog_ok_event, NULL);
	ObjcAttachFormFunc( win, PREFS_CANCEL, pref_dialog_cancel_event, NULL);
	ObjcAttachVar( OC_FORM, win, PREFS_SHOW_US_CLOCK, &tmp_clock_us, 1);	
	ObjcAttachVar( OC_FORM, win, PREFS_SHOW_ACC, &tmp_show_acc, 1);	
	ObjcAttachVar( OC_FORM, win, PREFS_SYSTEM_INFO, &tmp_show_system_info, 1);		
	ObjcAttachVar( OC_FORM, win, PREFS_SHOW_CLOCK, &tmp_show_clock, 1);			
}

