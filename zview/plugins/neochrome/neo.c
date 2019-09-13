/*
  1.00 initial release, medium and high res dropped (invalid modes for neochrome)
       these are incorrectly made files or nvision files with wrong file ext
  1.01 fixed missing fclose(), added 'nvision' info, added 7800 devsys support
  1.02 added nechrome master (raster) support
  1.03 firebee fixes (patched purec library)
  1.04 renamed codec neo.ldg -> neoimg.ldg to make it more obvious
       new codec for neochrome animations -> neoani.ldg
  1.05 made arrays global, better error handling
  1.06 added salert support
  1.07 smaller startup module
  1.08 Code cleanup, optimizations
*/

#define	VERSION		0x0108
#define NAME        "NEOchrome"
#define AUTHOR      "Zorro, Lonny Pursell"
#define DATE     __DATE__ " " __TIME__

#include <limits.h>
#include "plugin.h"
#include "zvplugin.h"

#ifdef PLUGIN_SLB
long __CDECL get_option(zv_int_t which)
{
	switch (which)
	{
	case OPTION_CAPABILITIES:
		return CAN_DECODE;
	case OPTION_EXTENSIONS:
		return (long) ("NEO\0");

	case INFO_NAME:
		return (long)NAME;
	case INFO_VERSION:
		return VERSION;
	case INFO_DATETIME:
		return (long)DATE;
	case INFO_AUTHOR:
		return (long)AUTHOR;
	}
	return -ENOSYS;
}
#endif

#define	NEO_SIZE	32128L
#define	PAL_SIZE	6800L
#define	DEVSYS_SIZE	128128L
#define DEVSYS_ID	0xBABE

#define fill4B(a)	( ( 4 - ( (a) % 4 ) ) & 0x03)


typedef struct
{
	int16_t flags;						/* Always 0                                     */
	int16_t resolution;					/* 0 = low, 1 = medium, 3 = high                */
	uint16_t palette[16];				/* Picture palette in Atari ST hardware format  */
	int8_t filename[12];				/* The original file name                       */
	int16_t color_anim_limit;			/* color animation limit                        */
	int16_t color_anim_speed;			/* color animation speed and direction          */
	int16_t color_steps;				/* numbre of color step                         */
	int16_t x_offset;					/* image offset                                 */
	int16_t y_offset;					/* "                                            */
	int16_t width;
	int16_t height;
	int16_t reserved[33];
} NEOHDR;

#define bail(msg)

#include "lof.c"

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
	int16_t handle;
	NEOHDR hdr;
	uint16_t *bmap;
	uint16_t *pal = NULL;
	uint32_t bms;
	uint32_t filelength;
	char pname[PATH_MAX];						/* path to palette file */
	size_t len;
	uint8_t pf[200];
	int16_t y, i;
	uint16_t cp[16];

	if ((handle = (int16_t) Fopen(name, 0)) < 0)
		return FALSE;

	filelength = lof(handle);
	if (filelength != NEO_SIZE && filelength != DEVSYS_SIZE)
	{
		bail("Invalid file size");
		Fclose(handle);
		return FALSE;
	}

	if (Fread(handle, sizeof(hdr), &hdr) != sizeof(hdr))
	{
		Fclose(handle);
		return FALSE;
	}

	if (hdr.resolution != 0)
	{
		bail("Invalid resolution flag");
		Fclose(handle);
		return FALSE;
	}
	if (hdr.flags == 0)
	{
		info->width = 320;
		info->height = 200;
		bms = 32000L;
	} else if ((uint16_t) hdr.flags == DEVSYS_ID)
	{
		info->width = 640;
		info->height = 400;
		bms = 128000L;
	} else
	{
		bail("Invalid header flags");
		Fclose(handle);
		return FALSE;
	}

	bmap = malloc(bms);
	if (bmap == NULL)
	{
		bail("Malloc(bitmap) failed");
		return FALSE;
	}
	if (Fread(handle, bms, bmap) != bms)
	{
		free(bmap);
		Fclose(handle);
		return FALSE;
	}
	Fclose(handle);

	/* beyond this point, handle neo *.rst files */
	/* do not change, this method handles unsorted rast blocks */
	/* code copied from my iff loader */
	strcpy(pname, name);
	len = strlen(pname);
	pname[len - 3] = 'r';
	pname[len - 2] = 's';
	pname[len - 1] = 't';

	info->planes = 4;
	info->indexed_color = TRUE;

	handle = (int16_t) Fopen(pname, FO_READ);
	if (handle >= 0)
	{
		if (lof(handle) == PAL_SIZE)
		{
			pal = malloc(200L * 32L);
			if (pal == NULL)
			{
				bail("Malloc(palette) failed");
				free(bmap);
				Fclose(handle);
				return FALSE;
			}
			memset(pf, 0, sizeof(pf));
			for (i = 0; i < 200; i++)
			{
				Fread(handle, 2L, &y);
				if (y >= 0 && y < 200)
				{
					if (pf[y])
					{
						Fseek(32L, handle, SEEK_CUR);	/* ignore it */
					} else 
					{
						pf[y] = 1;
						Fread(handle, 32L, &pal[y * 16]);
					}
				}
			}
			for (i = 0; i < 200; i++)
			{
				if (pf[i])
				{
					memcpy(cp, &pal[i * 16], 32L);
				} else
				{
					memcpy(&pal[i * 16], cp, 32L);
				}
			}
			info->planes = 24;
			info->indexed_color = FALSE;
		}
		Fclose(handle);
	}

	info->components = 3;
	info->colors = 1L << MIN(info->planes, 24);
	info->real_width = info->width;
	info->real_height = info->height;
	info->memory_alloc = TT_RAM;
	info->page = 1;
	info->delay = 0;
	info->orientation = UP_TO_DOWN;
	info->num_comments = 0;
	info->max_comments_length = 0;
	info->_priv_ptr = (void *) bmap;
	info->_priv_ptr_more = (void *)pal;
	info->_priv_var = 0;				/* y pos */
	info->_priv_var_more = 0;			/* pal y */

	strcpy(info->info, "NEOchrome");
	if (info->planes == 24)
	{
		strcat(info->info, " Master (Raster)");
	}
	if (hdr.reserved[0] == 0x4101)
	{
		strcat(info->info, " (Nvision)");
	}
	if (info->width == 640)
	{
		strcat(info->info, " (7800 DevSys)");
	}
	strcpy(info->compression, "None");

	if (info->indexed_color)
	{
		for (i = 0; i < info->colors; i++)
		{
			uint16_t c;
			
			c = (((hdr.palette[i] >> 7) & 0xE) + ((hdr.palette[i] >> 11) & 0x1));
			info->palette[i].red = (c << 4) | c;
			c = (((hdr.palette[i] >> 3) & 0xE) + ((hdr.palette[i] >> 7) & 0x1));
			info->palette[i].green = (c << 4) | c;
			c = (((hdr.palette[i] << 1) & 0xE) + ((hdr.palette[i] >> 3) & 0x1));
			info->palette[i].blue = (c << 4) | c;
		}
	}

	return TRUE;
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
	uint16_t *bmap = (uint16_t *)info->_priv_ptr;
	uint16_t *s = bmap + info->_priv_var;
	uint16_t x, c, p0, p1, p2, p3;

	switch (info->planes)
	{
	case 4:
		x = info->width >> 4;
		info->_priv_var += x << 2;
		do
		{
			p0 = *s++;
			p1 = *s++;
			p2 = *s++;
			p3 = *s++;

			for (c = 0; c < 16; c++)
			{
				*buffer++ = ((p0 >> 15) & 1) | ((p1 >> 14) & 2) | ((p2 >> 13) & 4) | ((p3 >> 12) & 8);
				p0 <<= 1;
				p1 <<= 1;
				p2 <<= 1;
				p3 <<= 1;
			}
		} while (--x > 0);
		break;

	case 2:
		x = info->width >> 4;
		info->_priv_var += x << 1;
		do
		{
			p0 = *s++;
			p1 = *s++;

			for (c = 0; c < 16; c++)
			{
				*buffer++ = ((p0 >> 15) & 1) | ((p1 >> 14) & 2);
				p0 <<= 1;
				p1 <<= 1;
			}
		} while (--x > 0);
		break;

	case 1:
		x = info->width >> 4;
		info->_priv_var += x;
		do
		{
			p0 = *s++;

			for (c = 0; c < 16; c++)
			{
				*buffer++ = ((p0 >> 15) & 1);
				p0 <<= 1;
			}
		} while (--x > 0);
		break;
	
	case 24:
		{
			int16_t bit;
			int16_t ndx;
			int16_t pln;
			uint16_t rgb;
			uint16_t i = 0;;
			uint16_t *pal = (int16_t *)info->_priv_ptr_more;
			
			x = 0;
			do
			{								/* 1-bit to 8-bit atari st word interleaved bitmap v1.01 */
				for (bit = 15; bit >= 0; bit--)
				{
					ndx = 0;
					for (pln = 0; pln < 4; pln++)
					{
						if ((bmap[info->_priv_var + pln] >> bit) & 1)
						{
							ndx = ndx | (1 << pln);
						}
					}
					rgb = pal[(info->_priv_var_more << 4) + ndx];
					buffer[i++] = (((rgb >> 7) & 0xE) + ((rgb >> 11) & 0x1)) * 17;
					buffer[i++] = (((rgb >> 3) & 0xE) + ((rgb >> 7) & 0x1)) * 17;
					buffer[i++] = (((rgb << 1) & 0xE) + ((rgb >> 3) & 0x1)) * 17;
					x++;
				}
				info->_priv_var = info->_priv_var + 4L;
			} while (x < info->width);
			info->_priv_var_more++;
		}
		break;
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
	(void) info;
	(void) txtdata;
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
	free(info->_priv_ptr);
	info->_priv_ptr = NULL;
	free(info->_priv_ptr_more);
	info->_priv_ptr_more = NULL;
}
