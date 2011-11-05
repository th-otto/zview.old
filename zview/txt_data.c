#include "general.h"
#include "pic_load.h"

/*==================================================================================*
 * delete_txt_data:																	*
 *			free memory reserved in txt_data struct.								*
 *----------------------------------------------------------------------------------*
 * IMAGE *img: 	The IMAGE struct where is the txt_data struct. to handle.			*
 *----------------------------------------------------------------------------------*
 * returns: 	--																	*
 *==================================================================================*/
void delete_txt_data( IMAGE *img)
{
	int16 	i;

	if( !img->comments)
		return;

	for ( i = 0; i < img->comments->lines; i++) 
	{
		if( img->comments->txt[i])
			gfree( img->comments->txt[i]);
	}

	gfree( img->comments);
	img->comments = NULL;
}


/*==================================================================================*
 * init_txt_data:																	*
 *			Initialisation with various shared_malloc of the txt_data struct.		*
 *			This struct. will contain the comment of the picture or the EXIF data.	*
 *----------------------------------------------------------------------------------*
 * txtdata: 			The IMAGE struct where is the txt_dataThe IMAGE struct 		*
 *						where is the txt_data struct. to be initialized.			*
 * lines:				The number of lines in the text.							*
 * max_lines_length:	The line size in character.									*
 *----------------------------------------------------------------------------------*
 * returns: 	0 if error else 1													*
 *==================================================================================*/
int16 init_txt_data( IMAGE *img, int16 lines, int16 max_lines_length)
{
	int16 	i;
	size_t	line_size = max_lines_length * sizeof( int8); 

	img->comments = gmalloc( sizeof( txt_data));

	if( !img->comments)
		return( 0);

	if( lines > 255)
		img->comments->lines	= 255;
	else
		img->comments->lines 	= lines;

	img->comments->max_lines_length 	= max_lines_length;

	for ( i = 0; i < img->comments->lines; i++) 
	{
		img->comments->txt[i] = gmalloc( line_size);

		if( !img->comments->txt[i])
		{
			delete_txt_data( img);
			return( 0);
		}
	}

	return( 1);
}

