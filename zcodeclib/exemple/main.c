#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <osbind.h>
#include <mintbind.h>
#include <gem.h>
#include <ldg.h>
#include <types2b.h>
#include <zcodec.h>


int main( int argc, char *argv[])
{
	IMAGE	*test_img = NULL;
	MFDB	*resize = NULL, screen	 = {0};
	int16	xy[8];

	/* We load the wanted codecs but if we want to
	load the entire codecs's pack, we can do "codecs_init( "all"); */
	if( !codecs_init( "gif.ldg"))
		Cconws("can't init the codec \n\r");

 	if ( argc > 1)
	{ /* we load the dragged picture, we put - 1 to both w and h options because we don't want zoom */
		if( ! (test_img = load_img( argv[argc - 1], -1, -1, 0)))
		{	
			Cconws("can't load image \n\r");
			codecs_quit();
			return 0;
		}
	}
	else
	{	
		Cconws("Drag and Drop a gif image\n\r");
		codecs_quit();
		return 0;
	}

	xy[0] = 0;
	xy[1] = 0;
	xy[2] = test_img->image[0].fd_w - 1;
	xy[3] = test_img->image[0].fd_h - 1;
	xy[4] = 20;
	xy[5] = 20;
	xy[6] = xy[4] + xy[2];
	xy[7] = xy[5] + xy[3];

	if ( test_img->image[0].fd_nplanes == 1)
	{
		int16	color[2] = { 0, 1};

		vrt_cpyfm( vdi_handle, MD_REPLACE, xy, &test_img->image[0], &screen, color);
	}
	else
		vro_cpyfm( vdi_handle, S_ONLY, xy, &test_img->image[0], &screen);

	if( !( resize = pic_resize( &test_img->image[0],64, 64, 1)))
	{	
		Cconws("can't resize image \n\r");
		delete_img( test_img);
		codecs_quit();
		return 0;
	}

	xy[0] = 0;
	xy[1] = 0;
	xy[2] = resize->fd_w - 1;
	xy[3] = resize->fd_h - 1;
	xy[4] = 20 + test_img->image[0].fd_w - 1;
	xy[5] = 20;
	xy[6] = xy[4] + xy[2];
	xy[7] = xy[5] + xy[3];

	/* draw the image */
	if ( resize->fd_nplanes == 1)
	{
		int16	color[2] = { 0, 1};

		vrt_cpyfm( vdi_handle, MD_REPLACE, xy, resize, &screen, color);
	}
	else
		vro_cpyfm( vdi_handle, S_ONLY, xy, resize, &screen);	


	delete_mfdb( resize, 1);
	delete_img( test_img);
	codecs_quit();
	return 0;
}
