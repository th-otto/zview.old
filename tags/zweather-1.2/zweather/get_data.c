#include "general.h"
#include "get_data.h"

int8 dummy[2] = "0";


int8 *get_data_uv(struct xml_uv *data, enum datas_uv type)
{
	int8 *str = dummy;

	if ( !data)
		return str;
	
	switch(type)
	{
		case _UV_INDEX: str = data->i; break;
		case _UV_TRANS: str = data->t; break;
		default:	break;
	}

	return str;
}
 

int8 *get_data_bar(struct xml_bar *data, enum datas_bar type)
{
	int8 *str = dummy;

	if ( !data)
		return str;

	switch(type)
	{
		case _BAR_R: str = data->r; break;
		case _BAR_D: str = data->d; break;
		default:	break;
	}

	return str;
}

int8 *get_data_wind(struct xml_wind *data, enum datas_wind type)
{
	int8 *str = dummy;

	if ( !data)
		return str;

	switch(type)
	{
		case _WIND_SPEED: str = data->s; break;
		case _WIND_GUST: str = data->gust; break;
		case _WIND_DIRECTION: str = data->t; break;
		case _WIND_TRANS: str = data->d; break;
		default:	break;
	}

	return str;
}

int8 *get_data_cc( struct xml_cc *data, enum datas type)
{ 
	int8 *str = dummy;

	if ( !data)
		return str;

	switch(type)
	{
		case LSUP: str = data->lsup; break;
		case OBST: str = data->obst; break;
		case FLIK: str = data->flik; break;
		case TRANS:    str = data->t; break;
		case TEMP:  str = data->tmp; break;
		case HMID: str = data->hmid; break;
		case VIS:  str = data->vis; break;
		case UV_INDEX:   return get_data_uv(data->uv, _UV_INDEX);
		case UV_TRANS:   return get_data_uv(data->uv, _UV_TRANS);
		case WIND_SPEED: return get_data_wind(data->wind, _WIND_SPEED);
		case WIND_GUST: return get_data_wind(data->wind, _WIND_GUST);
		case WIND_DIRECTION: return get_data_wind(data->wind, _WIND_DIRECTION);
		case WIND_TRANS: return get_data_wind(data->wind, _WIND_TRANS);
		case BAR_R:  return get_data_bar(data->bar, _BAR_R);
		case BAR_D: return get_data_bar(data->bar, _BAR_D);
		case DEWP: str = data->dewp; break;
		case WICON: str = data->icon; break;
		default:	break;
	}
	return str;
}

int8 *get_data_loc(struct xml_loc *data, enum datas_loc type)
{ 
	int8 *str = dummy;

	if ( !data)
		return str;

	switch(type)
	{
		case DNAM: str = data->dnam; break;
		case SUNR: str = data->sunr; break;
		case SUNS: str = data->suns; break;
		default:	break;
	}

	return str;
}

const int8 *get_data(struct xml_weather *data, enum datas type)
{
	int8 *str = dummy;

	if( data)
	{
		switch (type & 0xFF00)
		{
			case DATAS_CC: str = get_data_cc(data->cc, type); break;
			case DATAS_LOC: str = get_data_loc(data->loc, type); break;
			default:	break;
		}
	}

	return str;
}

int8 *get_data_part(struct xml_part *data, enum forecast type)
{
	int8 *str = dummy;

	if ( !data)
		return str;

	switch (type & 0x000F)
	{
		case F_ICON: str = data->icon; break;
		case F_TRANS: str = data->t; break;
		case F_PPCP: str = data->ppcp; break;
		case F_W_SPEED: str = get_data_wind(data->wind, _WIND_SPEED); break;
		case F_W_DIRECTION: str = get_data_wind(data->wind, _WIND_DIRECTION); break;
		default:	break;
	}

	return str;
}

const int8 *get_data_f(struct xml_dayf *data, enum forecast type)
{
	int8 *str = dummy;

	if (data)
	{
		switch (type & 0x0F00)
		{
			case ITEMS:
				switch(type)
				{
					case WDAY:		str = data->day; break;
					case TEMP_MIN:	str = data->low; break;
					case TEMP_MAX:	str = data->hi; break;
					default:	break;
				}
				break;
			case NPART: str = get_data_part(data->part[1], type); break;
			case DPART: str = get_data_part(data->part[0], type); break; 
		}
	}
        
	return str;
}

const int8 *get_unit( int myunit, enum datas type)
{
	int8 *str = dummy;
        
	switch (type & 0x00F0)
	{
		case 0x0020: str = (myunit == WEATHER_METRIC ? "ø C" : "ø F"); break;
		case 0x0030: str = "%"; break;
		case 0x0040: str = (myunit == WEATHER_METRIC ? "km/h" : "mph"); break;
		case 0x0050: str = (myunit == WEATHER_METRIC ? "mb" : "in"); break;
		default: break;
	}

	return str;
}

const int8 *get_lnks(struct xml_lnks *lnks, enum link type, int num)
{
	int8* str = "";
	
	if( !lnks || num >= XML_WEATHER_LINK_N)
		return str;
	
	switch (type)
	{
		case LINK:
			str = lnks->l[num];
		case TEXT:
			str = lnks->t[num];
		default:
			break;
	}
	
	return str;
}
