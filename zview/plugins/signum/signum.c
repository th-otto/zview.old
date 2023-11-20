#include "plugin.h"
#include "zvplugin.h"

#define VERSION  0x100
#define NAME     "Signum! (Image)"
#define AUTHOR   "Lonny Pursell, Thorsten Otto"
#define DATE     __DATE__ " " __TIME__

#define SCREEN_SIZE 32000

struct ImcHeader {
	uint32_t size;
	uint16_t width;
	uint16_t height;
	uint16_t hchunks;
	uint16_t vchunks;
	uint32_t data_offset;
	uint32_t size_of_data;
	uint16_t s14;
	char u4[16];
	char u5[32];
	char u6[32];
};

extern void __CDECL depack_imc(uint8_t *file_buffer, uint8_t *screen_buffer);

#ifdef PLUGIN_SLB
long __CDECL get_option(zv_int_t which)
{
	switch (which)
	{
	case OPTION_CAPABILITIES:
		return CAN_DECODE;
	case OPTION_EXTENSIONS:
		return (long)("IMC\0");

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


static long filesize(int16_t fd)
{
	long size;
	
	size = Fseek(0, fd, SEEK_END);
	Fseek(0, fd, SEEK_SET);
	return size;
}


boolean __CDECL reader_init(const char *name, IMGINFO info)
{
	int16_t handle;
	uint8_t *file_buffer;
	uint8_t *screen_buffer;
	char imc_tag[8];
	long fsize;
	
	info->_priv_ptr = NULL;
	handle = (int16_t) Fopen(name, 0);
	if (handle < 0)
		return FALSE;

	fsize = filesize(handle) - sizeof(imc_tag);
	
	Fread(handle, sizeof(imc_tag), imc_tag);
	if (memcmp(imc_tag, "bimc0002", sizeof(imc_tag)) != 0)
	{
		Fclose(handle);
		return FALSE;
	}
	
	file_buffer = malloc(fsize);
	screen_buffer = malloc(SCREEN_SIZE + 256);
	if (file_buffer == NULL || screen_buffer == NULL)
	{
		free(file_buffer);
		free(screen_buffer);
		Fclose(handle);
		return FALSE;
	}
	Fread(handle, fsize, file_buffer);
	Fclose(handle);
	
	memset(screen_buffer, 0, SCREEN_SIZE);
	depack_imc(file_buffer, screen_buffer);
	free(file_buffer);

	info->planes = 1;
	info->colors = 1L << 1;
	info->width = 640;
	info->height = 400;
	info->indexed_color = FALSE;
	
	info->components = 1;
	info->real_width = info->width;
	info->real_height = info->height;
	info->memory_alloc = TT_RAM;
	info->page = 1;
	info->orientation = UP_TO_DOWN;
	info->num_comments = 0;
	info->_priv_var = 0;
	info->_priv_ptr = screen_buffer;
	info->_priv_ptr_more = screen_buffer;
	strcpy(info->info, "Signum! (Image)");
	strcpy(info->compression, "RLE");
	
	return TRUE;
}


boolean __CDECL reader_read(IMGINFO info, uint8_t *buffer)
{
	int8_t byte;
	int16_t x;
	uint8_t *src = info->_priv_ptr_more;

	x = info->width >> 3;
	do
	{
		byte = *src++;
		buffer[7] = byte & 1;
		byte >>= 1;
		buffer[6] = byte & 1;
		byte >>= 1;
		buffer[5] = byte & 1;
		byte >>= 1;
		buffer[4] = byte & 1;
		byte >>= 1;
		buffer[3] = byte & 1;
		byte >>= 1;
		buffer[2] = byte & 1;
		byte >>= 1;
		buffer[1] = byte & 1;
		byte >>= 1;
		buffer[0] = byte & 1;
		buffer += 8;
	} while (--x > 0);
	info->_priv_ptr_more = src;
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
