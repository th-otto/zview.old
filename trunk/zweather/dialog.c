#include "general.h"
#include "icon.h"
#include "parsers.h"

extern struct xml_weather *zweatherdata;
extern int8	wind_direction[8];
extern void init_stik (void);

WINDOW *windialog = NULL;
int8	icon_temp[8] = "";

static void force_update( WINDOW *win, int obj, int mode, void *data)
{
	OBJECT	*weather_form = get_tree( WEATHER);
	int 	time_minute;

	graf_mouse( BUSYBEE, NULL);

    if( EvntFind( NULL, WM_XTIMER))
		EvntDelete( NULL, WM_XTIMER);

	if( zweatherdata)
		xml_weather_free( zweatherdata);

	strcpy( location_code, ObjcString( weather_form, WEATHER_CODE, NULL));

	zweatherdata = update_weatherdata( location_code);

	time_minute = atoi( ( const char*)ObjcString( weather_form, WEATHER_UPDATE_TIME, NULL));

	update_time = ( clock_t)( ( int32)time_minute * 12000L);

	ObjcChange( OC_FORM, windialog, WEATHER_UPDATE, NORMAL, 0);

	snd_rdw( windialog);

	EvntAttach( NULL, WM_XTIMER, timer_function);

	graf_mouse( ARROW, NULL);
}

static void weather_dialog_quit( WINDOW *win, int obj, int mode, void *data)
{
	ObjcChange( OC_FORM, win, obj, NORMAL, TRUE);
	ApplWrite( _AESapid, AP_TERM, 0, 0, 0, 0, 0);
}

static void CDECL continent_popup( WINDOW *win, int obj, int mode, void *data)
{
	OBJECT *pop;
	int16 x, y;
	static int last_choice = -1;
	int choice;

	rsrc_gaddr( 0, CONTINENT, &pop);
	objc_offset( FORM( win), obj, &x, &y);

	choice = MenuPopUp ( pop, x, y, -1, -1, last_choice, P_WNDW + P_CHCK);
	last_choice = choice;
	ObjcChange( OC_FORM, win, obj, NORMAL, TRUE);
}

void weather_dialog( void)
{
	OBJECT	*weather_form = get_tree( WEATHER);
	int 	frms[] = { WEATHER_PANEL1, WEATHER_PANEL2, WEATHER_PANEL3, WEATHER_PANEL4};
	int 	buts[] = { WEATHER_MAIN, WEATHER_FORECAST, WEATHER_MAP, WEATHER_PREF};
	int		time_minute = ( int)( update_time / 12000L);

	strcpy( ObjcString( weather_form, WEATHER_CONTINENT, NULL), "--");
	strcpy( ObjcString( weather_form, WEATHER_STATE, NULL), "--");
	strcpy( ObjcString( weather_form, WEATHER_CODE, NULL), location_code);
	sprintf( ObjcString( weather_form, WEATHER_UPDATE_TIME, NULL), "%d", time_minute);

	windialog = FormCreate( weather_form, NAME|MOVER, NULL, " zWeather ", NULL, TRUE, FALSE);

//	RsrcUserDraw( OC_FORM, windialog, WEATHER_ROOT,  draw_root_dial, NULL);
	RsrcUserDraw( OC_FORM, windialog, WEATHER_ICON,  draw_weather_icon, NULL);
	RsrcUserDraw( OC_FORM, windialog, WEATHER_ICON1, draw_forecast1_icon, NULL);
	RsrcUserDraw( OC_FORM, windialog, WEATHER_ICON2, draw_forecast2_icon, NULL);
	RsrcUserDraw( OC_FORM, windialog, WEATHER_ICON3, draw_forecast3_icon, NULL);
	RsrcUserDraw( OC_FORM, windialog, WEATHER_ICON4, draw_forecast4_icon, NULL);
	RsrcUserDraw( OC_FORM, windialog, WEATHER_ICON5, draw_forecast5_icon, NULL);
	RsrcUserDraw( OC_FORM, windialog, WEATHER_ICON6, draw_forecast6_icon, NULL);
	RsrcUserDraw( OC_FORM, windialog, WEATHER_WICON, draw_boussole_icon, NULL);

	FormThumb( windialog, frms, buts, 4);

	if( main_icon == NULL)
		main_icon = &icons[25];

	if( forecast_icon[0] == NULL)
		forecast_icon[0] = &icons[25];

	if( forecast_icon[1] == NULL)
		forecast_icon[1] = &icons[25];

	if( forecast_icon[2] == NULL)
		forecast_icon[2] = &icons[25];

	if( forecast_icon[3] == NULL)
		forecast_icon[3] = &icons[25];

	if( forecast_icon[4] == NULL)
		forecast_icon[4] = &icons[25];

	ObjcAttachVar( OC_FORM, windialog, WEATHER_METRIC, &unit, WEATHER_METRIC);
	ObjcAttachVar( OC_FORM, windialog, WEATHER_IMPERIAL, &unit, WEATHER_IMPERIAL);
	ObjcAttachFormFunc( windialog, WEATHER_UPDATE, force_update, NULL);
	ObjcAttachFormFunc( windialog, WEATHER_QUIT, weather_dialog_quit, NULL);
	ObjcAttachFormFunc( windialog, WEATHER_CONTINENT, continent_popup, NULL);

	vswr_mode( windialog->graf->handle, MD_TRANS);
	vsl_color( windialog->graf->handle, RED);
}
