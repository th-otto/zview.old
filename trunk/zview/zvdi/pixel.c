#include "../general.h"

void  ( *getPixel)( int, int, uint32, uint8*, uint8*, uint8*, uint8*);
void  ( *setPixel)( int, int, uint32, uint8, uint8, uint8, uint8*);


void getPixel_32( int x, int y, uint32 line_size, uint8 *red, uint8 *green, uint8 *blue, uint8 *src)
{
	register uint8 *line_source = src + ( y * line_size) + ( x << 2);
	
	*red   = line_source[1];
	*green = line_source[2];
	*blue  = line_source[3];
}


void setPixel_32( int x, int y, uint32 line_size, uint8 red, uint8 green, uint8 blue, uint8 *dst)
{
	register uint32 *line_dst = ( uint32*)( dst + ( y * line_size));
	
	line_dst[x] = ((((uint32)red << 8) | green) << 8) | blue;	
}


void getPixel_32r( int x, int y, uint32 line_size, uint8 *red, uint8 *green, uint8 *blue, uint8 *src)
{
	register uint8 *line_source = src + ( y * line_size) + ( x << 2);
	
	*red   = line_source[2];
	*green = line_source[1];
	*blue  = line_source[0];
}


void setPixel_32r( int x, int y, uint32 line_size, uint8 red, uint8 green, uint8 blue, uint8 *dst)
{
	register uint32 *line_dst = ( uint32*)( dst + ( y * line_size));

	line_dst[x] = (((((uint32)blue << 8) | green) << 8) | red) << 8;
}


void getPixel_32z( int x, int y, uint32 line_size, uint8 *red, uint8 *green, uint8 *blue, uint8 *src)
{
	register uint8 *line_source = src + ( y * line_size) + ( x << 2);
	
	*red   = line_source[0];
	*green = line_source[1];
	*blue  = line_source[2];
}


void setPixel_32z( int x, int y, uint32 line_size, uint8 red, uint8 green, uint8 blue, uint8 *dst)
{
	register uint32 *line_dst = ( uint32*)( dst + ( y * line_size));

	line_dst[x] = (((((uint32)red << 8) | green) << 8) | blue) << 8;
}


void getPixel_24( int x, int y, uint32 line_size, uint8 *red, uint8 *green, uint8 *blue, uint8 *src)
{
	register uint8 *line_source = src + ( y * line_size) + ( x * 3);
	
	*red   = line_source[0];
	*green = line_source[1];
	*blue  = line_source[2];
}


void setPixel_24( int x, int y, uint32 line_size, uint8 red, uint8 green, uint8 blue, uint8 *dst)
{
	register uint8 *line_dst = dst + ( y * line_size) + ( x * 3);
	
	line_dst[0] = red;
	line_dst[1] = green;
	line_dst[2] = blue;	
}


void getPixel_24r( int x, int y, uint32 line_size, uint8 *red, uint8 *green, uint8 *blue, uint8 *src)
{
	register uint8 *line_source = src + ( y * line_size) + ( x * 3);
	
	*red   = line_source[2];
	*green = line_source[1];
	*blue  = line_source[0];
}


void setPixel_24r( int x, int y, uint32 line_size, uint8 red, uint8 green, uint8 blue, uint8 *dst)
{
	register uint8 *line_dst = dst + ( y * line_size) + ( x * 3);
	
	line_dst[2] = red;
	line_dst[1] = green;
	line_dst[0] = blue;	
}


void getPixel_16( int x, int y, uint32 line_size, uint8 *red, uint8 *green, uint8 *blue, uint8 *src)
{
	register uint16 *line_source = ( uint16*)( src + ( y * line_size) + ( x << 1));
	register uint16 src16 = line_source[0];

	*red   = (( src16 >> 11) & 0x001F) << 3; 
    *green = (( src16 >> 5)  & 0x003F) << 2;
    *blue  = (( src16)       & 0x001F) << 3;
}


void setPixel_16( int x, int y, uint32 line_size, uint8 red, uint8 green, uint8 blue, uint8 *dst)
{
	register uint16 *line_dst = ( uint16*)( dst + ( y * line_size));
	
	line_dst[x] = (((uint16)red & 0xF8) << 8) | (((uint16)green & 0xFC) << 3) | ( blue >> 3);
}


void getPixel_16r( int x, int y, uint32 line_size, uint8 *red, uint8 *green, uint8 *blue, uint8 *src)
{
	register uint16 *line_source = ( uint16*)( src + ( y * line_size) + ( x << 1));
	register uint16 src16 = (line_source[0] >> 8) | (line_source[0] << 8);

	*red   = (( src16 >> 11) & 0x001F) << 3; 
    *green = (( src16 >> 5)  & 0x003F) << 2;
    *blue  = (( src16)       & 0x001F) << 3;
}


void setPixel_16r( int x, int y, uint32 line_size, uint8 red, uint8 green, uint8 blue, uint8 *dst)
{
	register uint16 *line_dst	= ( uint16*)( dst + ( y * line_size));
	register uint16  pix 		= (((uint16)red & 0xF8) << 8) | (((uint16)green & 0xFC) << 3) | ( blue >> 3);

	line_dst[x] = (pix >> 8) | (pix << 8);
}
