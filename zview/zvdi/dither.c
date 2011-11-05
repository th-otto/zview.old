#include	"../general.h"

extern uint32 	cube216[216];
extern uint32   graymap[32];

/* prototype */
inline uint8 dither_true ( uint8 * rgb, int16 * err, int8 ** buf);
inline uint8 dither_gray( uint8 *gray, int16 *err, int8 **buf);

inline uint8 ditherize_32 ( uint8 * rgb, int16 * err, int8 ** buf)
{
	int8 	*dth = *buf;
	unsigned short  r    = (( err[0] += ( int16)dth[0] + rgb[1]) <= 42 ? 0 : err[0] >= 213 ? 5 : ( err[0] * 3) >> 7);
	unsigned short  g    = (( err[1] += ( int16)dth[1] + rgb[2]) <= 42 ? 0 : err[1] >= 213 ? 5 : ( err[1] * 3) >> 7);
	unsigned short  b    = (( err[2] += ( int16)dth[2] + rgb[3]) <= 42 ? 0 : err[2] >= 213 ? 5 : ( err[2] * 3) >> 7);
	uint8 * irgb = ( uint8*)&cube216[( r * 6 + g) * 6 + b];
	
	err[0] -= irgb[1];
	dth[0] =  ( err[0] <= -254 ? ( err[0] =- 127) : err[0] >= +254 ? ( err[0] =+ 127) : ( err[0] >>= 1));
	err[1] -= irgb[2];
	dth[1] =  ( err[1] <= -254 ? ( err[1] =- 127) : err[1] >= +254 ? ( err[1] =+ 127) : ( err[1] >>= 1));
	err[2] -= irgb[3];
	dth[2] =  ( err[2] <= -254 ? ( err[2] = -127) : err[2] >= +254 ? ( err[2] = +127) : ( err[2] >>= 1));
	( *buf) += 3;
	
	return irgb[0];
}


inline uint8 dither_true ( uint8 * rgb, int16 * err, int8 ** buf)
{
	int8 	*dth = *buf;
	unsigned short  r    = (( err[0] += ( int16)dth[0] + rgb[0]) <= 42 ? 0 : err[0] >= 213 ? 5 : ( err[0] * 3) >> 7);
	unsigned short  g    = (( err[1] += ( int16)dth[1] + rgb[1]) <= 42 ? 0 : err[1] >= 213 ? 5 : ( err[1] * 3) >> 7);
	unsigned short  b    = (( err[2] += ( int16)dth[2] + rgb[2]) <= 42 ? 0 : err[2] >= 213 ? 5 : ( err[2] * 3) >> 7);
	uint8 * irgb = ( uint8*)&cube216[( r * 6 + g) * 6 + b];
	
	err[0] -= irgb[1];
	dth[0] =  ( err[0] <= -254 ? ( err[0] =- 127) : err[0] >= +254 ? ( err[0] =+ 127) : ( err[0] >>= 1));
	err[1] -= irgb[2];
	dth[1] =  ( err[1] <= -254 ? ( err[1] =- 127) : err[1] >= +254 ? ( err[1] =+ 127) : ( err[1] >>= 1));
	err[2] -= irgb[3];
	dth[2] =  ( err[2] <= -254 ? ( err[2] = -127) : err[2] >= +254 ? ( err[2] = +127) : ( err[2] >>= 1));
	( *buf) += 3;
	
	return irgb[0];
}


inline uint8 dither_gray( uint8 *gray, int16 *err, int8 **buf)
{
	int8 	*dth  		= *buf;
	unsigned short idx  = (( err[0] += ( int16)dth[0] + gray[0]) <= 0x07 ? 0 : err[0] >= 0xF8 ? 0x1F : err[0] >> 3);
	uint8 	*irgb 		= ( uint8*)&graymap[idx];
	
	err[0] -= irgb[1];
	dth[0] =  ( err[0] <= -254 ? ( err[0] = -127) : err[0] >= +254 ? ( err[0] =+ 127) : ( err[0] >>= 1));
	(*buf) += 1;
	
	return irgb[0];
}
