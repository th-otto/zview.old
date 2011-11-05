#include "..//general.h"
#include "..//pic_load.h"
#include "dither.h"
#include "raster.h"

inline void raster_mono ( DECDATA info, void * _dst)
{
#ifdef USE_ASM
	__asm__ volatile (
		"	subq.w		#1, %2			| width\n"
		"	move.l		%3, d0			| scale -> index\n"
		"	addq.l		#1, d0\n"
		"	lsr.l		#1, d0\n"
		"0:\n"
		"	clr.w 		d1				| chunk\n"
		"	move.w		#0x8000, d2		| pixel\n"
		"1:\n"
		"	swap		d0\n"
		"	btst		#0, (d0.w,%1) 	| (src[index>>16] & 1)\n"
		"	beq.b		2f\n"
		"	or.w		d2, d1        	| chunk |= pixel\n"
		"2:\n"
		"	swap		d0\n"
		"	add.l		%3, d0        	| index += info->IncXfx\n"
		"	lsr.w		#1, d2\n"
		"	dbeq		%2, 1b\n"
		"	move.w		d1, (%0)+\n"
		"	subq.w		#1, %2\n"
		"	bpl.b		0b"
		:                                       /* output */
		: "a"(_dst), "a"(info->RowBuf),
		  /*  %0         %1             */
		  "d"(info->DthWidth),"d"(info->IncXfx) /* input  */
		  /*  %2                  %3            */
		: "d0","d1","d2"                        /* clobbered */
	);
#else
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
#endif		 
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
		ins	+= *buf + ( int16)rgb[0] * 5 + ( int16)rgb[1] * 9 + (( int16)rgb[2] << 1);
		
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


#ifdef USE_ASM
inline void raster_chunk4 (uint8 *src, uint16* dst, size_t num)
{
	__asm__ volatile (
		"	clr.l		d4\n"
		"	move.b		(%0)+, d4\n"
		"	move.l		d4, d5\n"
		"	andi.b		#0x03, d4		|chunks 0/1\n"
		"	ror.l 		#1, d4\n"
		"	ror.w		#1, d4\n"
		"	andi.b		#0x0C, d5		|chunks 2/3\n"
		"	ror.l 		#3, d5\n"
		"	ror.w		#1, d5\n"
		
		"	subq.l		#2, %2\n"
		"	bmi			9f\n"
		
		"	moveq.l		#1, d1\n"
		
		"1: 							| chunk loop\n"
		"	move.b		(%0)+, d2\n"
		
		"	moveq.l		#0x03, d3 		|chunks 0/1\n"
		"	and.b		d2, d3\n"
		"	ror.l 		#1, d3\n"
		"	ror.w		#1, d3\n"
		"	lsr.l		d1, d3\n"
		"	or.l		d3, d4\n"
		"	moveq.l		#0x0C, d3 		|chunks 2/3\n"
		"	and.b		d2, d3\n"
		"	ror.l 		#3, d3\n"
		"	ror.w		#1, d3\n"
		"	lsr.l		d1, d3\n"
		"	or.l		d3, d5\n"
		
		"	addq.w		#1, d1\n"
		"	dbra		%2, 1b 			| chunk loop\n"
		"9:\n"
		"	movem.l		d4-d5, (%1)"
		: /* no return value */
		: "a"(src),"a"(dst), "d"(num)
		/*    %0       %1        %2 */
		: "d1","d2","d3","d4","d5"
	);
}

inline void raster_chunk8 ( uint8 *src, uint16* dst, size_t num)
{
	__asm__ volatile 
	(
	"	clr.l		d4\n"
	"	move.b		(%0)+, d4\n"
	"	move.l		d4, d5\n"
	"	move.l		d4, d6\n"
	"	move.l		d4, d7\n"
	"	andi.b		#0x03, d4 		|chunks 0/1\n"
	"	ror.l 		#1, d4\n"
	"	ror.w		#1, d4\n"
	"	andi.b		#0x0C, d5		|chunks 2/3\n"
	"	ror.l 		#3, d5\n"
	"	ror.w		#1, d5\n"
	"	andi.b		#0x30, d6		|chunks 4/5\n"
	"	ror.l 		#5, d6\n"
	"	ror.w		#1, d6\n"
	"	andi.b		#0xC0, d7		|chunks 6/7\n"
	"	ror.l 		#7, d7\n"
	"	ror.w		#1, d7\n"
	"	subq.l		#2, %2\n"
	"	bmi			9f\n"
	"	moveq.l		#1, d1\n"
	"1: 							| chunk loop\n"
	"	move.b		(%0)+, d2\n"
	"	moveq.l		#0x03, d3 		|chunks 0/1\n"
	"	and.b		d2, d3\n"
	"	ror.l 		#1, d3\n"
	"	ror.w		#1, d3\n"
	"	lsr.l		d1, d3\n"
	"	or.l		d3, d4\n"
	"	moveq.l		#0x0C, d3 		|chunks 2/3\n"
	"	and.b		d2, d3\n"
	"	ror.l 		#3, d3\n"
	"	ror.w		#1, d3\n"
	"	lsr.l		d1, d3\n"
	"	or.l		d3, d5\n"
	"	moveq.l		#0x30, d3 		|chunks 4/5\n"
	"	and.b		d2, d3\n"
	"	ror.l 		#5, d3\n"
	"	ror.w		#1, d3\n"
	"	lsr.l		d1, d3\n"
	"	or.l		d3, d6\n"
	"	move.l		#0xC0, d3 		|chunks 6/7\n"
	"	and.b		d2, d3\n"
	"	ror.l 		#7, d3\n"
	"	ror.w		#1, d3\n"
	"	lsr.l		d1, d3\n"
	"	or.l		d3, d7\n"
		
	"	addq.w		#1, d1\n"
	"	dbra		%2, 1b 			| chunk loop\n"
	"9:\n"
	"	movem.l		d4-d7, (%1)"
		: /* no return value */
		: "a"(src),"a"(dst), "d"(num)
		/*    %0       %1        %2 */
		: "d1","d2","d3","d4","d5","d6","d7"
	);
}

#else

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

#endif

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
#ifdef USE_ASM
	size_t  x     = (info->IncXfx + 1) >> 1;
	__asm__ volatile (
	"	subq.w		#1, %4\n"
	"	swap		%6 				| -> chunk counter\n"
		
	"	1: 							| line loop\n"
	"	clr.w		%6\n"
	"	clr.l		d4\n"
	"	clr.l		d5\n"
	"	clr.l		d6\n"
	"	clr.l		d7\n"
		
	"	5: 							| chunk loop\n"
	"	swap		%4 				| -> value\n"
	"	swap		%6 				| -> mask\n"
	"	move.w		(%3), d3\n"
	"	add.l		%5, (%3)\n"
	"	move.b		(d3.w,%1), d3 	| palette index\n"
	"	and.w		%6, d3\n"
	"	lsl.w		#2, d3\n"
	"	move.b		(d3.w,%2), %4 	| pixel value\n"
	"	swap		%6 				| -> chunk counter\n"
		
	"	moveq.l		#0x03, d3 		|chunks 0/1\n"
	"	and.b		%4, d3\n"
	"	ror.l 		#1, d3\n"
	"	ror.w		#1, d3\n"
	"	lsr.l		%6, d3\n"
	"	or.l		d3, d4\n"
	"	moveq.l		#0x0C, d3 		|chunks 2/3\n"
	"	and.b		%4, d3\n"
	"	ror.l 		#3, d3\n"
	"	ror.w		#1, d3\n"
	"	lsr.l		%6, d3\n"
	"	or.l		d3, d5\n"
	"	moveq.l		#0x30, d3 		|chunks 4/5\n"
	"	and.b		%4, d3\n"
	"	ror.l	 	#5, d3\n"
	"	ror.w		#1, d3\n"
	"	lsr.l		%6, d3\n"
	"	or.l		d3, d6\n"
	"	move.l		#0xC0, d3 		|chunks 6/7\n"
	"	and.b		%4, d3\n"
	"	ror.l 		#7, d3\n"
	"	ror.w		#1, d3\n"
	"	lsr.l		%6, d3\n"
	"	or.l		d3, d7\n"
		
	"	swap		%4 				| -> width\n"
	"	addq.b		#1, %6\n"
	"	btst.b		#4, %6\n"
	"	dbne		%4, 5b 			| chunk loop\n"

	"	movem.l		d4-d7, (%0)\n"
	"	adda.w		#16, %0\n"
	"	subq.w		#1, %4\n"
	"	bpl.b		1b"
		:
		: "a"(_dst),"a"(info->RowBuf),"a"(info->Pixel),"a"(&x),
		/*    %0        %1                %2               %3 */
		  "d"(info->DthWidth),"d"(info->IncXfx),"d"(info->PixMask)
		/*    %4                  %5                %6          */
		: "d3","d4","d5","d6","d7"
	);
#else	
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
#endif
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


inline void dither32_P8 (DECDATA info, void * _dst)
{
	uint8 	*dst  	= _dst;
	int16  	width 	= info->DthWidth;
	uint32 	x     	= (info->IncXfx + 1) >> 1;
	int8 	*dth   	= info->DthBuf;
	int16   err[3] 	= { 0, 0, 0 };

	do 
	{
		*(dst++) = ditherize_32 (&info->RowBuf[(x >>16) << 2], err, &dth);
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
#ifdef USE_ASM
	if (info->IncXfx == 0x00010000uL) {
		__asm__ volatile (
			"subq.l		#1, %2\n"
		"	lsr.l		#1, %2\n"
		"	1:\n"
		"	clr.l		d0\n"
		"	move.w		(%0)+, d0 	|........:........|12345678:12345678|\n"
		"	lsl.l		#8, d0    	|........:12345678|12345678:........|\n"
		"	lsr.w		#8, d0    	|........:12345678|........:12345678|\n"
		"	move.l		d0, d1\n"
		"	lsr.l  		#2, d0    	|........:..123456|78......:..123456|\n"
		"	andi.w		#0x3F, d0 	|........:..123456|........:..123456|\n"
		"	lsl.l		#5, d0    	|.....123:456.....|.....123:456.....|\n"
		"	lsr.l		#3, d1    	|........:...12345|678.....:...12345|\n"
		"	andi.w		#0x1F, d1 	|........:...12345|........:...12345|\n"
		"	or.l		d1, d0\n"
		"	lsl.l		#8, d1    	|...12345:........|...12345:........|\n"
		"	lsl.l		#3, d1    	|12345...:........|12345...:........|\n"
		"	or.l		d1, d0\n"
		"	move.l		d0, (%1)+\n"
		"	dbra		%2, 1b"
			: /* no return */
			: "a"(info->RowBuf),"a"(_dst), "d"((long)info->DthWidth)
			/*    %0                %1         %2           */
			: "d0","d1","d2"
		);
	} else
#endif
{
	uint16 *dst 	= _dst;
	int16   width 	= info->DthWidth;
	uint32  x     	= (info->IncXfx + 1) >> 1;

	do 
	{
#ifdef USE_ASM
		__asm__ volatile (
		"	move.b		(%0), d0\n"
		"	move.b		d0, d1\n"
		"	lsl.w		#5, d0\n"
		"	move.b		d1, d0\n"
		"	lsl.l		#6, d0\n"
		"	move.b		d1, d0\n"
		"	lsr.l		#3, d0\n"
		"	move.w		d0, (%1)"
			: /* no return */
			: "a"(&info->RowBuf[x >>16]),"a"(dst++)
			: "d0","d1"
		);
#else

		uint16 rgb = info->RowBuf[(x >>16)];
		*(dst++)  = ((rgb & 0xF8) <<8) | ((rgb & 0xFC) <<3) | (rgb >>3);
#endif
		x += info->IncXfx;
	} while (--width);
}}
	
inline void dither_16 (DECDATA info, void * _dst)
{
#ifdef USE_ASM
	if (info->IncXfx == 0x00010000uL) {
		__asm__ volatile (
		"	subq.l		#1, %2\n"
		"	lsr.l		#1, %2\n"
		"1:\n"
		"	movem.w		(%0), d0/d1/d2 |R8:G8| / |B8:r8| / |g8:b8|\n"
		"	addq.l		#6, %0\n"
		"	lsl.l		#5, d0      |........:...RRRRR|RRRGGGGG:GGG00000|\n"
		"	lsl.w		#3, d0	  	|........:...RRRRR|GGGGGGGG:00000000|\n"
		"	lsl.l		#6, d0      |.....RRR:RRGGGGGG|GG000000:00000000|\n"
		"	move.w		d1, d0      |.....RRR:RRGGGGGG|BBBBBBBB:rrrrrrrr|\n"
		"	lsl.l		#5, d0      |RRRRRGGG:GGGBBBBB|BBBrrrrr:rrr00000|\n"
		"	move.w		d0, d1      |........:........|BBBrrrrr:rrr00000|\n"
		"	lsl.l		#8, d1      |........:BBBrrrrr|rrr00000:00000000|\n"
		"	move.w		d2, d1      |........:BBBrrrrr|gggggggg:bbbbbbbb|\n"
		"	lsl.l		#6, d1      |..BBBrrr:rrgggggg|ggbbbbbb:bb000000|\n"
		"	lsl.w		#2, d1		|..BBBrrr:rrgggggg|bbbbbbbb:00000000|\n"
		"	lsl.l		#5, d1      |rrrrrggg:gggbbbbb|bbb00000:00000000|\n"
		"	swap		d1\n"
		"	move.w		d1, d0\n"
		"	move.l		d0, (%1)+\n"
		"	dbra		%2, 1b"
			: /* no return */
			: "a"(info->RowBuf),"a"(_dst), "d"(info->DthWidth)
			/*    %0                %1        %2           */
			: "d0","d1","d2"
		);
	} else
#endif
{
	uint16 *dst 	= _dst;
	int16   width 	= info->DthWidth;
	uint32  x     	= (info->IncXfx + 1) >> 1;
	
	do 
	{
#ifdef USE_ASM
		__asm__ volatile (
			"move.b		(%0)+, d0\n"
		"	lsl.w		#5, d0\n"
		"	move.b		(%0)+, d0\n"
		"	lsl.l		#6, d0\n"
		"	move.b		(%0)+, d0\n"
		"	lsr.l		#3, d0\n"
		"	move.w		d0, (%1)"
			: /* no return */
			: "a"(&info->RowBuf[(x >>16) *3]),"a"(dst++)
			: "d0"
		);
#else
		register uint8 *rgb = &info->RowBuf[(x >> 16) * 3];

		*(dst++) = (((uint16)rgb[0] & 0xF8) << 8) | (((uint16)rgb[1] & 0xFC) << 3) | ( rgb[2] >> 3);
#endif
		x += info->IncXfx;
	} while (--width);
}}


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
