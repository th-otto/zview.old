#ifdef __PUREC__
#include "..\general.h"
#include "..\load_img.h"
#else
#include "../general.h"
#include "../load_img.h"
#endif
#include "dither.h"
#include "raster.h"

inline void raster_mono ( DECDATA info, void * _dst)
{
	uint16	*dst   	= _dst;
	uint32 	*map	= info->Pixel;
	int16   width 	= info->DthWidth;
	uint32  x     	= (info->IncXfx + 1) >> 1;
	uint16  pixel 	= 0x8000;
	uint16  chunk 	= 0;

	do 
	{
		if (  map[(int16)info->RowBuf[x >>16] & 1]) 
			chunk |= pixel;
		
		if ( !--width || !(pixel >>= 1)) 
		{
			*(dst++) = chunk;
			chunk    = 0;
			pixel    = 0x8000;
		}
		x += info->IncXfx;
	
	} while ( width);		 
}

inline void raster_D1 ( DECDATA info, void * _dst)
{
	uint16	*dst	= _dst;
	int16   width 	= info->DthWidth;
	uint32  x		= (info->IncXfx + 1) >> 1;
	uint32 	*map	= info->Pixel;
	uint16	mask	= info->PixMask;
	int16  	*buf	= info->DthBuf;
	int16   ins		= 0;
	uint16	pixel	= 0x8000;
	uint16	chunk	= 0;

	do 
	{
		ins += *buf + map[(int16)info->RowBuf[x >>16] & mask];

		if ( ins < 2040)
			chunk |= pixel;
		else
			ins -= 4080;

		*(buf++) = (ins >>= 2);
		
		if ( !--width || !(pixel >>= 1)) 
		{
			*(dst++) = chunk;
			chunk    = 0;
			pixel    = 0x8000;
		}
		x += info->IncXfx;
		
	} while ( width);
}

inline void gscale_D1( DECDATA info, void * _dst)
{
	uint16 	*dst  	= _dst;
	int16   width 	= info->DthWidth;
	uint32  x     	= ( info->IncXfx + 1) >> 1;
	int16 	*buf   	= info->DthBuf;
	int16   ins   	= 0;
	uint16  pixel 	= 0x8000;
	uint16  chunk 	= 0;
	
	do 
	{
		ins += *buf + ( int16)info->RowBuf[x >>16];
		
		if ( ins < 0x80)
			chunk |= pixel;
		else
			ins -= 0xFF;
		
		*(buf++) = (ins >>= 2);
		
		if ( !--width || !(pixel >>= 1)) 
		{
			*(dst++) = chunk;
			chunk    = 0;
			pixel    = 0x8000;
		}
		x += info->IncXfx;
		
	} while (width);
}

inline void dither_D1 ( DECDATA info, void * _dst)
{
	uint16 	*dst  	= _dst;
	int16   width 	= info->DthWidth;
	uint32  x     	= ( info->IncXfx + 1) >> 1;
	int16 	*buf   	= info->DthBuf;
	int16   ins   	= 0;
	uint16  pixel 	= 0x8000;
	uint16  chunk 	= 0;
	
	do 
	{
		uint8  *rgb   = &info->RowBuf[(x >>16) *3];
		ins	+= *buf + ( int16)rgb[0] * 5 + ( int16)rgb[1] * 9 + ( int16)rgb[2] * 2;
		
		if (ins < 2040) 
			chunk |= pixel;
		else
			ins -= 4080;
		
		*(buf++) = (ins >>= 2);
		
		if (!--width || !(pixel >>= 1)) 
		{
			*(dst++) = chunk;
			chunk    = 0;
			pixel    = 0x8000;
		}
		x += info->IncXfx;
		
	} while( width);
}

inline void raster_D2 ( DECDATA info, void * _dst)
{
	uint16 	*dst  	= _dst;
	int16   width 	= info->DthWidth;
	uint32  x     	= ( info->IncXfx + 1) >> 1;
	uint32 	*map   	= info->Pixel;
	uint16  mask  	= info->PixMask;
	int16 	*buf   	= info->DthBuf;
	int16   ins   	= 0;
	uint16  pixel 	= 0x8000;
	uint16  chunk 	= 0;

	do 
	{
		ins += *buf + map[( int16)info->RowBuf[x >>16] & mask];
		
		if ( ins < 2040)
			chunk |= pixel;
		else
			ins -= 4080;
		
		*(buf++) = (ins >>= 2);
		
		if (!--width || !(pixel >>= 1)) 
		{
			*(dst++) = chunk;
			*(dst++) = chunk;
			chunk    = 0;
			pixel    = 0x8000;
		}
		x += info->IncXfx;
		
	} while ( width);
}

inline void gscale_D2( DECDATA info, void * _dst)
{
	uint16 	*dst  	= _dst;
	int16   width 	= info->DthWidth;
	uint32  x     	= ( info->IncXfx + 1) >> 1;
	int16 	*buf   	= info->DthBuf;
	int16   ins   	= 0;
	uint16  pixel 	= 0x8000;
	uint16  chunk 	= 0;
	
	do 
	{
		ins += *buf + (short)info->RowBuf[x >>16];
		
		if (ins < 0x80)
			chunk |= pixel;
		else
			ins -= 0xFF;
		
		*(buf++) = (ins >>= 2);
		
		if (!--width || !(pixel >>= 1)) 
		{
			*(dst++) = chunk;
			*(dst++) = chunk;
			chunk    = 0;
			pixel    = 0x8000;
		}
		x += info->IncXfx;
		
	} while (width);
}

inline void dither_D2 ( DECDATA info, void * _dst)
{
	uint16 	*dst  	= _dst;
	int16   width 	= info->DthWidth;
	uint32  x     	= ( info->IncXfx + 1) >> 1;
	int16 	*buf   	= info->DthBuf;
	int16   ins   	= 0;
	uint16  pixel 	= 0x8000;
	uint16  chunk 	= 0;

	do 
	{
		uint8  *rgb   = &info->RowBuf[( x >>16) * 3];
		ins += *buf + (int16)rgb[0] * 5 + (int16)rgb[1] * 9 + (int16)rgb[2] * 2;
		
		if (ins < 2040)
			chunk |= pixel;
		else
			ins -= 4080;
		
		*(buf++) = (ins >>= 2);
		
		if (!--width || !(pixel >>= 1)) 
		{
			*(dst++) = chunk;
			*(dst++) = chunk;
			chunk    = 0;
			pixel    = 0x8000;
		}
		x += info->IncXfx;
		
	} while ( width);
}

inline void raster_chunks ( uint8 *src, uint16 *dst, uint16 num, uint16 depth)
{
	uint16   mask  	= 0x8000;
	uint16   *chunk = dst;
	uint8    pixel 	= *src;
	int16    i     	= depth;

	do 
	{
		*( chunk++) = ( pixel & 1 ? mask : 0);
		pixel >>= 1;
	} while (--i);
	
	while (--num) 
	{
		mask >>= 1;
		chunk = dst;
		pixel = *( ++src);
		i     = depth;

		do 
		{
			if ( pixel & 1) *chunk |=  mask;
			else           *chunk &= ~mask;
			chunk++;
			pixel >>= 1;
		} while (--i);
	}
}



inline void raster_I4 ( DECDATA info, void * _dst)
{
	uint16 	*dst   	= _dst;
	int16   width 	= info->DthWidth;
	uint32  x     	= (info->IncXfx +1) >> 1;
	uint8   buf[16];
	int16  	n   	= 16;
	uint8 	*tmp 	= buf;

	do 
	{
		uint16 idx = info->RowBuf[x >>16];
		*(tmp++)  = *( uint8*)&info->Pixel[idx];
		
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

inline void gscale_I4( DECDATA info, void * _dst)
{
	uint16 	*dst   	= _dst;
	int16   width 	= info->DthWidth;
	uint32  x     	= (info->IncXfx +1) >> 1;	
	int8 	*dth    = info->DthBuf;
	int16   err    	= 0;
	uint8   buf[16];
	int16  	n   	= 16;
	uint8 	*tmp 	= buf;

	do 
	{
		*(tmp++) = dither_gray (&info->RowBuf[x >>16], &err, &dth);
		
		if (!--width || !--n) 
		{
			raster_chunk4 (buf, dst, tmp - buf);
			dst += 4;
			n    = 16;
			tmp  = buf;
		}
		x += info->IncXfx;
		
	} while (width);
}

inline void dither_I4 ( DECDATA info, void * _dst)
{
	uint16 	*dst   	= _dst;
	int16   width 	= info->DthWidth;
	uint32  x     	= (info->IncXfx +1) >> 1;	
	int8 	*dth    = info->DthBuf;
	int16   err[3] 	= { 0, 0, 0 };	
	uint8   buf[16];
	int16  	n   	= 16;
	uint8 	*tmp 	= buf;

	do 
	{
		*(tmp++) = dither_true (&info->RowBuf[(x >>16) *3], err, &dth);
		
		if (!--width || !--n) 
		{
			raster_chunk4 (buf, dst, tmp - buf);
			dst += 4;
			n    = 16;
			tmp  = buf;
		}
		x += info->IncXfx;
		 
	} while (width);
}

inline void raster_I8 ( DECDATA info, void * _dst)
{
	uint16 	*dst   	= _dst;
	int16   width 	= info->DthWidth;
	uint32  x     	= (info->IncXfx +1) >> 1;
	uint8   buf[16];
	int16  	n   	= 16;
	uint8 	*tmp 	= buf;
	
	do 
	{
		uint16 idx = info->RowBuf[x >>16];
		*(tmp++)  = *(uint8*)&info->Pixel[idx];
		
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

inline void gscale_I8( DECDATA info, void * _dst)
{
	uint16 	*dst   	= _dst;
	int16   width 	= info->DthWidth;
	uint32  x     	= (info->IncXfx + 1) >> 1;	
	int8 	*dth    = info->DthBuf;
	int16   err    	= 0;
	uint8   buf[16];
	int16  	n   	= 16;
	uint8 	*tmp	 = buf;

	do 
	{
		*(tmp++) = dither_gray (&info->RowBuf[x >>16], &err, &dth);
		
		if (!--width || !--n) {
			raster_chunk8 (buf, dst, tmp - buf);
			dst += 8;
			n    = 16;
			tmp  = buf;
		}
		x += info->IncXfx;
		
	} while (width);
}

inline void dither_I8 (DECDATA info, void * _dst)
{
	uint16 	*dst   	= _dst;
	int16   width 	= info->DthWidth;
	uint32  x     	= (info->IncXfx + 1) >> 1;	
	int8 	*dth    = info->DthBuf;
	int16   err[3] 	= { 0, 0, 0 };
	uint8   buf[16];
	int16  	n   	= 16;
	uint8 	*tmp 	= buf;

	do 
	{
		*(tmp++) = dither_true (&info->RowBuf[(x >>16) *3], err, &dth);
		
		if (!--width || !--n) 
		{
			raster_chunk8 (buf, dst, tmp - buf);
			dst += 8;
			n    = 16;
			tmp  = buf;
		}
		x += info->IncXfx;
		 
	} while ( width);
}

inline void raster_P8 (DECDATA info, void * _dst)
{
	uint8  	*dst  = _dst;
	int16   width = info->DthWidth;
	uint32  x     = (info->IncXfx + 1) >> 1;
	uint32 	*map  = info->Pixel;
	uint16  mask  = info->PixMask;

	do 
	{
		*(dst++) = *( uint8*)&map[( int16)info->RowBuf[x >>16] & mask];
		x += info->IncXfx;
	} while (--width);
}

inline void gscale_P8( DECDATA info, void * _dst)
{
	uint8  	*dst  = _dst;
	int16   width = info->DthWidth;
	uint32  x     = (info->IncXfx + 1) >> 1;
	int8    *dth  = info->DthBuf;
	int16   err   = 0;

	do 
	{
		*(dst++) = dither_gray (&info->RowBuf[x >>16], &err, &dth);
		x += info->IncXfx;
	} while (--width);
}

inline void dither_P8 (DECDATA info, void * _dst)
{
	uint8 	*dst  	= _dst;
	int16  	width 	= info->DthWidth;
	uint32 	x     	= (info->IncXfx + 1) >> 1;
	int8 	*dth   	= info->DthBuf;
	int16   err[3] 	= { 0, 0, 0 };

	do 
	{
		*(dst++) = dither_true (&info->RowBuf[(x >>16) *3], err, &dth);
		x += info->IncXfx;
	} while (--width);
}

inline void raster_15 (DECDATA info, void * _dst)
{
	uint16 	*dst  = _dst;
	int16   width = info->DthWidth;
	uint32  x     = (info->IncXfx + 1) >> 1;
	uint32 	*map  = info->Pixel;
	uint16  mask  = info->PixMask;
	
	do 
	{
		*(dst++) = map[(short)info->RowBuf[x >>16] & mask];
		x += info->IncXfx;
	} while (--width);
}

inline void gscale_15( DECDATA info, void * _dst)
{
	uint16 * dst   = _dst;
	int16   width  = info->DthWidth;
	uint32  x      = (info->IncXfx + 1) >> 1;
	
	do 
	{
		uint16 rgb = info->RowBuf[(x >>16)];
		rgb        = ((rgb & 0xF8) <<7) | ((rgb & 0xF8) <<2) | (rgb >>3);
		*(dst++)   = (rgb >> 8) | (rgb << 8);
		x += info->IncXfx;
	} while (--width);
}

inline void dither_15 (DECDATA info, void * _dst)
{
	uint16 	*dst  = _dst;
	int16   width = info->DthWidth;
	uint32  x     = (info->IncXfx + 1) >> 1;

	do {
		uint8 * rgb = &info->RowBuf[(x >>16) * 3];
		*(dst++)   	= (((uint16)rgb[0] & 0xF8) << 7) | (((uint16)rgb[1] & 0xF8) << 2) | ( rgb[2] >> 3);
		x += info->IncXfx;
	} while (--width);
}

inline void raster_15r (DECDATA info, void * _dst)
{
	uint16 	*dst  = _dst;
	int16   width = info->DthWidth;
	uint32  x     = (info->IncXfx + 1) >> 1;
	uint32 	*map  = info->Pixel;
	uint16  mask  = info->PixMask;

	do 
	{
		uint16 rgb = map[(short)info->RowBuf[x >>16] & mask];
		*(dst++)  = (rgb >> 8) | (rgb << 8);
		x += info->IncXfx;
	} while (--width);
}

inline void gscale_15r( DECDATA info, void * _dst)
{
	uint16 * dst   = _dst;
	int16   width  = info->DthWidth;
	uint32  x      = (info->IncXfx + 1) >> 1;
	
	do 
	{
		uint16 rgb = info->RowBuf[(x >>16)];
		rgb        = ((rgb & 0xF8) <<7) | ((rgb & 0xF8) <<2) | (rgb >>3);
		*(dst++)   = (rgb >> 8) | (rgb << 8);
		x += info->IncXfx;
	} while (--width);
}

inline void dither_15r (DECDATA info, void * _dst)
{
	uint16 	*dst  = _dst;
	int16   width = info->DthWidth;
	uint32  x     = (info->IncXfx + 1) >> 1;
	
	do 
	{
		uint8 * rgb = &info->RowBuf[(x >>16) * 3];
		uint16  pix = (((uint16)rgb[0] & 0xF8) << 7) | (((uint16)rgb[1] & 0xF8) << 2) | (rgb[2] >> 3);
		*(dst++)   = (pix >> 8) | (pix << 8);
		x += info->IncXfx;
	} while (--width);
}

inline void raster_16 (DECDATA info, void * _dst)
{
	uint16 	*dst  = _dst;
	int16   width = info->DthWidth;
	uint32  x     = (info->IncXfx + 1) >> 1;
	uint32 	*map  = info->Pixel;
	uint16  mask  = info->PixMask;
	
	do 
	{
		*(dst++) = map[(int16)info->RowBuf[x >>16] & mask];
		x += info->IncXfx;
	} while (--width);
}

inline void gscale_16( DECDATA info, void * _dst)
{
	register uint16 *dst 	= _dst;
	register int16   width 	= info->DthWidth;
	register uint32  x     	= (info->IncXfx + 1) >> 1;

	do 
	{
		uint16 rgb = info->RowBuf[(x >>16)];
		*(dst++)  = ((rgb & 0xF8) <<8) | ((rgb & 0xFC) <<3) | (rgb >>3);
		x += info->IncXfx;
	} while (--width);
}
	
inline void dither_16 (DECDATA info, void * _dst)
{
	register uint16 *dst 	= _dst;
	register int16   width 	= info->DthWidth;
	register uint32  x     	= (info->IncXfx + 1) >> 1;
	
	do 
	{
		register uint8 *rgb = &info->RowBuf[(x >> 16) * 3];

		*(dst++) = (((uint16)rgb[0] & 0xF8) << 8) | (((uint16)rgb[1] & 0xFC) << 3) | ( rgb[2] >> 3);
		x += info->IncXfx;
	} while (--width);
}


inline void raster_16r (DECDATA info, void * _dst)
{
	uint16 	*dst  = _dst;
	int16   width = info->DthWidth;
	uint32  x     = (info->IncXfx + 1) >> 1;
	uint32 	*map  = info->Pixel;
	uint16  mask  = info->PixMask;
	
	do 
	{
		uint16 rgb = map[(int16)info->RowBuf[x >>16] & mask];
		*(dst++)  = (rgb >> 8) | (rgb << 8);
		x += info->IncXfx;
	} while (--width);
}

inline void gscale_16r( DECDATA info, void * _dst)
{
	uint16  *dst   = _dst;
	int16   width = info->DthWidth;
	uint32  x     = (info->IncXfx + 1) >> 1;

	do 
	{
		uint16 rgb = info->RowBuf[(x >>16)];
		rgb        = ((rgb & 0xF8) <<8) | ((rgb & 0xFC) <<3) | (rgb >>3);
		*(dst++)   = (rgb >> 8) | (rgb << 8);
		x += info->IncXfx;
	} while (--width);
}

inline void dither_16r (DECDATA info, void * _dst)
{
	uint16 	*dst  = _dst;
	int16   width = info->DthWidth;
	uint32  x     = (info->IncXfx + 1) >> 1;
	
	do 
	{
		uint8 * rgb = &info->RowBuf[(x >>16) *3];
		uint16  pix = ((( uint16)rgb[0] & 0xF8) << 8) | ((( uint16)rgb[1] & 0xFC) << 3) | ( rgb[2] >> 3);
		*(dst++)   = (pix >> 8) | (pix << 8);
		x += info->IncXfx;
	} while (--width);
}

inline void raster_24 (DECDATA info, void * _dst)
{
	uint8 	*dst  = _dst;
	int16   width = info->DthWidth;
	uint32  x     = (info->IncXfx + 1) >> 1;
	uint32 	*map  = info->Pixel;
	uint16  mask  = info->PixMask;

	do 
	{
		uint8 * rgb = (uint8*)&map[( int16)info->RowBuf[x >>16] & mask];
		*(dst++) = *(++rgb);
		*(dst++) = *(++rgb);
		*(dst++) = *(++rgb);
		x += info->IncXfx;
	} while (--width);
}

inline void gscale_24( DECDATA info, void * _dst)
{
	uint8  *dst  = _dst;
	int16  width = info->DthWidth;
	uint32 x     = (info->IncXfx + 1) >> 1;

	do 
	{
		uint8 rgb = info->RowBuf[(x >>16)];
		*(dst++) = rgb;
		*(dst++) = rgb;
		*(dst++) = rgb;
		x += info->IncXfx;
	} while (--width);
}

inline void dither_24 (DECDATA info, void * _dst)
{
	uint8 	*dst  = _dst;
	int16   width = info->DthWidth;
	uint32  x     = (info->IncXfx + 1) >> 1;
	
	do 
	{
		uint8 * rgb = &info->RowBuf[(x >>16) *3];
		*(dst++) = *(rgb++);
		*(dst++) = *(rgb++);
		*(dst++) = *(rgb);
		x += info->IncXfx;
	} while (--width);
}


inline void raster_24r (DECDATA info, void * _dst)
{
	uint8 	*dst  = _dst;
	int16   width = info->DthWidth;
	uint32  x     = (info->IncXfx + 1) >> 1;
	uint32 	*map  = info->Pixel;
	uint16  mask  = info->PixMask;

	do 
	{
		uint8 * rgb = (uint8*)&map[(int16)info->RowBuf[x >>16] & mask];
		*(dst++) = rgb[3];
		*(dst++) = rgb[2];
		*(dst++) = rgb[1];
		x += info->IncXfx;
	} while (--width);
}

inline void gscale_24r( DECDATA info, void * _dst)
{
	uint8  *dst  = _dst;
	int16  width = info->DthWidth;
	uint32 x     = (info->IncXfx + 1) >> 1;

	do 
	{
		uint8 rgb = info->RowBuf[(x >> 16)];
		*(dst++) = rgb;
		*(dst++) = rgb;
		*(dst++) = rgb;
		x += info->IncXfx;
	} while (--width);
}

inline void dither_24r( DECDATA info, void * _dst)
{
	uint8 	*dst  = _dst;
	int16   width = info->DthWidth;
	uint32  x     = (info->IncXfx + 1) >> 1;
	
	do 
	{
		uint8 * rgb = &info->RowBuf[(x >> 16) *3];
		*(dst++) = rgb[2];
		*(dst++) = rgb[1];
		*(dst++) = rgb[0];
		x += info->IncXfx;
	} while (--width);
}

inline void raster_32 (DECDATA info, void * _dst)
{
	uint32 * dst   = _dst;
	int16   width = info->DthWidth;
	uint32  x     = (info->IncXfx + 1) >> 1;
	uint32 	*map  = info->Pixel;
	uint16  mask  = info->PixMask;
	
	do 
	{
		*(dst++) = map[(int16)info->RowBuf[x >> 16] & mask];
		x += info->IncXfx;
	} while (--width);
}

inline void gscale_32( DECDATA info, void * _dst)
{
	uint32 *dst   = _dst;
	int16   width = info->DthWidth;
	uint32  x     = (info->IncXfx + 1) >> 1;

	do 
	{
		uint32 rgb = info->RowBuf[(x >> 16)];
		*(dst++)  = ((( rgb << 8) | rgb) << 8) | rgb;
		x += info->IncXfx;
	} while (--width);
}

inline void dither_32 (DECDATA info, void * _dst)
{
	uint32 * dst   = _dst;
	int16   width = info->DthWidth;
	uint32  x     = (info->IncXfx + 1) >> 1;
	
	do 
	{
		uint8 * rgb = &info->RowBuf[(x >> 16) *3];
		*(dst++)   = ((((uint32)rgb[0] << 8) | rgb[1]) << 8) | rgb[2];
		x += info->IncXfx;
	} while (--width);
}


inline void raster_32r( DECDATA info, void * _dst)
{
	uint32 * dst   = _dst;
	int16   width = info->DthWidth;
	uint32  x     = (info->IncXfx + 1) >> 1;
	uint32 	*map  = info->Pixel;
	uint16  mask  = info->PixMask;
	
	do 
	{
		uint8 * rgb = (uint8*)&map[(int16)info->RowBuf[x >>16] & mask];
		*(dst++) = (((((int32)rgb[3] << 8) | rgb[2]) << 8) | rgb[1]) << 8;
		x += info->IncXfx;
	} while (--width);
}

inline void gscale_32r( DECDATA info, void * _dst)
{
	uint32 *dst   = _dst;
	int16   width = info->DthWidth;
	uint32  x     = (info->IncXfx + 1) >> 1;

	do 
	{
		uint32 rgb = info->RowBuf[(x >>16)];
		*(dst++)  = (((( rgb << 8) | rgb) << 8) | rgb) << 8;
		x += info->IncXfx;
	} while (--width);
}

inline void dither_32r (DECDATA info, void * _dst)
{
	uint32 * dst   = _dst;
	int16   width = info->DthWidth;
	uint32  x     = (info->IncXfx + 1) >> 1;
	
	do 
	{
		uint8 * rgb = &info->RowBuf[(x >>16) *3];
		*(dst++)   = (((((uint32)rgb[2] <<8) | rgb[1]) << 8) | rgb[0]) << 8;
		x += info->IncXfx; 
	} while (--width);
}

inline void raster_32z ( DECDATA info, void * _dst)
{
	uint32 * dst   = _dst;
	int16   width = info->DthWidth;
	uint32  x     = (info->IncXfx +1) >> 1;
	uint32 * map   = info->Pixel;
	uint16   mask  = info->PixMask;
	do {
		*(dst++) = map[(int16)info->RowBuf[x >>16] & mask] << 8;
		x += info->IncXfx;
	} while (--width);
}

inline void gscale_32z (DECDATA info, void * _dst)
{
	uint32 * dst   = _dst;
	int16   width = info->DthWidth;
	uint32  x     = (info->IncXfx +1) >> 1;
	do {
		uint32 rgb = info->RowBuf[(x >>16)];
		*(dst++)  = ((((rgb <<8) | rgb) <<8) | rgb) << 8;
		x += info->IncXfx;
	} while (--width);
}

inline void dither_32z (DECDATA info, void * _dst)
{
	uint32  *dst	= _dst;
	int16   width	= info->DthWidth;
	uint32  x    	= (info->IncXfx +1) >> 1;
	do {
		uint8 * rgb = &info->RowBuf[(x >>16) *3];
		*(dst++)   = (((((uint32)rgb[0] <<8) | rgb[1]) <<8) | rgb[2]) << 8;
		x += info->IncXfx;
	} while (--width);
}
