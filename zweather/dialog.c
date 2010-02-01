#include "general.h"
#include "icon.h"
#include "parsers.h"

extern struct xml_weather *zweatherdata;
extern int8	wind_direction[8];
extern void init_stik (void);

WINDOW *windialog = NULL;
int16 	hcell = 0;
int8	icon_temp[8] = "";

/*==================================================================================*
 * void WindIconify:																*
 *		Function to handle iconfied windows drawing... If the icon doesn't exist,	*
 *		It is created.																*
 *----------------------------------------------------------------------------------*
 * input:																			*
 *		win -> the window to handle.												*
 *----------------------------------------------------------------------------------*
 * returns: 																		*
 *      --																			*
 *==================================================================================*/

static void WindIconify( WINDOW *win)
{	
	int16 	posx, posy, x, y, w, h, xy[8];
	MFDB 	screen = {0};
	MFDB   *iconified_icon = &main_icon->image[0];

	WindGet( win, WF_WORKXYWH, &x, &y, &w, &h);

	xy[0] = x;
	xy[1] = y;
	xy[2] = xy[0] + w - 1;
	xy[3] = xy[1] + h - 1;

	vsf_color( win->graf.handle, LWHITE);
	v_bar( win->graf.handle, xy);

	posx    = MAX( x + (( w - iconified_icon->fd_w) >> 1), x);
	posy 	= MAX( y + (( h - iconified_icon->fd_h) >> 1), y);

	if (posx == x)
		xy[2] = w - 1;
	else
		xy[2] = iconified_icon->fd_w - 1;

	if (posy == y)
		xy[3] = h - 1;
	else
		xy[3] = iconified_icon->fd_h - 1;

	xy[0] = 0;
	xy[1] = 0;
	xy[4] = posx;
	xy[5] = posy;
	xy[6] = posx + xy[2];
	xy[7] = posy + xy[3];

	/* draw the image */
	if ( iconified_icon->fd_nplanes == 1)
	{
		int16	color[2] = { BLACK, WHITE};

		vrt_cpyfm( win->graf.handle, MD_REPLACE, xy, iconified_icon, &screen, color);
	}
	else
		vro_cpyfm( win->graf.handle, S_ONLY, xy, iconified_icon, &screen);

	v_ftext( win->graf.handle, x + w - 2, y + h - hcell, icon_temp);
}

/*
void send_iconify( void)
{
	WindSet( windialog, WF_ICONIFY, 0, 1, 0, 1);
	ObjcChange( OC_FORM, windialog, WEATHER_ICONIFY, NORMAL, 0);
}
*/


static void sting_layer( WINDOW *win, int16 obj_index) 
{
	use_sting = ( use_sting == TRUE ? FALSE : TRUE);

	/* a wait loop while the mouse button is pressed */		
	while(( evnt.mbut == 1) || ( evnt.mbut == 2))
		graf_mkstate( &evnt.mx, &evnt.my, &evnt.mbut, &evnt.mkstate); 
}


void force_update( void)
{
	OBJECT	*weather_form = get_tree( WEATHER);
	int 	time_minute;

	graf_mouse( BUSYBEE, NULL);

    if( EvntFind( NULL, WM_XTIMER))
		EvntDelete( NULL, WM_XTIMER);

	if( zweatherdata)
		xml_weather_free( zweatherdata);

	if( use_sting)
		init_stik();

	strcpy( location_code, ObjcString( weather_form, WEATHER_CODE, NULL));

	zweatherdata = update_weatherdata( location_code);

	time_minute = atoi( ( const char*)ObjcString( weather_form, WEATHER_UPDATE_TIME, NULL));

	update_time = ( clock_t)( ( int32)time_minute * 12000L);

	ObjcChange( OC_FORM, windialog, WEATHER_UPDATE, NORMAL, 0);

	snd_rdw( windialog);

	EvntAttach( NULL, WM_XTIMER, timer_function);

	graf_mouse( ARROW, NULL);
}


void weather_dialog( void)
{
	OBJECT	*weather_form = get_tree( WEATHER);
	int 	frms[] = { WEATHER_PANEL1, WEATHER_PANEL2, 	WEATHER_PANEL3};
	int 	buts[] = { WEATHER_MAIN,   WEATHER_FORECAST,   WEATHER_PREF};
	int		time_minute = ( int)( update_time / 12000L);
	int16	dum;

	strcpy( ObjcString( weather_form, WEATHER_CODE, NULL), location_code);
	sprintf( ObjcString( weather_form, WEATHER_UPDATE_TIME, NULL), "%d", time_minute);

	if( use_sting == 1)
 		ObjcChange( OC_OBJC, weather_form, WEATHER_STING, SELECTED, 0);
	else
		ObjcChange( OC_OBJC, weather_form, WEATHER_STING, NORMAL, 0);

	windialog = FormCreate( weather_form, NAME|MOVER|SMALLER, NULL, " zWeather ", NULL, TRUE, FALSE);

	RsrcUserDraw( OC_FORM, windialog, WEATHER_ICON,  draw_weather_icon, NULL);
	RsrcUserDraw( OC_FORM, windialog, WEATHER_ICON1, draw_forecast1_icon, NULL);
	RsrcUserDraw( OC_FORM, windialog, WEATHER_ICON2, draw_forecast2_icon, NULL);
	RsrcUserDraw( OC_FORM, windialog, WEATHER_ICON3, draw_forecast3_icon, NULL);
	RsrcUserDraw( OC_FORM, windialog, WEATHER_ICON4, draw_forecast4_icon, NULL);
	RsrcUserDraw( OC_FORM, windialog, WEATHER_ICON5, draw_forecast5_icon, NULL);
	RsrcUserDraw( OC_FORM, windialog, WEATHER_ICON6, draw_forecast6_icon, NULL);
	RsrcUserDraw( OC_FORM, windialog, WEATHER_WICON, draw_boussole_icon, NULL);

	FormThumb( windialog, frms, buts, 3);

	if( main_icon == NULL)
		main_icon = icons[25];

	if( forecast_icon[0] == NULL)
		forecast_icon[0] = icons[25];

	if( forecast_icon[1] == NULL)
		forecast_icon[1] = icons[25];

	if( forecast_icon[2] == NULL)
		forecast_icon[2] = icons[25];

	if( forecast_icon[3] == NULL)
		forecast_icon[3] = icons[25];

	if( forecast_icon[4] == NULL)
		forecast_icon[4] = icons[25];

	if( forecast_icon[5] == NULL)
		forecast_icon[5] = icons[25];

	ObjcAttach( OC_FORM, windialog, WEATHER_METRIC, 	BIND_VAR,	&unit);
	ObjcAttach( OC_FORM, windialog, WEATHER_IMPERIAL,	BIND_VAR,	&unit);
	ObjcAttach( OC_FORM, windialog, WEATHER_UPDATE,		BIND_FUNC,	force_update);
	ObjcAttach( OC_FORM, windialog, WEATHER_STING, 		BIND_FUNC,	sting_layer);
	ObjcAttach( OC_FORM, windialog, WEATHER_QUIT, 		BIND_FUNC,	applexit);

	vswr_mode( windialog->graf.handle, MD_TRANS);
	vst_point( windialog->graf.handle, 8, &dum, &dum, &dum, &hcell);
	vst_alignment( windialog->graf.handle, 2, 5, &dum, &dum);
	vst_color( windialog->graf.handle, LBLUE);
	vsl_color( windialog->graf.handle, RED);
	hcell++;

	WindSetStr( windialog, WF_ICONDRAW, WindIconify);

//	EvntAttach( wininfo, WM_CLOSED, close_modal);
}

