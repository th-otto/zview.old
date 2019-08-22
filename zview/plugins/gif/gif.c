#include <stdlib.h>
#include <gif_lib.h>
#include "zview.h"
#include "imginfo.h"
#include "zvgif.h"

#ifdef PLUGIN_SLB
#include "plugin.h"

long __CDECL get_option(zv_int_t which)
{
	switch (which)
	{
	case OPTION_CAPABILITIES:
		return CAN_DECODE;
	case OPTION_EXTENSIONS:
		return (long) ("GIF\0");
	}
	return -ENOSYS;
}
#endif

/*==================================================================================*
 * void decodecolormap:																*
 *		Convert 8 bits + palette data to standard RGB								*
 *----------------------------------------------------------------------------------*
 * input:																			*
 *		src			->	The 8 bits source.											*
 *		dst			->	The destination where put the RGB data.						*
 *		cm			->	The source's colormap.										*
 *		width		->	picture's width.											*
 *----------------------------------------------------------------------------------*
 * return:	 																		*
 *     --																			*
 *==================================================================================*/
static void decodecolormap(uint8_t *src, uint8_t *dst, GifColorType *cm, uint16_t width, int16_t trans_index,
						   uint32_t transparent_color, int16_t draw_trans)
{
	GifColorType *cmentry;
	uint16_t i;

	for (i = 0; i < width; i++)
	{
		if (src[i] == trans_index)
		{
			if (draw_trans == FALSE)
			{
				dst += 3;
				continue;
			}
			*(dst++) = ((transparent_color >> 16) & 0xFF);
			*(dst++) = ((transparent_color >> 8) & 0xFF);
			*(dst++) = ((transparent_color) & 0xFF);
		} else
		{
			cmentry = &cm[src[i]];
			*(dst++) = cmentry->Red;
			*(dst++) = cmentry->Green;
			*(dst++) = cmentry->Blue;
		}
	}
}


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
	static int16_t InterlacedOffset[] = { 0, 4, 2, 1 }, InterlacedJumps[] = { 8, 8, 4, 2 };
	int16_t i, j;
	int16_t error = 0;
	int16_t transpar = -1;
	int16_t interlace = FALSE;
	int16_t draw_trans = TRUE;
	uint16_t delaycount = 0;
	GifFileType *gif;
	GifRecordType rec;
	txt_data comment = { 0, };
	img_data img = { 0, };

	if ((gif = DGifOpenFileName(name, NULL)) == NULL)
		return FALSE;

	img.imagecount = 0;

	do
	{
		if (DGifGetRecordType(gif, &rec) == GIF_ERROR)
		{
			error = 1;
			break;
		}

		if (rec == IMAGE_DESC_RECORD_TYPE)
		{
			if (DGifGetImageDesc(gif) == GIF_ERROR)
			{
				error = 1;
				break;
			} else
			{
				ColorMapObject *map = (gif->Image.ColorMap ? gif->Image.ColorMap : gif->SColorMap);
				int Row, Col, Width, Height;
				int32_t line_size;
				uint8_t *line_buffer = NULL;
				uint8_t *img_buffer;

				if (map == NULL)
				{
					error = 1;
					break;
				}

				Row = gif->Image.Top;		/* Image Position relative to Screen. */
				Col = gif->Image.Left;
				Width = gif->Image.Width;
				Height = gif->Image.Height;

#if 0
				if ( Col + Width > gif->SWidth || Row + Height > gif->SWidth)
				{
					error = 1;
					break;
				}
#endif
				info->components = map->BitsPerPixel == 1 ? 1 : 3;
				info->planes = map->BitsPerPixel;
				info->colors = map->ColorCount;
				info->width = (uint16_t) gif->SWidth;
				info->height = (uint16_t) gif->SHeight;
				line_size = (int32_t) gif->SWidth * (int32_t) info->components;

				if (gif->Image.Interlace)
					interlace = TRUE;

				img.image_buf[img.imagecount] = (uint8_t *) malloc(line_size * (gif->SHeight + 2));
				img_buffer = img.image_buf[img.imagecount];

				if (img_buffer == NULL)
				{
					error = 1;
					break;
				}

				if (map->BitsPerPixel != 1)
				{
					line_buffer = (uint8_t *) malloc(line_size + 128);

					if (line_buffer == NULL)
					{
						free(img.image_buf[img.imagecount]);
						error = 1;
						break;
					}
				}

				if (img.imagecount)
				{
					/* the transparency is the background picture */
					draw_trans = FALSE;

					if ((Row > 0 && Height < gif->SHeight) || (Col > 0 && Width < gif->SWidth))
						memcpy(img_buffer, img.image_buf[img.imagecount - 1], line_size * gif->SHeight);
					else
						draw_trans = TRUE;
				}

				if (interlace)
				{
					for (i = 0; i < 4; i++)
					{
						for (j = Row + InterlacedOffset[i]; j < Row + Height; j += InterlacedJumps[i])
						{
							if (map->BitsPerPixel != 1)
							{
								if (DGifGetLine(gif, line_buffer, Width) != GIF_OK)
								{
									error = 1;
									free(img.image_buf[img.imagecount]);
									break;
								}

								decodecolormap(line_buffer, &img_buffer[(j * line_size) + (Col * 3)], map->Colors,
											   Width, transpar, info->background_color, draw_trans);
							} else if (DGifGetLine(gif, &img_buffer[(j * line_size) + Col], Width) != GIF_OK)
							{
								free(img.image_buf[img.imagecount]);
								error = 1;
								break;
							}
						}
					}
				} else
				{
					for (i = 0; i < Height; i++, Row++)
					{
						if (map->BitsPerPixel != 1)
						{
							if (DGifGetLine(gif, line_buffer, Width) != GIF_OK)
							{
								error = 1;
								free(img.image_buf[img.imagecount]);
								break;
							}

							decodecolormap(line_buffer, &img_buffer[(Row * line_size) + (Col * 3)], map->Colors, Width,
										   transpar, info->background_color, draw_trans);
						} else if (DGifGetLine(gif, &img_buffer[(Row * line_size) + Col], Width) != GIF_OK)
						{
							free(img.image_buf[img.imagecount]);
							error = 1;
							break;
						}
					}
				}

				if (line_buffer)
				{
					free(line_buffer);
					line_buffer = NULL;
				}

				img.imagecount++;
			}

			if (info->thumbnail)
				break;

		} else if (rec == EXTENSION_RECORD_TYPE)
		{
			int code;
			GifByteType *block;

			if (DGifGetExtension(gif, &code, &block) == GIF_ERROR)
			{
				error = 1;
				break;
			} else
			{
				while (block != NULL)
				{
					switch (code)
					{
					case COMMENT_EXT_FUNC_CODE:
						if (comment.lines > 254)
							break;

						block[block[0] + 1] = '\0';	/* Convert gif's pascal-like string */
						comment.txt[comment.lines] = (char *) malloc(block[0] + 1);

						if (comment.txt[comment.lines] == NULL)
							break;

						strcpy(comment.txt[comment.lines], (char *) block + 1);
						comment.max_lines_length =
							MAX(comment.max_lines_length, (int16_t) strlen(comment.txt[comment.lines]) + 1);
						comment.lines++;
						break;

					case GRAPHICS_EXT_FUNC_CODE:
						if (block[1] & 1)
							transpar = (uint16_t) block[4];
						else
							transpar = -1;

						img.delay[delaycount++] = (block[2] + (block[3] << 8)) << 1;

						break;

#if 0
						/*
						   In version 2.0 beta Netscape, introduce a special extention for
						   set a maximum loop playback.. Netscape itself doesn't use it anymore
						   and play the animation infinitly... so we don't care about it.
						 */
					case APPLICATION_EXT_FUNC_CODE:
						if (reading_netscape_ext == 0)
						{
							if (memcmp(block + 1, "NETSCAPE", 8) != 0)
								break;

							if (block[0] != 11)
								break;

							if (memcmp( block + 9, "2.0", 3) != 0)
								break;

							reading_netscape_ext = 1;
						} else
						{
							if ((block[1] & 7) == 1)
								info->max_loop_count = block[2] | ( block[3] << 8);
						}
						break;
#endif
					default:
						break;
					}

					if (DGifGetExtensionNext(gif, &block) == GIF_ERROR)
					{
						error = 1;
						break;
					}
				}
			}
		} else
			break;
	} while (rec != TERMINATE_RECORD_TYPE);


	if (error)
	{
		for (i = 0; i < comment.lines; i++)
		{
			if (comment.txt[i])
				free(comment.txt[i]);
		}

		for (i = 0; i < img.imagecount; i++)
		{
			if (img.image_buf[i])
				free(img.image_buf[i]);
		}

		DGifCloseFile(gif, NULL);
		return FALSE;
	}

	info->real_height = info->height;
	info->real_width = info->width;
	info->orientation = UP_TO_DOWN;
	info->page = img.imagecount;
	info->num_comments = comment.lines;
	info->max_comments_length = comment.max_lines_length;
	info->indexed_color = FALSE;
	info->memory_alloc = TT_RAM;
	info->_priv_var = 0L;				/* page line counter */
	info->_priv_var_more = 0L;			/* current page returned */
	info->_priv_ptr = (void *) &comment;
	info->_priv_ptr_more = (void *) &img;

	strcpy(info->info, "GIF");

#if 0
	if (gif->Version)
		strcat(info->info, "89a"); 
	else
		strcat(info->info, "87a"); 
#endif

	if (interlace)
		strcat(info->info, " (Interlaced)");

	strcpy(info->compression, "LZW");

	DGifCloseFile(gif, NULL);

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
void __CDECL reader_get_txt(IMGINFO info, txt_data * txtdata)
{
	int16_t i;
	txt_data *comment = (txt_data *) info->_priv_ptr;

	for (i = 0; i < txtdata->lines; i++)
		strcpy(txtdata->txt[i], comment->txt[i]);
}


/*==================================================================================*
 * boolean __CDECL reader_read:														*
 *		This function fits the buffer with image data								*
 *----------------------------------------------------------------------------------*
 * input:																			*
 *		buffer		->	The destination buffer.										*
 *		info		->	The IMGINFO struct.											*
 *----------------------------------------------------------------------------------*
 * return:	 																		*
 *      TRUE if all ok else FALSE.													*
 *==================================================================================*/
boolean __CDECL reader_read(IMGINFO info, uint8_t * buffer)
{
	img_data *img = (img_data *) info->_priv_ptr_more;
	int32_t line_size = (int32_t) info->width * (int32_t) info->components;
	uint8_t *line_src;

	if ((int32_t) info->page_wanted != info->_priv_var_more)
	{
		info->_priv_var_more = (int32_t) info->page_wanted;
		info->_priv_var = 0;
	}

	line_src = &img->image_buf[info->page_wanted][info->_priv_var];

	info->_priv_var += line_size;
	info->delay = img->delay[info->page_wanted];

	memcpy(buffer, line_src, line_size);

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
	txt_data *comment = (txt_data *) info->_priv_ptr;
	img_data *img = (img_data *) info->_priv_ptr_more;
	int16_t i;

	for (i = 0; i < comment->lines; i++)
	{
		if (comment->txt[i])
			free(comment->txt[i]);
	}

	for (i = 0; i < img->imagecount; i++)
	{
		if (img->image_buf[i])
			free(img->image_buf[i]);
	}
}
