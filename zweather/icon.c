#include "general.h"

IMAGE *main_icon = NULL;
IMAGE *boussole = NULL;
IMAGE *logo = NULL;
IMAGE *forecast_icon[5] = { NULL };
IMAGE *icons[48] = { NULL };

int16 codec_load = 0;

int16 register_icons( void)
{
	int16 i, end = 0;
	int8  icon_name[16];

	if( !( codec_load = codecs_init( "png.ldg")))
		goto error;

	for( i = 0; i <= 47; i++) 
	{
		sprintf( icon_name, "icons\\%d.png", i);

		if( ! ( icons[i] = load_img( icon_name, -1, -1, 1)))
			goto error;
	}
	
	if( !( boussole = load_img( "icons\\boussole.png", -1, -1, 1)))
		goto error;

	if( !( logo = load_img( "icons\\logo.png", -1, -1, 1)))
		goto error;

	end = 1;
	
	error:
		return end;
}

void unregister_icons( void)
{
	int16 i;

	if( codec_load == 0)
		return;

	codecs_quit();

	delete_img( logo);

	delete_img( boussole);

	for( i = 0; i <= 47; i++) 
	{
		delete_img( icons[i]);
	}
}
