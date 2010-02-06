#include "general.h"
#include "icon.h"
#include "get_data.h"

/* the variable need to be global because this function is done by */
/* the AES in supervisor mode.. it's safe. */
static int16 xy[8];
static MFDB  screen = {0};
int8	wind_direction[8] = "";
extern struct xml_weather *zweatherdata;

OBJECT *get_tree( int16 obj_index)
{
	OBJECT *tree;
	RsrcGaddr( NULL, 0, obj_index, &tree);
	return tree;
}


/* Return a string from resource file */

int8 *get_string( int16 str_index)
{
	int8 *txt;
	rsrc_gaddr( 5, str_index,  &txt);
	return txt;
}

void CDECL draw_boussole_icon( WINDOW *win, PARMBLK *pblk, void *data)
{
	xy[0] = xy[1] = 0;
	xy[2] = 47;
	xy[3] = 47;
	xy[4] = pblk->pb_x;
	xy[5] = pblk->pb_y;
	xy[6] = xy[4] + 47;
	xy[7] = xy[5] + 47;

	vro_cpyfm( win->graf->handle, S_ONLY, xy, &boussole, &screen);

	xy[2] = pblk->pb_x + 23;
	xy[3] = pblk->pb_y + 23;

	switch( wind_direction[0])
	{
		case 'N':
			if( strcmp( wind_direction, "NNE") == 0)
			{
				xy[0] = pblk->pb_x + 27;
				xy[1] = pblk->pb_y + 9;
				break;
			}

			if( strcmp( wind_direction, "NNW") == 0)
			{
				xy[0] = pblk->pb_x + 18;
				xy[1] = pblk->pb_y + 9;
				break;
			}

			if( strcmp( wind_direction, "NW") == 0)
			{
				xy[0] = pblk->pb_x + 12;
				xy[1] = pblk->pb_y + 12;
				break;
			}

			if( strcmp( wind_direction, "NE") == 0)
			{
				xy[0] = pblk->pb_x + 34;
				xy[1] = pblk->pb_y + 12;
				break;
			}

			/* default 'N'*/
			xy[0] = pblk->pb_x + 23;
			xy[1] = pblk->pb_y + 8;
			break;

		case 'E':
			if( strcmp( wind_direction, "ENE") == 0)
			{
				xy[0] = pblk->pb_x + 36;
				xy[1] = pblk->pb_y + 17;
				break;
			}

			if( strcmp( wind_direction, "ESE") == 0)
			{
				xy[0] = pblk->pb_x + 36;
				xy[1] = pblk->pb_y + 28;
				break;
			}

			/* default 'E'*/
			xy[0] = pblk->pb_x + 38;
			xy[1] = pblk->pb_y + 23;
			break;

		case 'W':
			if( strcmp( wind_direction, "WSW") == 0)
			{
				xy[0] = pblk->pb_x + 9;
				xy[1] = pblk->pb_y + 28;
				break;
			}

			if( strcmp( wind_direction, "WNW") == 0)
			{
				xy[0] = pblk->pb_x + 9;
				xy[1] = pblk->pb_y + 17;
				break;
			}

			/* default 'W'*/
			xy[0] = pblk->pb_x + 8;
			xy[1] = pblk->pb_y + 23;
			break;

		case 'S':
			if( strcmp( wind_direction, "SSE") == 0)
			{
				xy[0] = pblk->pb_x + 28;
				xy[1] = pblk->pb_y + 36;
				break;
			}

			if( strcmp( wind_direction, "SSW") == 0)
			{
				xy[0] = pblk->pb_x + 18;
				xy[1] = pblk->pb_y + 36;
				break;
			}

			if( strcmp( wind_direction, "SW") == 0)
			{
				xy[0] = pblk->pb_x + 12;
				xy[1] = pblk->pb_y + 33;
				break;
			}

			if( strcmp( wind_direction, "SE") == 0)
			{
				xy[0] = pblk->pb_x + 34;
				xy[1] = pblk->pb_y + 33;;
				break;
			}

		default:

			/* default 'S'*/
			xy[0] = pblk->pb_x + 23;
			xy[1] = pblk->pb_y + 38;
			break;
	}

	v_pline( win->graf->handle, 2, xy);
}

void CDECL draw_weather_icon( WINDOW *win, PARMBLK *pblk, void *data)
{
	xy[0] = xy[1] = 0;
	xy[2] = 47;
	xy[3] = 47;
	xy[4] = pblk->pb_x;
	xy[5] = pblk->pb_y;
	xy[6] = xy[4] + 47;
	xy[7] = xy[5] + 47;

	vro_cpyfm( win->graf->handle, S_ONLY, xy, main_icon, &screen);
}

void CDECL draw_forecast1_icon( WINDOW *win, PARMBLK *pblk, void *data)
{
	xy[0] = xy[1] = 0;
	xy[2] = 47;
	xy[3] = 47;
	xy[4] = pblk->pb_x;
	xy[5] = pblk->pb_y;
	xy[6] = xy[4] + 47;
	xy[7] = xy[5] + 47;

	vro_cpyfm( win->graf->handle, S_ONLY, xy, forecast_icon[0], &screen);
}


void CDECL draw_forecast2_icon( WINDOW *win, PARMBLK *pblk, void *data)
{
	xy[0] = xy[1] = 0;
	xy[2] = 47;
	xy[3] = 47;
	xy[4] = pblk->pb_x;
	xy[5] = pblk->pb_y;
	xy[6] = xy[4] + 47;
	xy[7] = xy[5] + 47;

	vro_cpyfm( win->graf->handle, S_ONLY, xy, forecast_icon[1], &screen);
}

void CDECL draw_forecast3_icon( WINDOW *win, PARMBLK *pblk, void *data)
{
	xy[0] = xy[1] = 0;
	xy[2] = 47;
	xy[3] = 47;
	xy[4] = pblk->pb_x;
	xy[5] = pblk->pb_y;
	xy[6] = xy[4] + 47;
	xy[7] = xy[5] + 47;

	vro_cpyfm( win->graf->handle, S_ONLY, xy, forecast_icon[2], &screen);
}

void CDECL draw_forecast4_icon( WINDOW *win, PARMBLK *pblk, void *data)
{
	xy[0] = xy[1] = 0;
	xy[2] = 47;
	xy[3] = 47;
	xy[4] = pblk->pb_x;
	xy[5] = pblk->pb_y;
	xy[6] = xy[4] + 47;
	xy[7] = xy[5] + 47;

	vro_cpyfm( win->graf->handle, S_ONLY, xy, forecast_icon[3], &screen);
}

void CDECL draw_forecast5_icon( WINDOW *win, PARMBLK *pblk, void *data)
{
	xy[0] = xy[1] = 0;
	xy[2] = 47;
	xy[3] = 47;
	xy[4] = pblk->pb_x;
	xy[5] = pblk->pb_y;
	xy[6] = xy[4] + 47;
	xy[7] = xy[5] + 47;

	vro_cpyfm( win->graf->handle, S_ONLY, xy, forecast_icon[4], &screen);
}

void CDECL draw_forecast6_icon( WINDOW *win, PARMBLK *pblk, void *data)
{
	xy[0] = xy[1] = 0;
	xy[2] = 60;
	xy[3] = 60;
	xy[4] = pblk->pb_x;
	xy[5] = pblk->pb_y;
	xy[6] = xy[4] + 60;
	xy[7] = xy[5] + 60;

	vro_cpyfm( win->graf->handle, S_ONLY, xy, &logo->image[0], &screen);
}



void errshow( const char *name, int16 error)
{
  	switch( error)
  	{
		case ALERT_WINDOW:
			( void)FormAlert( 1 , get_string( ALERT_WINDOW));
			break;
		case ENOMEM:
			( void)FormAlert( 1 , get_string( NOMEM));
			break;
		case E_RSC:
 			( void)FormAlert( 1 , "[1][zweather.rsc not found!][Abort]");
      		break;
		case CANT_LOAD_IMG:
 			( void)FormAlert( 1 , get_string( CANT_LOAD_IMG), name);
      		break;
		case NOZCODECS:
 			( void)FormAlert( 1 , get_string( NOZCODECS));
      		break;
		case BADDATA:
 			( void)FormAlert( 1 , get_string( BADDATA));
      		break;
		default:
			( void)FormAlert( 1 , get_string( GENERIC));
			break;
	}
}






