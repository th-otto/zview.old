#include "plugin.h"
#include "zvplugin.h"

#define VERSION  0x200
#define NAME     "Dr. Doodle"
#define AUTHOR   "Lonny Pursell, Thorsten Otto"
#define DATE     __DATE__ " " __TIME__
#define MISCINFO "Parts of code taken from St2BMP,\n" \
                 "written by Hans Wessels"

static uint16_t const medpal[4] = {
	0x0fff, 0x0f00, 0x00f0, 0x0000
};
static uint16_t const lowpal[16] = {
	0x0fff, 0x0f00, 0x00f0, 0x0ff0,
	0x000f, 0x0f0f, 0x00ff, 0x0555,
	0x0333, 0x0f33, 0x03f3, 0x0ff3,
	0x033f, 0x0f3f, 0x03ff, 0x0000
};

#define DOODLE_FILESIZE 32000


#ifdef PLUGIN_SLB
long __CDECL get_option(zv_int_t which)
{
	switch (which)
	{
	case OPTION_CAPABILITIES:
		return CAN_DECODE;
	case OPTION_EXTENSIONS:
		return (long)("DOO\0");
	case INFO_NAME:
		return (long)NAME;
	case INFO_VERSION:
		return VERSION;
	case INFO_DATETIME:
		return (long)DATE;
	case INFO_AUTHOR:
		return (long)AUTHOR;
	case INFO_MISC:
		return (long)MISCINFO;
	}
	return -ENOSYS;
}
#endif


static long filesize(int16_t fd)
{
	long size;
	
	size = Fseek(0, fd, SEEK_END);
	Fseek(0, fd, SEEK_SET);
	return size;
}


/* code from atarist.c, app code St2BMP (placed into public domain) */
/* written by Hans Wessels */

/*
 * guess resolution from picturedata, 0=low, 1=med, 2=high
 *
 * routine guesses resolution by converting the bitplane pixels
 * to chunky pixels in a byte, just enough to fil a byte (8 
 * pixels for high, 4 for medium and 2 for lowres). And count
 * the number of runlength bytes in each mode. The correct
 * resolution is the one having the most runlength bytes.
 * If there is a draw, mono wins, then low res, medium res is
 * never chosen in a draw.
 */
static uint8_t guess_resolution(const uint8_t *buffer)
{
	const uint8_t *scan;
	int16_t reslow;
	int16_t resmed;
	int16_t resmono;
	int16_t i;
	int16_t k;
	uint8_t last_byte;
	uint16_t plane0;
	uint16_t plane1;
	uint16_t plane2;
	uint16_t plane3;
	uint16_t mask;
	uint8_t byte;
	
	reslow = 0;
	resmed = 0;
	resmono = 0;

	/* test high */
	last_byte = 0;
	for (scan = buffer, i = 0; i < DOODLE_FILESIZE; i++)
	{
		byte = *scan++;
		if (byte == last_byte)
			resmono++;
		else
			last_byte = byte;
	}

	/* test_med */
	last_byte = 0;
	for (scan = buffer, i = 0; i < DOODLE_FILESIZE / 4; i++)
	{
		plane0 = *scan++;
		mask = 0x8000;
		plane0 <<= 8;
		plane0 += *scan++;
		plane1 = *scan++;
		plane1 <<= 8;
		plane1 += *scan++;
		do {
			byte = 0;
			for (k = 0; k < 4; k++)
			{
				byte += byte;
				if (plane0 & mask)
					byte++;
				byte += byte;
				if (plane1 & mask)
					byte++;
				mask >>= 1;
			}
			if (byte == last_byte)
				resmed++;
			else
				last_byte = byte;
		} while (mask != 0);
	}
	
	/* test_low */
	last_byte = 0;
	for (scan = buffer, i = 0; i < DOODLE_FILESIZE / 8; i++)
	{
		plane0 = *scan++;
		mask = 0x8000;
		plane0 <<= 8;
		plane0 += *scan++;
		plane1 = *scan++;
		plane1 <<= 8;
		plane1 += *scan++;
		plane2 = *scan++;
		plane2 <<= 8;
		plane2 += *scan++;
		plane3 = *scan++;
		plane3 <<= 8;
		plane3 += *scan++;
		do {
			byte = 0;
			for (k = 0; k < 2; k++)
			{
				byte += byte;
				if (plane0 & mask)
					byte++;
				byte += byte;
				if (plane1 & mask)
					byte++;
				byte += byte;
				if (plane2 & mask)
					byte++;
				byte += byte;
				if (plane3 & mask)
					byte++;
				mask >>= 1;
			}
			if (byte == last_byte)
				reslow++;
			else
				last_byte = byte;
		} while (mask != 0);
	}

	if (resmono >= reslow && resmono >= resmed)
		return 2;
	if (resmed > reslow)
		return 1;
	return 0;
}


static void setpal(IMGINFO info, int16_t colors, const uint16_t *palette)
{
	int16_t i;

	for (i = 0; i < colors; i++)
	{
		uint16_t c;
		
		c = ((palette[i] >> 7) & 0x0e) + ((palette[i] >> 11) & 0x01);
		info->palette[i].red = (c << 4) | c;
		c = ((palette[i] >> 3) & 0x0e) + ((palette[i] >> 7) & 0x01);
		info->palette[i].green = (c << 4) | c;
		c = (((palette[i]) << 1) & 0x0e) + ((palette[i] >> 3) & 0x01);
		info->palette[i].blue = (c << 4) | c;
	}
}


boolean __CDECL reader_init(const char *name, IMGINFO info)
{
	uint8_t resolution;
	long shift;
	int16_t handle;
	uint8_t *file_buffer;
	
	info->_priv_ptr = NULL;
	handle = (int16_t) Fopen(name, 0);
	if (handle < 0)
		return FALSE;
	
	if (filesize(handle) != DOODLE_FILESIZE)
	{
		Fclose(handle);
		return FALSE;
	}
	
	file_buffer = malloc(DOODLE_FILESIZE);
	if (file_buffer == NULL)
	{
		Fclose(handle);
		return FALSE;
	}
	Fread(handle, DOODLE_FILESIZE, file_buffer);
	Fclose(handle);
	
	resolution = guess_resolution(file_buffer);
	shift = Kbshift(-1);
	if (shift & K_ALT)
		resolution = 0;
	else if (shift & K_CTRL)
		resolution = 1;
	else if (shift & K_LSHIFT)
		resolution = 2;
	
	info->planes = 4 >> resolution;
	info->colors = 1L << info->planes;
	switch (info->planes)
	{
	case 4:
		info->width = 320;
		info->height = 200;
		info->indexed_color = TRUE;
		setpal(info, 16, lowpal);
		break;
	case 2:
		info->width = 640;
		info->height = 200;
		info->indexed_color = TRUE;
		setpal(info, 4, medpal);
		break;
	case 1:
		info->width = 640;
		info->height = 400;
		info->indexed_color = FALSE;
		break;
	}
	
	info->components = info->planes == 1 ? 1 : 3;
	info->real_width = info->width;
	info->real_height = info->height;
	info->memory_alloc = TT_RAM;
	info->page = 1;
	info->orientation = UP_TO_DOWN;
	info->num_comments = 0;
	info->_priv_var = 0;
	info->_priv_ptr = file_buffer;
	info->_priv_ptr_more = file_buffer;
	strcpy(info->info, "Dr. Doodle");
	strcpy(info->compression, "None");
	
	return TRUE;
}


boolean __CDECL reader_read(IMGINFO info, uint8_t *buffer)
{
	int16_t i;
	int16_t x;
	uint16_t byte;
	int16_t plane;
	
	switch (info->planes)
	{
	case 1:
		{
			uint8_t *src;

			src = (uint8_t *)info->_priv_ptr_more;
			x = info->width >> 3;
			do
			{
				byte = *src++;
				for (i = 7; i >= 0; i--)
				{
					*buffer++ = (byte >> i) & 1;
				}
			} while (--x > 0);
			info->_priv_ptr_more = src;
		}
		break;
	default:
		{
			uint16_t *src;

			src = (uint16_t *)info->_priv_ptr_more;
			x = info->width >> 4;
			do
			{
				for (i = 15; i >= 0; i--)
				{
					for (plane = byte = 0; plane < info->planes; plane++)
					{
						if ((src[plane] >> i) & 1)
							byte |= 1 << plane;
					}
					*buffer++ = byte;
				}
				src += info->planes;
			} while (--x > 0);
			info->_priv_ptr_more = src;
		}
		break;
	}
	return TRUE;
}


void __CDECL reader_quit(IMGINFO info)
{
	free(info->_priv_ptr);
	info->_priv_ptr = NULL;
}


void __CDECL reader_get_txt(IMGINFO info, txt_data *txtdata)
{
	(void)info;
	(void)txtdata;
}
