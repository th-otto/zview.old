#include "plugin.h"
#include "zvplugin.h"
#include <png.h>

#define VERSION 0x200
#define AUTHOR "Zorro"

#ifdef PLUGIN_SLB
long __CDECL get_option(zv_int_t which)
{
	switch (which)
	{
	case OPTION_CAPABILITIES:
		return CAN_DECODE;
	case OPTION_EXTENSIONS:
		return (long) ("PNG\0");
	}
	return -ENOSYS;
}
#endif

#define alpha_composite( composite, fg, alpha, bg) {									\
    uint16_t temp = (( uint16_t)( fg) * ( uint16_t)( alpha) +						\
                   ( uint16_t)( bg) * ( uint16_t)(255 - ( uint16_t)( alpha)) + ( uint16_t)128);	\
    ( composite) = ( uint8_t)(( temp + ( temp >> 8)) >> 8);								\
}

static boolean first_pass;
static uint8_t *line_buffer;
static uint8_t *png_image1;
static uint8_t *png_image_ptr;
static int16_t number_passes;


/*==================================================================================*
 * boolean __CDECL reader_init:														*
 *		Open the file "name", fit the "info" struct. ( see zview.h) and make others	*
 *		things needed by the decoder.												*
 *----------------------------------------------------------------------------------*
 * input:																			*
 *		name		->	The file to open.											*
 *		info		->	The IMGINFO struct. to fit.									*
 *----------------------------------------------------------------------------------*
 * return:	 																		*
 *      TRUE if all ok else FALSE.													*
 *==================================================================================*/
boolean __CDECL reader_init(const char *name, IMGINFO info)
{
	png_structp png_ptr = NULL;
	png_infop info_ptr = NULL;
	FILE *png_file;
	char header[8];
	int header_size = sizeof(header);
#if 0
	png_color_16 my_background = { 0,0xFFFF,0xFFFF,0xFFFF,0xFFFF };
    png_color_16p image_background;
#endif
	png_textp png_text_ptr;
	int num_text;

	first_pass = TRUE;
	png_image1 = NULL;
	line_buffer = NULL;
	number_passes = 0;

	if ((png_file = fopen(name, "rb")) == NULL)
		return FALSE;

	if (fread(header, 1, header_size, png_file) != header_size || png_sig_cmp(header, 0, header_size))
	{
		fclose(png_file);
		return FALSE;
	}

	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, 0, 0);

	if (!png_ptr || (info_ptr = png_create_info_struct(png_ptr)) == NULL)
	{
		png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
		fclose(png_file);
		return FALSE;
	}

	if (setjmp(png_jmpbuf(png_ptr)))
	{
		png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
		fclose(png_file);
		return FALSE;
	}

	png_init_io(png_ptr, png_file);
	png_set_sig_bytes(png_ptr, (int) header_size);
	png_read_info(png_ptr, info_ptr);

	if (png_get_color_type(png_ptr, info_ptr) == PNG_COLOR_TYPE_PALETTE)
		png_set_expand(png_ptr);
	if (png_get_color_type(png_ptr, info_ptr) == PNG_COLOR_TYPE_GRAY && png_get_bit_depth(png_ptr, info_ptr) < 8)
		png_set_expand(png_ptr);
	if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
		png_set_tRNS_to_alpha(png_ptr);
	if (png_get_bit_depth(png_ptr, info_ptr) == 16)
		png_set_strip_16(png_ptr);
	if (png_get_color_type(png_ptr, info_ptr) == PNG_COLOR_TYPE_GRAY ||
		png_get_color_type(png_ptr, info_ptr) == PNG_COLOR_TYPE_GRAY_ALPHA)
		png_set_gray_to_rgb(png_ptr);

	strcpy(info->info, "Portable Network Format ");
	strcpy(info->compression, "ZIP");

	if (png_get_interlace_type(png_ptr, info_ptr) == PNG_INTERLACE_ADAM7)
	{
		strcat(info->info, "( Interlaced)");
		number_passes = png_set_interlace_handling(png_ptr);
	}

	png_read_update_info(png_ptr, info_ptr);

	png_get_text(png_ptr, info_ptr, &png_text_ptr, &num_text);

	info->width = png_get_image_width(png_ptr, info_ptr);
	info->height = png_get_image_height(png_ptr, info_ptr);
	info->real_width = info->width;
	info->real_height = info->height;
	info->memory_alloc = TT_RAM;
	info->components = png_get_channels(png_ptr, info_ptr) > 3 ? 3 : png_get_channels(png_ptr, info_ptr);
	info->planes = png_get_bit_depth(png_ptr, info_ptr);
	info->colors = 1L << info->planes;
	info->delay = 0;
	info->orientation = UP_TO_DOWN;
	info->page = 1;
	info->num_comments = num_text;
	info->max_comments_length = 0;
	info->indexed_color = FALSE;
	info->_priv_ptr = (void *) png_ptr;
	info->_priv_ptr_more = (void *) info_ptr;
	info->__priv_ptr_more = png_file;

	if (png_get_channels(png_ptr, info_ptr) == 4)
	{
		line_buffer = (uint8_t *) malloc(png_get_rowbytes(png_ptr, info_ptr) + 64);

		if (line_buffer == NULL)
		{
			reader_quit(info);
			return FALSE;
		}
	}

	if (num_text)
	{
		int16_t i;

		for (i = 0; i < num_text; i++)
			info->max_comments_length =
				MAX(info->max_comments_length, (strlen(png_text_ptr[i].key) + strlen(png_text_ptr[i].text) + 3));
	}

	return TRUE;
}


/*==================================================================================*
 * boolean __CDECL reader_get_txt													*
 *		This function , like other function mus be always present.					*
 *		It fills txtdata struct. with the text present in the picture ( if any).	*
 *----------------------------------------------------------------------------------*
 * input:																			*
 *		txtdata		->	The destination text buffer.								*
 *		info		->	The IMGINFO struct. to fit.									*
 *----------------------------------------------------------------------------------*
 * return:	 																		*
 *      --																			*
 *==================================================================================*/
void __CDECL reader_get_txt(IMGINFO info, txt_data *txtdata)
{
	int16_t i;
	png_infop info_ptr = (png_infop) info->_priv_ptr_more;
	png_structp png_ptr = (png_structp) info->_priv_ptr;
	png_textp png_text_ptr;
	int num_text;

	png_get_text(png_ptr, info_ptr, &png_text_ptr, &num_text);

	for (i = 0; i < txtdata->lines; i++)
		sprintf(txtdata->txt[i], "%s: %s", png_text_ptr[i].key, png_text_ptr[i].text);
}


/*==================================================================================*
 * boolean __CDECL reader_read:														*
 *		This function fits the buffer with image data								*
 *----------------------------------------------------------------------------------*
 * input:																			*
 *		buffer		->	The destination buffer.										*
 *		info		->	The IMGINFO struct. to fit.									*
 *----------------------------------------------------------------------------------*
 * return:	 																		*
 *      TRUE if all ok else FALSE.													*
 *==================================================================================*/
boolean __CDECL reader_read(IMGINFO info, uint8_t *buffer)
{
	png_structp png_ptr = (png_structp) info->_priv_ptr;
	png_infop info_ptr = (png_infop) info->_priv_ptr_more;
	uint8_t *buf;
	int16_t i;

	if (png_get_channels(png_ptr, info_ptr) == 4)
		buf = line_buffer;
	else
		buf = buffer;

	if (setjmp(png_jmpbuf(png_ptr)))
		return FALSE;

	if (png_get_interlace_type(png_ptr, info_ptr) == PNG_INTERLACE_ADAM7)
	{
		/* We make the first pass here and not before to avoid memory fragmentation */
		if (first_pass == TRUE)
		{
			int16_t pass;

			png_image1 = (uint8_t *) malloc(png_get_rowbytes(png_ptr, info_ptr) * (info->height + 1));

			png_image_ptr = png_image1;

			for (pass = 1; pass < number_passes; pass++)
			{
				for (i = 0; i < info->height; i++)
				{
					png_bytep row = png_image1 + i * png_get_rowbytes(png_ptr, info_ptr);

					png_read_row(png_ptr, row, NULL);
				}
			}
			first_pass = FALSE;
		}

		png_read_row(png_ptr, png_image_ptr, NULL);
		memcpy(buf, png_image_ptr, png_get_rowbytes(png_ptr, info_ptr));
		png_image_ptr += png_get_rowbytes(png_ptr, info_ptr);
	} else
	{
		png_read_row(png_ptr, buf, NULL);
	}

	if (png_get_channels(png_ptr, info_ptr) == 4)
	{
		uint8_t red, green, blue;
		uint8_t r, g, b, a;
		uint8_t *dest = buffer;
		uint8_t *buf_ptr = buf;

		for (i = info->width; i > 0; --i)
		{
			r = *buf_ptr++;
			g = *buf_ptr++;
			b = *buf_ptr++;
			a = *buf_ptr++;

			if (a == 255)
			{
				red = r;
				green = g;
				blue = b;
			} else if (a == 0)
			{
				red = 0xFF;
				green = 0xFF;
				blue = 0xFF;
			} else
			{
				uint32_t transparent_color = info->background_color;

				alpha_composite(red, r, a, (transparent_color >> 16) & 0xFF);
				alpha_composite(green, g, a, (transparent_color >> 8) & 0xFF);
				alpha_composite(blue, b, a, (transparent_color) & 0xFF);
			}

			*dest++ = red;
			*dest++ = green;
			*dest++ = blue;
		}
	}

	return TRUE;
}

/*==================================================================================*
 * boolean __CDECL reader_quit:														*
 *		This function makes the last job like close the file handler and free the	*
 *		allocated memory.															*
 *----------------------------------------------------------------------------------*
 * input:																			*
 *		info		->	The IMGINFO struct. to fit.									*
 *----------------------------------------------------------------------------------*
 * return:	 																		*
 *      --																			*
 *==================================================================================*/
void __CDECL reader_quit(IMGINFO info)
{
	png_structp png_ptr = (png_structp) info->_priv_ptr;
	png_infop info_ptr = (png_infop) info->_priv_ptr_more;

	if (png_ptr)
	{
		if (png_image1)
			free(png_image1);

		if (line_buffer)
			free(line_buffer);

		if (!setjmp(png_jmpbuf(png_ptr)))
			png_read_end(png_ptr, info_ptr);

		png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
		fclose(info->__priv_ptr_more);
		info->_priv_ptr = NULL;
		info->_priv_ptr_more = NULL;
		info->__priv_ptr_more = NULL;
	}
}
