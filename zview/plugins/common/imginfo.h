#ifndef __ZVIEW_IMGINFO_H__
#define __ZVIEW_IMGINFO_H__

#if defined(__PUREC__) && !defined(_COMPILER_H)
/*
 * far from being a complete replacement for <stdint.h>,
 * but good enough for our purposes
 */
typedef signed char int8_t;
typedef unsigned char uint8_t;
typedef signed short int16_t;
typedef unsigned short uint16_t;
typedef signed long int32_t;
typedef unsigned long uint32_t;
typedef long ssize_t;
typedef long off_t;
#else
#include <stdint.h>
#endif


#define ST_RAM			0
#define TT_RAM			1

#ifndef JPEGLIB_H
typedef enum
{
	JCS_UNKNOWN,		/* error/unspecified */
	JCS_GRAYSCALE,		/* monochrome */
	JCS_RGB,			/* red/green/blue */
	JCS_YCbCr,			/* Y/Cb/Cr (also known as YUV) */
	JCS_CMYK,			/* C/M/Y/K */
	JCS_YCCK,			/* Y/Cb/Cr/K */
	JCS_BG_RGB,			/* big gamut red/green/blue, bg-sRGB */
	JCS_BG_YCC			/* big gamut Y/Cb/Cr, bg-sYCC */
} J_COLOR_SPACE;
#endif

#define UP_TO_DOWN 		0
#define DOWN_TO_UP		1

#ifndef TRUE
#define TRUE		1
#define FALSE		0
#endif

#ifndef __boolean_defined
#define __boolean_defined 1
#if defined(__PUREC__) || defined(__TURBOC__) || defined(__AHCC__) || defined(__MSHORT__)
/* because it is a return code from plugin functions */
typedef int32_t boolean;
#else
typedef int boolean;
#endif
#endif

#if defined(__PUREC__) || defined(__MSHORT__)
typedef long zv_int_t;
typedef unsigned long zv_uint_t;
#else
typedef int zv_int_t;
typedef unsigned int zv_uint_t;
#endif

#ifndef __GNUC__
#ifndef __attribute__
#define __attribute__(x)
#endif
#endif

typedef struct _img_data
{
	int16_t		imagecount;
	uint16_t	delay[1024];
	uint8_t		*image_buf[1024];
} img_data;


#include "txt_data.h"


typedef struct
{
	uint8_t	red;
	uint8_t	green;
	uint8_t	blue;
} COLOR_MAP;


typedef struct _img_info *IMGINFO;

typedef struct _img_info
{
	/* Data returned by the decoder Picture's initialisation */
	uint16_t	width;					/* Picture width 											*/
	uint16_t	height;					/* Picture height											*/
	uint16_t	components;  			/* Picture components 										*/
	uint16_t	planes;					/* Picture bits by pixel									*/
	uint32_t	colors;					/* Picture colors number									*/
	int16_t		orientation;			/* picture orientation: UP_TO_DOWN or DONW_TO_UP ( ex: TGA)	*/
	uint16_t	page;					/* Number of page/image in the file 						*/
	uint16_t	delay;					/* Animation delay in 200th sec. between each frame.		*/
	uint16_t	num_comments;			/* Lines in the file's comment. ( 0 if none) 	 			*/
	uint16_t	max_comments_length;	/* The length of the greater line in the comment 			*/
	int16_t		indexed_color;			/* Picture has a palette: 0) No 1) Yes						*/
	int16_t		memory_alloc;			/* where to do the memory allocation for the reader_read
										   function buffer, possible values are ST_RAM or TT_RAM	*/
	COLOR_MAP	palette[256];			/* Palette in standard RGB format							*/


	/* data send to the codec */
	uint32_t	background_color;		/* The wanted background's color in hexa */
	int16_t		thumbnail;				/* 1 if true else 0.
										   Some format have thumbnail inside the picture, if 1, we
										   want only this thumbnail and not the image itself 		*/
	uint16_t	page_wanted;			/* The wanted page to get for the reader_read() function
										   The first page is 0 and the last is img_info->page - 1	*/


	/* The following variable are only for Information Purpose, the zview doesn't
	   use it for decoding routine but only for the information dialog								*/
	char		info[40];				/* Picture's info, for ex: "Gif 87a" 						*/
	char		compression[5];			/* Compression type, ex: "LZW" 								*/
	uint16_t	real_width;				/* Real picture width( if the thumbnail variable is 1)		*/
	uint16_t	real_height;			/* Real picture height( if the thumbnail variable is 1)		*/


	/* Private data for the plugin */
	void		*_priv_ptr;
	void		*_priv_ptr_more;
	void		*__priv_ptr_more;

	int32_t		_priv_var;
	int32_t		_priv_var_more;
} img_info;

#endif /* __ZVIEW_IMGINFO_H__ */
