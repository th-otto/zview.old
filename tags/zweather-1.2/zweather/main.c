#include "general.h"
#include "http_client.h"
#include "parsers.h"
#include "icon.h"
#include "get_data.h"

static clock_t chrono_value;
struct xml_weather *zweatherdata = NULL;

extern void init_stik (void);
extern void weather_dialog( void);
extern int8	wind_direction[8];
extern int8	icon_temp[8];


struct xml_weather *update_weatherdata( int8 *location)
{
	OBJECT	*weather_form			= get_tree( WEATHER);
	xmlNode *cur_node 				= NULL;
	xmlDoc	*doc 					= NULL;
	struct	xml_weather *weather 	= NULL;
	struct	stat attrs;
	int		icon_to_draw;
	int8	fullfilename[256], url[1024];

	sprintf( fullfilename, "cache\\%s.xml", location);
	sprintf( url, "/weather/local/%s?cc=*&dayf=%d&unit=%c", location, XML_WEATHER_DAYF_N, unit == WEATHER_METRIC ? 'm' : 's');
	strcat( url, "&link=xoap&prod=xoap&par=1160181056&key=1031965b2f676b1b" );

	if( !http_get_file( url, "xoap.weather.com", fullfilename))
		return NULL;

	stat( fullfilename, &attrs);

	if( attrs.st_size < 1024)
	{
		errshow( "", BADDATA);
		return NULL;
	}

	if( !( doc = xmlParseFile( fullfilename)))
		return NULL;

	cur_node = xmlDocGetRootElement( doc);

	if( cur_node)
	{
		const int8 *sTmp;
		
		weather = parse_weather( cur_node);

		sprintf( weather_form[WEATHER_REPORT].ob_spec.tedinfo->te_ptext, "%s", get_data(weather, DNAM));	
		sprintf( weather_form[WEATHER_TIME].ob_spec.tedinfo->te_ptext, "Last update: %s", get_data(weather, LSUP));	
		sprintf( weather_form[WEATHER_TEMP].ob_spec.tedinfo->te_ptext, "%s%s", get_data(weather, TEMP), get_unit(unit, TEMP));	
		sprintf( weather_form[WEATHER_FEEL].ob_spec.tedinfo->te_ptext, "%s%s", get_data(weather, FLIK), get_unit(unit, FLIK));	
		zstrncpy( weather_form[WEATHER_DESCRIPTION].ob_spec.tedinfo->te_ptext, get_data(weather, TRANS), 26L);
		sprintf( weather_form[WEATHER_DEW].ob_spec.tedinfo->te_ptext, "%s%s", get_data(weather, DEWP), get_unit(unit, DEWP));	
		sprintf( weather_form[WEATHER_WSPEED].ob_spec.tedinfo->te_ptext, "%s %s", get_data(weather, WIND_SPEED), get_unit(unit, WIND_SPEED));	

		sprintf( weather_form[WEATHER_SUNRISE].ob_spec.tedinfo->te_ptext, "%s", get_data(weather, SUNR));
		sprintf( weather_form[WEATHER_SUNSET].ob_spec.tedinfo->te_ptext, "%s", get_data(weather, SUNS));
		sprintf( weather_form[WEATHER_UV].ob_spec.tedinfo->te_ptext, "%s %s", get_data(weather, UV_INDEX), get_data(weather, UV_TRANS));
		sprintf( weather_form[WEATHER_HUMIDITY].ob_spec.tedinfo->te_ptext, "%s%s", get_data(weather, HMID), get_unit(unit, HMID));
		sprintf( weather_form[WEATHER_VISIBILITY].ob_spec.tedinfo->te_ptext, "%s", get_data(weather, VIS));
		sprintf( weather_form[WEATHER_PRESSURE].ob_spec.tedinfo->te_ptext, "%s %s and %s.", get_data(weather, BAR_R), get_unit(unit, BAR_R), get_data(weather, BAR_D));

		sprintf( ObjcString( weather_form, WEATHER_FORECAST1, NULL), " %s ", get_data_f(weather->dayf[0], WDAY));
		// today data may be outdated afternoon
		sTmp = get_data_f(weather->dayf[0], TEMP_MAX);
		strcpy( weather_form[WEATHER_DAY1].ob_spec.tedinfo->te_ptext, sTmp);
		if( strcmp( sTmp, "N/A") != 0)
		{
			strcat( weather_form[WEATHER_DAY1].ob_spec.tedinfo->te_ptext, get_unit(unit, TEMP));
		}
		sTmp = get_data_f(weather->dayf[0], TEMP_MIN);
		strcpy( weather_form[WEATHER_NIGHT1].ob_spec.tedinfo->te_ptext, sTmp);
		if( strcmp( sTmp, "N/A") != 0)
		{
			strcat( weather_form[WEATHER_NIGHT1].ob_spec.tedinfo->te_ptext, get_unit(unit, TEMP));
		}
		sTmp = get_data_f(weather->dayf[0], W_SPEED_D);
		strcpy( weather_form[WEATHER_WIND1].ob_spec.tedinfo->te_ptext, sTmp);
		if( strcmp( sTmp, "N/A") != 0)
		{
			strcat( weather_form[WEATHER_WIND1].ob_spec.tedinfo->te_ptext, " ");
			strcat( weather_form[WEATHER_WIND1].ob_spec.tedinfo->te_ptext, get_unit(unit, WIND_SPEED));
		}
		zstrncpy( weather_form[WEATHER_DESCRIPTION1].ob_spec.tedinfo->te_ptext, get_data_f(weather->dayf[0], TRANS_D), 18L);
	
		sprintf( ObjcString( weather_form, WEATHER_FORECAST2, NULL), " %s ", get_data_f(weather->dayf[1], WDAY));
		sprintf( weather_form[WEATHER_DAY2].ob_spec.tedinfo->te_ptext, "%s%s",get_data_f(weather->dayf[1], TEMP_MAX), get_unit(unit, TEMP));
		sprintf( weather_form[WEATHER_NIGHT2].ob_spec.tedinfo->te_ptext, "%s%s",get_data_f(weather->dayf[1], TEMP_MIN), get_unit(unit, TEMP));
		sprintf( weather_form[WEATHER_WIND2].ob_spec.tedinfo->te_ptext, "%s %s",get_data_f(weather->dayf[1], W_SPEED_D), get_unit(unit, WIND_SPEED));
		zstrncpy( weather_form[WEATHER_DESCRIPTION2].ob_spec.tedinfo->te_ptext, get_data_f(weather->dayf[1], TRANS_D), 18L);

		sprintf( ObjcString( weather_form, WEATHER_FORECAST3, NULL), " %s ", get_data_f(weather->dayf[2], WDAY));
		sprintf( weather_form[WEATHER_DAY3].ob_spec.tedinfo->te_ptext, "%s%s",get_data_f(weather->dayf[2], TEMP_MAX), get_unit(unit, TEMP));
		sprintf( weather_form[WEATHER_NIGHT3].ob_spec.tedinfo->te_ptext, "%s%s",get_data_f(weather->dayf[2], TEMP_MIN), get_unit(unit, TEMP));
		sprintf( weather_form[WEATHER_WIND3].ob_spec.tedinfo->te_ptext, "%s %s",get_data_f(weather->dayf[2], W_SPEED_D), get_unit(unit, WIND_SPEED));
		zstrncpy( weather_form[WEATHER_DESCRIPTION3].ob_spec.tedinfo->te_ptext, get_data_f(weather->dayf[2], TRANS_D), 18L);

		sprintf( ObjcString( weather_form, WEATHER_FORECAST4, NULL), " %s ", get_data_f(weather->dayf[3], WDAY));
		sprintf( weather_form[WEATHER_DAY4].ob_spec.tedinfo->te_ptext, "%s%s",get_data_f(weather->dayf[3], TEMP_MAX), get_unit(unit, TEMP));
		sprintf( weather_form[WEATHER_NIGHT4].ob_spec.tedinfo->te_ptext, "%s%s",get_data_f(weather->dayf[3], TEMP_MIN), get_unit(unit, TEMP));
		sprintf( weather_form[WEATHER_WIND4].ob_spec.tedinfo->te_ptext, "%s %s",get_data_f(weather->dayf[3], W_SPEED_D), get_unit(unit, WIND_SPEED));
		zstrncpy( weather_form[WEATHER_DESCRIPTION4].ob_spec.tedinfo->te_ptext, get_data_f(weather->dayf[3], TRANS_D), 18L);

		sprintf( ObjcString( weather_form, WEATHER_FORECAST5, NULL), " %s ", get_data_f(weather->dayf[4], WDAY));
		sprintf( weather_form[WEATHER_DAY5].ob_spec.tedinfo->te_ptext, "%s%s",get_data_f(weather->dayf[4], TEMP_MAX), get_unit(unit, TEMP));
		sprintf( weather_form[WEATHER_NIGHT5].ob_spec.tedinfo->te_ptext, "%s%s",get_data_f(weather->dayf[4], TEMP_MIN), get_unit(unit, TEMP));
		sprintf( weather_form[WEATHER_WIND5].ob_spec.tedinfo->te_ptext, "%s %s",get_data_f(weather->dayf[4], W_SPEED_D), get_unit(unit, WIND_SPEED));
		zstrncpy( weather_form[WEATHER_DESCRIPTION5].ob_spec.tedinfo->te_ptext, get_data_f(weather->dayf[4], TRANS_D), 18L);
		
		zstrncpy( weather_form[WEATHER_LINK1].ob_spec.tedinfo->te_ptext, get_lnks(weather->lnks, TEXT, 0), 20L);
		zstrncpy( weather_form[WEATHER_LINK2].ob_spec.tedinfo->te_ptext, get_lnks(weather->lnks, TEXT, 1), 20L);
		zstrncpy( weather_form[WEATHER_LINK3].ob_spec.tedinfo->te_ptext, get_lnks(weather->lnks, TEXT, 2), 20L);
		zstrncpy( weather_form[WEATHER_LINK4].ob_spec.tedinfo->te_ptext, get_lnks(weather->lnks, TEXT, 3), 20L);

		icon_to_draw = atoi( ( const char*)get_data(weather, WICON));
		main_icon = icons[icon_to_draw];
	
		icon_to_draw = atoi( ( const char*)get_data_f(weather->dayf[0], ICON_D));
		forecast_icon[0] = icons[icon_to_draw];

		icon_to_draw = atoi( ( const char*)get_data_f(weather->dayf[1], ICON_D));
		forecast_icon[1] = icons[icon_to_draw];

		icon_to_draw = atoi( ( const char*)get_data_f(weather->dayf[2], ICON_D));
		forecast_icon[2] = icons[icon_to_draw];

		icon_to_draw = atoi( ( const char*)get_data_f(weather->dayf[3], ICON_D));
		forecast_icon[3] = icons[icon_to_draw];

		icon_to_draw = atoi( ( const char*)get_data_f(weather->dayf[4], ICON_D));
		forecast_icon[4] = icons[icon_to_draw];

		sprintf( icon_temp, "%sø", get_data( weather, TEMP));
		sprintf( wind_direction, "%s", get_data( weather, WIND_DIRECTION)); 
	}

	xmlFreeDoc( doc);
	
	return weather;
}

void timer_function( WINDOW *win)
{
	#ifdef DEBUG 
	char   tmp[15];
	struct tm	*tmt;
	time_t	tim;
	#endif

	clock_t current_t = clock();
	clock_t relative_t;
	
	relative_t = current_t - chrono_value;
	
	if( relative_t < update_time)
		return;
	
	chrono_value = current_t;

	#ifdef DEBUG 
	tim = time( NULL);
	tmt = localtime( &tim);
	strftime( tmp, 12, "%H:%M:%S", tmt);
	zdebug( "update: %s", tmp);
	#endif

	if( zweatherdata)
		xml_weather_free( zweatherdata);

	zweatherdata = update_weatherdata( location_code);

	if( windialog)
		snd_rdw( windialog);
}

void applexit( void) 
{
    if( EvntFind( NULL, WM_XTIMER))
		EvntDelete( NULL, WM_XTIMER);

    while( wglb.first) 
    {
        snd_msg( wglb.first, WM_DESTROY, 0, 0, 0, 0);
        EvntWindom( MU_MESAG);
    }

	if( zweatherdata)
		xml_weather_free( zweatherdata);

	prefs_write();
	unregister_icons();
	RsrcXtype( 0, NULL, 0);
	RsrcFree();
	xmlCleanupParser();
	ApplExit();
	exit( 0);
}


int main( int argc, char *argv[])
{
	ApplInit();

	if( !RsrcLoad( "zweather.rsc")) 
	{
		errshow( "", E_RSC);
		ApplExit();
		return 0;
	}

	RsrcXtype( RSRC_XTYPE, NULL, 0);
	
	if( !register_icons())
	{
		errshow( "", NOZCODECS);
		unregister_icons();
		RsrcXtype( 0, NULL, 0);
		RsrcFree();
		ApplExit();
		return 0;
	}

	prefs_read();

	if( use_sting)
		init_stik();

	zweatherdata = update_weatherdata( location_code);

	evnt.timer = 1000L;

	weather_dialog();

	chrono_value = clock();
	EvntAttach( NULL, WM_XTIMER, timer_function);
    EvntAttach( NULL, AP_TERM,  applexit);

	for(;;) 
		EvntWindom( MU_MESAG|MU_TIMER|MU_KEYBD|MU_BUTTON);

	applexit();
	return 0;
}

