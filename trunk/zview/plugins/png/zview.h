/*
 * PNG LDG plugin.
 * Copyright (c) 2004-2005 Zorro ( zorro270@yahoo.fr)
 *
 * This plugin is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This plugin is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <setjmp.h>
#ifdef __PUREC__
#include <tos.h>
#include "libpng/png.h"
#include "types2b.h"
#else
#include <osbind.h>
#include <mintbind.h>
#include <libshare.h>
#include "libpng/png.h"
#include "types2b.h"
#include "mem/mem_util.h"
#endif
#include <mt_gem.h>
#include <ldg.h>

#ifndef TRUE
#define TRUE		1
#define FALSE		0
#endif

#ifndef MIN
#define MAX(a,b) ((a)>(b)?(a):(b))
#define MIN(a,b) ((a)<(b)?(a):(b))
#endif

#define UP_TO_DOWN 		0
#define DOWN_TO_UP		1
#define ST_RAM			0
#define TT_RAM			1

typedef struct _txt_data
{
	int16		lines;					/* Number of lines in the text 						*/
	int16		max_lines_length;		/* The length of the greater line ( in nbr char)	*/
	int8    	*txt[256];
} txt_data;


typedef struct
{
	uint8	red;
	uint8	green;
	uint8	blue;
} COLOR_MAP;


typedef struct _img_info *IMGINFO;

typedef struct _img_info
{
	/* Data returned by the decoder Picture's initialisation */
	uint16    	width;					/* Picture width 											*/
	uint16		height;					/* Picture Height											*/
	uint16 		components;  			/* Picture components 										*/
	uint16	 	planes;					/* Picture bits by pixel									*/
	uint32     	colors;					/* Picture colors number									*/
	int16		orientation;			/* picture orientation: UP_TO_DOWN or DONW_TO_UP ( ex: TGA)	*/
	uint16		page;					/* Number of page/image in the file 						*/
	uint16		delay;					/* Animation delay in 200th sec. between each frame.		*/
	uint16		num_comments;			/* Lines in the file's comment. ( 0 if none) 	 			*/
	uint16		max_comments_length;	/* The length of the greater line in the comment 			*/
	int16		indexed_color;			/* Picture has a palette: 0) No 1) Yes						*/
	int16		memory_alloc;			/* where to do the memory allocation for the reader_read 
										   function buffer, possible values are ST_RAM or TT_RAM	*/
	COLOR_MAP	palette[256];			/* Palette in standard RGB format							*/


	/* data send to the codec */
	uint32		background_color;		/* The wanted background's color in hexa */		
	int16		thumbnail;				/* 1 if true else 0.
										   Some format have thumbnail inside the picture, if 1, we 
										   want only this thumbnail and not the image itself 		*/
	uint16		page_wanted;			/* The wanted page to get for the reader_read() function 											   
										   The first page is 0 and the last is img_info->page - 1	*/											   


	/* The following variable are only for Information Prupose, the zview doesn't 
	   use it for decoding routine but only for the information dialog								*/ 
	int8		info[40];				/* Picture's info, for ex: "Gif 87a" 						*/
	int8		compression[5];			/* Compression type, ex: "LZW" 								*/
	uint16    	real_width;				/* Real picture width( if the thumbnail variable is 1)		*/
	uint16		real_height;			/* Ral picture Height( if the thumbnail variable is 1)		*/


	/* Private data for the plugin */
	void		*_priv_ptr;			
	void		*_priv_ptr_more;			
	void		*__priv_ptr_more;

	int32		_priv_var;
	int32		_priv_var_more;
} img_info;


