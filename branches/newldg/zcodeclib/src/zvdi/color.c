#ifdef __PUREC__
#include "..\general.h"
#include "..\load_img.h"
#else
#include "../general.h"
#include "../load_img.h"
#endif
#include "vdi.h"

typedef struct 
{
	uint8 satur;
	uint8 red, green, blue;
} SRGB;

static SRGB screen_colortab[256];


/* prototype */
void save_colors(void);
int16 remap_color (int32 value);
uint32 color_lookup ( uint32 rgb, int16 *trans);
void cnvpal_mono ( IMGINFO info, DECDATA data);
void cnvpal_1_2 (IMGINFO info, DECDATA data);
void cnvpal_4_8( IMGINFO info, DECDATA data);
void cnvpal_15( IMGINFO info, DECDATA data);
void cnvpal_high( IMGINFO info, DECDATA data);
void cnvpal_true( IMGINFO info, DECDATA data);


static uint8 saturation (uint8 * rgb)
{
	uint8 satr;

	if (rgb[0] >= rgb[1]) 
	{
		if (rgb[1] >= rgb[2])   
			satr = rgb[0]   - rgb[2];
		else if (rgb[0] > rgb[2]) 
			satr = rgb[0]   - rgb[1];
		else
			satr = rgb[2]  - rgb[1];
	} 
	else if (rgb[1] >= rgb[2]) 
	{
		if (rgb[0] >= rgb[2])
	    	satr = rgb[1] - rgb[2];
		else
			satr = rgb[1] - rgb[0];
	} 
	else 
		satr = rgb[2]  - rgb[0];

	return satr;
}


void save_colors(void)
{
	register int16  i;
	int16 colors = 1 << nplanes;

	for( i = 0; i < colors; i++)
	{
		int16 coltab[3];
		
		vq_color( vdi_handle, i, 1, coltab);

		screen_colortab[i].red   = ((((int32)coltab[0] << 8 ) - coltab[0]) + 500) / 1000;
		screen_colortab[i].green = ((((int32)coltab[1] << 8 ) - coltab[1]) + 500) / 1000;
		screen_colortab[i].blue  = ((((int32)coltab[2] << 8 ) - coltab[2]) + 500) / 1000;
		screen_colortab[i].satur = saturation ( &screen_colortab[i].red);
	}
}

int16 remap_color (int32 value)
{
	int16 red   = ((uint8*)&value)[1];
	int16 green = ((uint8*)&value)[2];
	int16 blue  = ((uint8*)&value)[3];
	int16 satur = saturation (((uint8*)&value) +1);
	
	int16  best_fit  = 0;
	uint16 best_err  = 0xFFFFu;
	int16  max_color = 1 << nplanes;
	
	int16 i = 0;
	
	value = ( value & 0x00FFFFFFl) | (( int32)satur << 24);
	
	do 
	{
		if ( *( int32*)&screen_colortab[i] == value) 
		{
			/* gotcha! */
			best_fit = i;
			break;
		} 
		else 
		{
			uint16 err = 
			( red > screen_colortab[i].red ? red - screen_colortab[i].red : screen_colortab[i].red - red)
			+ (green > screen_colortab[i].green ? green - screen_colortab[i].green : screen_colortab[i].green - green)
			+ (blue  > screen_colortab[i].blue  ? blue  - screen_colortab[i].blue  : screen_colortab[i].blue - blue)
			+ (satur > screen_colortab[i].satur ? satur - screen_colortab[i].satur : screen_colortab[i].satur - satur);
			
			if (err <= best_err) 
			{
				best_err = err;
				best_fit = i;
			}
		}
	} while (++i < max_color);
	
	return best_fit;
}


uint32 color_lookup ( uint32 rgb, int16 *trans)
{
	uint8 idx = ((rgb & ~0xFFuL) == ~0xFFuL ? rgb : remap_color (rgb));
	return ( (( int32)(trans ? trans[idx] : idx) << 24) | (*(int32*)&screen_colortab[idx] & 0x00FFFFFFuL));
}


void cnvpal_mono ( IMGINFO info, DECDATA data)
{
/*	if ( info->indexed_color) 
	{
		uint32  bgnd, fgnd;
		
		bgnd = (((( int32)info->palette[0].red <<8) | info->palette[0].green) <<8) | info->palette[0].blue;
		fgnd = (((( int32)info->palette[1].red <<8) | info->palette[1].green) <<8) | info->palette[1].blue;

		data->Pixel[0] = remap_color ( bgnd);
		data->Pixel[1] = remap_color ( fgnd);
		
	}
	else 
*/	{
		data->Pixel[0] = G_WHITE;
		data->Pixel[1] = G_BLACK;
	}
}


void cnvpal_1_2 (IMGINFO info, DECDATA data)
{
	register int32 i;

	for( i = 0; i < info->colors; i++)
	{
		data->Pixel[i] = ( int16)info->palette[i].red * 5 + ( int16)info->palette[i].green * 9 + (( int16)info->palette[i].blue << 1);
	}
}


void cnvpal_4_8( IMGINFO info, DECDATA data)
{
	register int32 i;

	for( i = 0; i < info->colors; i++)
	{
	    uint32 rgb = (((( int32)info->palette[i].red << 8) | info->palette[i].green) << 8) | info->palette[i].blue;
		data->Pixel[i] = rgb | (( int32)pixel_val[remap_color ( rgb)] << 24);
	}
}


void cnvpal_15( IMGINFO info, DECDATA data)
{
	register int32 i;

	for( i = 0; i < info->colors; i++)
	{
		data->Pixel[i] = (( int16)( info->palette[i].red & 0xF8) << 7) | (( int16)( info->palette[i].green & 0xF8) << 2) | ( info->palette[i].blue >> 3);
	}
}


void cnvpal_high( IMGINFO info, DECDATA data)
{
	register int32 i;

	for( i = 0; i < info->colors; i++)
	{
		data->Pixel[i] = (( int16)( info->palette[i].red & 0xF8) << 8) | (( int16)( info->palette[i].green & 0xFC) << 3) | ( info->palette[i].blue >> 3);
	}
}


void cnvpal_true ( IMGINFO info, DECDATA data)
{
	register int32 i;
	
	for( i = 0; i < info->colors; i++)
	{
		data->Pixel[i] = (((( int32)info->palette[i].red << 8) | info->palette[i].green) << 8) | info->palette[i].blue;
	}
}
