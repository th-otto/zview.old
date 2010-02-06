#include "general.h"

IMAGE *weather_icons = { NULL };
MFDB *main_icon = NULL;
MFDB boussole;
IMAGE *logo = NULL;
MFDB *forecast_icon[5] = { NULL };
MFDB icons[48];

int16 codec_load = 0;

int16 register_icons( void)
{
	MFDB *bm;
	uint8 *data;
	uint32 icon_buffer_size = 288 * app.nplanes;

	if( !( codec_load = codecs_init( "png.ldg")))
		return 0;

	if( !( weather_icons = load_img( "icons\\weather.png", -1, -1, 1)))
		return 0;

	if( !( logo = load_img( "icons\\logo.png", -1, -1, 1)))
		return 0;

	bm   = &icons[1];	// why 1 ???
	data = weather_icons->image->fd_addr;
	bm->fd_w 		= 48;
	bm->fd_h 		= 48;
	bm->fd_wdwidth	= 3;
	bm->fd_stand 	= 0;
	bm->fd_nplanes 	= app.nplanes;
	bm->fd_r1      	= 0;
	bm->fd_r2      	= 0;
	bm->fd_r3	  	= 0;
	bm->fd_addr 	= data;

	bm   = &icons[2];
	bm->fd_w 		= 48;
	bm->fd_h 		= 48;
	bm->fd_wdwidth	= 3;
	bm->fd_stand 	= 0;
	bm->fd_nplanes 	= app.nplanes;
	bm->fd_r1      	= 0;
	bm->fd_r2      	= 0;
	bm->fd_r3	  	= 0;
	bm->fd_addr 	= data;

	bm   = &icons[3];
	data += icon_buffer_size;
	bm->fd_w 		= 48;
	bm->fd_h 		= 48;
	bm->fd_wdwidth	= 3;
	bm->fd_stand 	= 0;
	bm->fd_nplanes 	= app.nplanes;
	bm->fd_r1      	= 0;
	bm->fd_r2      	= 0;
	bm->fd_r3	  	= 0;
	bm->fd_addr 	= data;

	bm   = &icons[4];
	bm->fd_w 		= 48;
	bm->fd_h 		= 48;
	bm->fd_wdwidth	= 3;
	bm->fd_stand 	= 0;
	bm->fd_nplanes 	= app.nplanes;
	bm->fd_r1      	= 0;
	bm->fd_r2      	= 0;
	bm->fd_r3	  	= 0;
	bm->fd_addr 	= data;

	bm   = &icons[5];
	data += icon_buffer_size;
	bm->fd_w 		= 48;
	bm->fd_h 		= 48;
	bm->fd_wdwidth	= 3;
	bm->fd_stand 	= 0;
	bm->fd_nplanes 	= app.nplanes;
	bm->fd_r1      	= 0;
	bm->fd_r2      	= 0;
	bm->fd_r3	  	= 0;
	bm->fd_addr 	= data;

	bm   = &icons[6];
	data += icon_buffer_size;
	bm->fd_w 		= 48;
	bm->fd_h 		= 48;
	bm->fd_wdwidth	= 3;
	bm->fd_stand 	= 0;
	bm->fd_nplanes 	= app.nplanes;
	bm->fd_r1      	= 0;
	bm->fd_r2      	= 0;
	bm->fd_r3	  	= 0;
	bm->fd_addr 	= data;

	bm   = &icons[7];
	data += icon_buffer_size;
	bm->fd_w 		= 48;
	bm->fd_h 		= 48;
	bm->fd_wdwidth	= 3;
	bm->fd_stand 	= 0;
	bm->fd_nplanes 	= app.nplanes;
	bm->fd_r1      	= 0;
	bm->fd_r2      	= 0;
	bm->fd_r3	  	= 0;
	bm->fd_addr 	= data;

	bm   = &icons[8];
	bm->fd_w 		= 48;
	bm->fd_h 		= 48;
	bm->fd_wdwidth	= 3;
	bm->fd_stand 	= 0;
	bm->fd_nplanes 	= app.nplanes;
	bm->fd_r1      	= 0;
	bm->fd_r2      	= 0;
	bm->fd_r3	  	= 0;
	bm->fd_addr 	= data;

	bm   = &icons[9];
	data += icon_buffer_size;
	bm->fd_w 		= 48;
	bm->fd_h 		= 48;
	bm->fd_wdwidth	= 3;
	bm->fd_stand 	= 0;
	bm->fd_nplanes 	= app.nplanes;
	bm->fd_r1      	= 0;
	bm->fd_r2      	= 0;
	bm->fd_r3	  	= 0;
	bm->fd_addr 	= data;

	bm   = &icons[10];
	bm->fd_w 		= 48;
	bm->fd_h 		= 48;
	bm->fd_wdwidth	= 3;
	bm->fd_stand 	= 0;
	bm->fd_nplanes 	= app.nplanes;
	bm->fd_r1      	= 0;
	bm->fd_r2      	= 0;
	bm->fd_r3	  	= 0;
	bm->fd_addr 	= icons[8].fd_addr;

	bm   = &icons[11];
	bm->fd_w 		= 48;
	bm->fd_h 		= 48;
	bm->fd_wdwidth	= 3;
	bm->fd_stand 	= 0;
	bm->fd_nplanes 	= app.nplanes;
	bm->fd_r1      	= 0;
	bm->fd_r2      	= 0;
	bm->fd_r3	  	= 0;
	bm->fd_addr 	= icons[6].fd_addr;

	bm   = &icons[12];
	data += icon_buffer_size;
	bm->fd_w 		= 48;
	bm->fd_h 		= 48;
	bm->fd_wdwidth	= 3;
	bm->fd_stand 	= 0;
	bm->fd_nplanes 	= app.nplanes;
	bm->fd_r1      	= 0;
	bm->fd_r2      	= 0;
	bm->fd_r3	  	= 0;
	bm->fd_addr 	= data;

	bm   = &icons[13];
	data += icon_buffer_size;
	bm->fd_w 		= 48;
	bm->fd_h 		= 48;
	bm->fd_wdwidth	= 3;
	bm->fd_stand 	= 0;
	bm->fd_nplanes 	= app.nplanes;
	bm->fd_r1      	= 0;
	bm->fd_r2      	= 0;
	bm->fd_r3	  	= 0;
	bm->fd_addr 	= data;

	bm   = &icons[14];
	data += icon_buffer_size;
	bm->fd_w 		= 48;
	bm->fd_h 		= 48;
	bm->fd_wdwidth	= 3;
	bm->fd_stand 	= 0;
	bm->fd_nplanes 	= app.nplanes;
	bm->fd_r1      	= 0;
	bm->fd_r2      	= 0;
	bm->fd_r3	  	= 0;
	bm->fd_addr 	= data;

	bm   = &icons[15];
	bm->fd_w 		= 48;
	bm->fd_h 		= 48;
	bm->fd_wdwidth	= 3;
	bm->fd_stand 	= 0;
	bm->fd_nplanes 	= app.nplanes;
	bm->fd_r1      	= 0;
	bm->fd_r2      	= 0;
	bm->fd_r3	  	= 0;
	bm->fd_addr 	= icons[5].fd_addr;

	bm   = &icons[16];
	bm->fd_w 		= 48;
	bm->fd_h 		= 48;
	bm->fd_wdwidth	= 3;
	bm->fd_stand 	= 0;
	bm->fd_nplanes 	= app.nplanes;
	bm->fd_r1      	= 0;
	bm->fd_r2      	= 0;
	bm->fd_r3	  	= 0;
	bm->fd_addr 	= icons[5].fd_addr;

	bm   = &icons[17];
	bm->fd_w 		= 48;
	bm->fd_h 		= 48;
	bm->fd_wdwidth	= 3;
	bm->fd_stand 	= 0;
	bm->fd_nplanes 	= app.nplanes;
	bm->fd_r1      	= 0;
	bm->fd_r2      	= 0;
	bm->fd_r3	  	= 0;
	bm->fd_addr 	= icons[3].fd_addr;

	bm   = &icons[18];
	bm->fd_w 		= 48;
	bm->fd_h 		= 48;
	bm->fd_wdwidth	= 3;
	bm->fd_stand 	= 0;
	bm->fd_nplanes 	= app.nplanes;
	bm->fd_r1      	= 0;
	bm->fd_r2      	= 0;
	bm->fd_r3	  	= 0;
	bm->fd_addr 	= icons[12].fd_addr;

	bm   = &icons[19];
	data += icon_buffer_size;
	bm->fd_w 		= 48;
	bm->fd_h 		= 48;
	bm->fd_wdwidth	= 3;
	bm->fd_stand 	= 0;
	bm->fd_nplanes 	= app.nplanes;
	bm->fd_r1      	= 0;
	bm->fd_r2      	= 0;
	bm->fd_r3	  	= 0;
	bm->fd_addr 	= data;

	bm   = &icons[20];
	data += icon_buffer_size;
	bm->fd_w 		= 48;
	bm->fd_h 		= 48;
	bm->fd_wdwidth	= 3;
	bm->fd_stand 	= 0;
	bm->fd_nplanes 	= app.nplanes;
	bm->fd_r1      	= 0;
	bm->fd_r2      	= 0;
	bm->fd_r3	  	= 0;
	bm->fd_addr 	= data;

	bm   = &icons[21];
	bm->fd_w 		= 48;
	bm->fd_h 		= 48;
	bm->fd_wdwidth	= 3;
	bm->fd_stand 	= 0;
	bm->fd_nplanes 	= app.nplanes;
	bm->fd_r1      	= 0;
	bm->fd_r2      	= 0;
	bm->fd_r3	  	= 0;
	bm->fd_addr 	= icons[19].fd_addr;

	bm   = &icons[22];
	data += icon_buffer_size;
	bm->fd_w 		= 48;
	bm->fd_h 		= 48;
	bm->fd_wdwidth	= 3;
	bm->fd_stand 	= 0;
	bm->fd_nplanes 	= app.nplanes;
	bm->fd_r1      	= 0;
	bm->fd_r2      	= 0;
	bm->fd_r3	  	= 0;
	bm->fd_addr 	= data;

	bm   = &icons[23];
	bm->fd_w 		= 48;
	bm->fd_h 		= 48;
	bm->fd_wdwidth	= 3;
	bm->fd_stand 	= 0;
	bm->fd_nplanes 	= app.nplanes;
	bm->fd_r1      	= 0;
	bm->fd_r2      	= 0;
	bm->fd_r3	  	= 0;
	bm->fd_addr 	= icons[1].fd_addr;

	bm   = &icons[24];
	bm->fd_w 		= 48;
	bm->fd_h 		= 48;
	bm->fd_wdwidth	= 3;
	bm->fd_stand 	= 0;
	bm->fd_nplanes 	= app.nplanes;
	bm->fd_r1      	= 0;
	bm->fd_r2      	= 0;
	bm->fd_r3	  	= 0;
	bm->fd_addr 	= icons[1].fd_addr;

	bm   = &icons[25];
	data += icon_buffer_size;
	bm->fd_w 		= 48;
	bm->fd_h 		= 48;
	bm->fd_wdwidth	= 3;
	bm->fd_stand 	= 0;
	bm->fd_nplanes 	= app.nplanes;
	bm->fd_r1      	= 0;
	bm->fd_r2      	= 0;
	bm->fd_r3	  	= 0;
	bm->fd_addr 	= data;

	bm   = &icons[26];
	data += icon_buffer_size;
	bm->fd_w 		= 48;
	bm->fd_h 		= 48;
	bm->fd_wdwidth	= 3;
	bm->fd_stand 	= 0;
	bm->fd_nplanes 	= app.nplanes;
	bm->fd_r1      	= 0;
	bm->fd_r2      	= 0;
	bm->fd_r3	  	= 0;
	bm->fd_addr 	= data;

	bm   = &icons[27];
	data += icon_buffer_size;
	bm->fd_w 		= 48;
	bm->fd_h 		= 48;
	bm->fd_wdwidth	= 3;
	bm->fd_stand 	= 0;
	bm->fd_nplanes 	= app.nplanes;
	bm->fd_r1      	= 0;
	bm->fd_r2      	= 0;
	bm->fd_r3	  	= 0;
	bm->fd_addr 	= data;

	bm   = &icons[28];
	data += icon_buffer_size;
	bm->fd_w 		= 48;
	bm->fd_h 		= 48;
	bm->fd_wdwidth	= 3;
	bm->fd_stand 	= 0;
	bm->fd_nplanes 	= app.nplanes;
	bm->fd_r1      	= 0;
	bm->fd_r2      	= 0;
	bm->fd_r3	  	= 0;
	bm->fd_addr 	= data;

	bm   = &icons[29];
	data += icon_buffer_size;
	bm->fd_w 		= 48;
	bm->fd_h 		= 48;
	bm->fd_wdwidth	= 3;
	bm->fd_stand 	= 0;
	bm->fd_nplanes 	= app.nplanes;
	bm->fd_r1      	= 0;
	bm->fd_r2      	= 0;
	bm->fd_r3	  	= 0;
	bm->fd_addr 	= data;

	bm   = &icons[30];
	data += icon_buffer_size;
	bm->fd_w 		= 48;
	bm->fd_h 		= 48;
	bm->fd_wdwidth	= 3;
	bm->fd_stand 	= 0;
	bm->fd_nplanes 	= app.nplanes;
	bm->fd_r1      	= 0;
	bm->fd_r2      	= 0;
	bm->fd_r3	  	= 0;
	bm->fd_addr 	= data;

	bm   = &icons[31];
	data += icon_buffer_size;
	bm->fd_w 		= 48;
	bm->fd_h 		= 48;
	bm->fd_wdwidth	= 3;
	bm->fd_stand 	= 0;
	bm->fd_nplanes 	= app.nplanes;
	bm->fd_r1      	= 0;
	bm->fd_r2      	= 0;
	bm->fd_r3	  	= 0;
	bm->fd_addr 	= data;

	bm   = &icons[32];
	data += icon_buffer_size;
	bm->fd_w 		= 48;
	bm->fd_h 		= 48;
	bm->fd_wdwidth	= 3;
	bm->fd_stand 	= 0;
	bm->fd_nplanes 	= app.nplanes;
	bm->fd_r1      	= 0;
	bm->fd_r2      	= 0;
	bm->fd_r3	  	= 0;
	bm->fd_addr 	= data;

	bm   = &icons[33];
	data += icon_buffer_size;
	bm->fd_w 		= 48;
	bm->fd_h 		= 48;
	bm->fd_wdwidth	= 3;
	bm->fd_stand 	= 0;
	bm->fd_nplanes 	= app.nplanes;
	bm->fd_r1      	= 0;
	bm->fd_r2      	= 0;
	bm->fd_r3	  	= 0;
	bm->fd_addr 	= data;

	bm   = &icons[34];
	data += icon_buffer_size;
	bm->fd_w 		= 48;
	bm->fd_h 		= 48;
	bm->fd_wdwidth	= 3;
	bm->fd_stand 	= 0;
	bm->fd_nplanes 	= app.nplanes;
	bm->fd_r1      	= 0;
	bm->fd_r2      	= 0;
	bm->fd_r3	  	= 0;
	bm->fd_addr 	= data;

	bm   = &icons[35];
	bm->fd_w 		= 48;
	bm->fd_h 		= 48;
	bm->fd_wdwidth	= 3;
	bm->fd_stand 	= 0;
	bm->fd_nplanes 	= app.nplanes;
	bm->fd_r1      	= 0;
	bm->fd_r2      	= 0;
	bm->fd_r3	  	= 0;
	bm->fd_addr 	= icons[3].fd_addr;

	bm   = &icons[36];
	bm->fd_w 		= 48;
	bm->fd_h 		= 48;
	bm->fd_wdwidth	= 3;
	bm->fd_stand 	= 0;
	bm->fd_nplanes 	= app.nplanes;
	bm->fd_r1      	= 0;
	bm->fd_r2      	= 0;
	bm->fd_r3	  	= 0;
	bm->fd_addr 	= icons[25].fd_addr;

	bm   = &icons[37];
	bm->fd_w 		= 48;
	bm->fd_h 		= 48;
	bm->fd_wdwidth	= 3;
	bm->fd_stand 	= 0;
	bm->fd_nplanes 	= app.nplanes;
	bm->fd_r1      	= 0;
	bm->fd_r2      	= 0;
	bm->fd_r3	  	= 0;
	bm->fd_addr 	= icons[3].fd_addr;

	bm   = &icons[38];
	bm->fd_w 		= 48;
	bm->fd_h 		= 48;
	bm->fd_wdwidth	= 3;
	bm->fd_stand 	= 0;
	bm->fd_nplanes 	= app.nplanes;
	bm->fd_r1      	= 0;
	bm->fd_r2      	= 0;
	bm->fd_r3	  	= 0;
	bm->fd_addr 	= icons[3].fd_addr;

	bm   = &icons[39];
	bm->fd_w 		= 48;
	bm->fd_h 		= 48;
	bm->fd_wdwidth	= 3;
	bm->fd_stand 	= 0;
	bm->fd_nplanes 	= app.nplanes;
	bm->fd_r1      	= 0;
	bm->fd_r2      	= 0;
	bm->fd_r3	  	= 0;
	bm->fd_addr 	= icons[12].fd_addr;

	bm   = &icons[40];
	bm->fd_w 		= 48;
	bm->fd_h 		= 48;
	bm->fd_wdwidth	= 3;
	bm->fd_stand 	= 0;
	bm->fd_nplanes 	= app.nplanes;
	bm->fd_r1      	= 0;
	bm->fd_r2      	= 0;
	bm->fd_r3	  	= 0;
	bm->fd_addr 	= icons[12].fd_addr;

	bm   = &icons[41];
	bm->fd_w 		= 48;
	bm->fd_h 		= 48;
	bm->fd_wdwidth	= 3;
	bm->fd_stand 	= 0;
	bm->fd_nplanes 	= app.nplanes;
	bm->fd_r1      	= 0;
	bm->fd_r2      	= 0;
	bm->fd_r3	  	= 0;
	bm->fd_addr 	= icons[5].fd_addr;

	bm   = &icons[42];
	bm->fd_w 		= 48;
	bm->fd_h 		= 48;
	bm->fd_wdwidth	= 3;
	bm->fd_stand 	= 0;
	bm->fd_nplanes 	= app.nplanes;
	bm->fd_r1      	= 0;
	bm->fd_r2      	= 0;
	bm->fd_r3	  	= 0;
	bm->fd_addr 	= icons[5].fd_addr;

	bm   = &icons[43];
	data += icon_buffer_size;
	bm->fd_w 		= 48;
	bm->fd_h 		= 48;
	bm->fd_wdwidth	= 3;
	bm->fd_stand 	= 0;
	bm->fd_nplanes 	= app.nplanes;
	bm->fd_r1      	= 0;
	bm->fd_r2      	= 0;
	bm->fd_r3	  	= 0;
	bm->fd_addr 	= data;

	bm   = &icons[44];
	bm->fd_w 		= 48;
	bm->fd_h 		= 48;
	bm->fd_wdwidth	= 3;
	bm->fd_stand 	= 0;
	bm->fd_nplanes 	= app.nplanes;
	bm->fd_r1      	= 0;
	bm->fd_r2      	= 0;
	bm->fd_r3	  	= 0;
	bm->fd_addr 	= icons[30].fd_addr;

	bm   = &icons[45];
	data += icon_buffer_size;
	bm->fd_w 		= 48;
	bm->fd_h 		= 48;
	bm->fd_wdwidth	= 3;
	bm->fd_stand 	= 0;
	bm->fd_nplanes 	= app.nplanes;
	bm->fd_r1      	= 0;
	bm->fd_r2      	= 0;
	bm->fd_r3	  	= 0;
	bm->fd_addr 	= data;

	bm   = &icons[46];
	data += icon_buffer_size;
	bm->fd_w 		= 48;
	bm->fd_h 		= 48;
	bm->fd_wdwidth	= 3;
	bm->fd_stand 	= 0;
	bm->fd_nplanes 	= app.nplanes;
	bm->fd_r1      	= 0;
	bm->fd_r2      	= 0;
	bm->fd_r3	  	= 0;
	bm->fd_addr 	= data;

	bm   = &icons[47];
	data += icon_buffer_size;
	bm->fd_w 		= 48;
	bm->fd_h 		= 48;
	bm->fd_wdwidth	= 3;
	bm->fd_stand 	= 0;
	bm->fd_nplanes 	= app.nplanes;
	bm->fd_r1      	= 0;
	bm->fd_r2      	= 0;
	bm->fd_r3	  	= 0;
	bm->fd_addr 	= data;

	bm   = &boussole;
	data += icon_buffer_size;
	bm->fd_w 		= 48;
	bm->fd_h 		= 48;
	bm->fd_wdwidth	= 3;
	bm->fd_stand 	= 0;
	bm->fd_nplanes 	= app.nplanes;
	bm->fd_r1      	= 0;
	bm->fd_r2      	= 0;
	bm->fd_r3	  	= 0;
	bm->fd_addr 	= data;

	return 1;
}

void unregister_icons( void)
{
	if( codec_load == 0)
		return;

	delete_img( logo);
	delete_img( weather_icons);
	codecs_quit();
}
