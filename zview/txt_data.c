#include "general.h"
#include "pic_load.h"
#include "txt_data.h"

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
		free( img->comments->txt[i]);
	}

	free( img->comments);
	img->comments = NULL;
}


/*==================================================================================*
 * init_txt_data:																	*
 *			Initialisation with various malloc of the txt_data struct.		*
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
	size_t	line_size;

	img->comments = calloc(1, sizeof( txt_data));

	if( !img->comments)
		return FALSE;
	if (lines == 0 || max_lines_length == 0)
		return TRUE;
	line_size = max_lines_length + 1;

	if( lines > MAX_TXT_DATA)
		lines	= MAX_TXT_DATA;
	img->comments->lines 	= lines;

	img->comments->max_lines_length 	= max_lines_length;

	for ( i = 0; i < lines; i++) 
	{
		img->comments->txt[i] = malloc( line_size);

		if( !img->comments->txt[i])
		{
			img->comments->lines = i;
			delete_txt_data( img);
			return( 0);
		}
	}

	return TRUE;
}
