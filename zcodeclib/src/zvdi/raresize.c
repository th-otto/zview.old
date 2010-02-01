#ifdef __PUREC__
#include "..\general.h"
#include "..\load_img.h"
#else
#include "../general.h"
#include "../load_img.h"
#endif
#include "dither.h"
#include "raster.h"

inline void resize_mono ( DECDATA info, void * _dst)
{
	uint8	*dst   	= _dst;
	int16   width 	= info->DthWidth;
	uint32  x     	= (info->IncXfx + 1) >> 4;
	uint16  pixel 	= 0x80, chunk = 0;

	do 
	{
		chunk |= ( info->RowBuf[x >> 16] & 1);
		chunk <<= 1;		

		if ( !--width || !(pixel >>= 1)) 
		{
			*(dst++) = chunk;
			chunk    = 0;
			pixel    = 0x80;
		}
		
		x += ( info->IncXfx >> 3);

	} while ( width);		
}


inline void resize_I4 ( DECDATA info, void * _dst)
{
	uint16 	*dst   	= _dst;
	int16   width 	= info->DthWidth;
	uint32  x     	= (info->IncXfx +1) >> 1;
	uint8   buf[16];
	int16  	n   	= 16;
	uint8 	*tmp 	= buf;

	do 
	{
		*(tmp++) = *( uint8*)&info->DstBuf[x >> 16];
		
		if (!--width || !--n) 
		{
			raster_chunk4( buf, dst, tmp - buf);
			dst += 4;
			n    = 16;
			tmp  = buf;
		}
		x += info->IncXfx;
		
	} while ( width);
}


inline void resize_I8 ( DECDATA info, void * _dst)
{
	uint16 	*dst   		= _dst;
	int16   width 		= info->DthWidth;
	uint32  x     		= (info->IncXfx +1) >> 1;
	uint8   buf[16];
	int16  	n   		= 16;
	uint8 	*tmp 		= buf;
	
	do 
	{
		*(tmp++) = *( uint8*)&info->DstBuf[x >> 16];
		
		if (!--width || !--n) 
		{
			raster_chunk8 (buf, dst, tmp - buf);
			dst += 8;
			n    = 16;
			tmp  = buf;
		}
		x += info->IncXfx;
		
	} while (width);
}


inline void resize_P8(DECDATA info, void * _dst)
{
	uint8 	*dst  		= _dst;
	int16   width 		= info->DthWidth;
	uint32  x     		= (info->IncXfx + 1) >> 1;
	
	do 
	{
		uint8 *rgb = &info->RowBuf[x >> 16];
		*(dst++) = *(rgb);
		x += info->IncXfx;
	} while (--width);
}


inline void resize_16(DECDATA info, void * _dst)
{
	uint8 	*dst  		= _dst;
	int16   width 		= info->DthWidth;
	uint32  x     		= (info->IncXfx + 1) >> 1;
	
	do 
	{
		uint8 *rgb = &info->RowBuf[(x >> 16) << 1];
		*(dst++) = *(rgb++);
		*(dst++) = *(rgb);
		x += info->IncXfx;
	} while (--width);
}


inline void resize_24(DECDATA info, void * _dst)
{
	uint8 	*dst  = _dst;
	int16   width = info->DthWidth;
	uint32  x     = (info->IncXfx + 1) >> 1;
	
	do 
	{
		uint8 * rgb = &info->RowBuf[(x >>16) * 3];
		*(dst++) = *(rgb++);
		*(dst++) = *(rgb++);
		*(dst++) = *(rgb);
		x += info->IncXfx;
	} while (--width);
}


inline void resize_32( DECDATA info, void * _dst)
{
	uint8  *dst   = _dst;
	int16   width = info->DthWidth;
	uint32  x     = (info->IncXfx + 1) >> 1;
	
	do 
	{
		uint8 * rgb = &info->RowBuf[(x >> 16) << 2];
		*(dst++) = *(rgb++);
		*(dst++) = *(rgb++);
		*(dst++) = *(rgb++);
		*(dst++) = *(rgb);

		x += info->IncXfx;
	} while (--width);
}
