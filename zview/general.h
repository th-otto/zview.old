/*
 * zView.
 * Copyright (c) 2004-2005 Zorro ( zorro270@yahoo.fr)
 *
 * This application is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This application is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "types2b.h"
#include <osbind.h>
#include <mintbind.h>
#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <windom1.h>
#include <scancode.h>
#include <ldg.h>
#include <ldg/mem.h>
#include "mem.h"
#include "zaes.h"
#include "zedit/libtedit.h"
#include "pdf/goo/gmem.h"
#include "zview.rsh"

/* Window's Attrib. */
#define WINATTRIB  MOVER|NAME|VSLIDE|MOVER|CLOSER|DNARROW|UPARROW|SIZER|SMALLER|FULLER  

/* Custom data name */
#define WD_ICON		0x49434F4EL
#define WD_WIMG		0x57494D47L

/* Custom error code */
#define E_NAMEEXIST	10001
#define E_RSC		10002
#define NO_EDDI		10003
#define NO_ICON		10004

/* custom def */
#define preview_mode	1
#define full_size		0
#define ON		 		1
#define OFF		 		0
#define UNKNOWN		   -1

/* xfont text attrib. */
#define BOLD		0x01
#define LIGHT		0x02
#define ITALIC		0x04
#define ULINE		0x08
#define INVERSE		0x10
#define SHADOW		0x20
#define MONOSPACE	0x40


#ifndef MAXNAMLEN
	#define MAXNAMLEN	255
#endif

#ifndef MAX_PATH
	#define MAX_PATH	1024
#endif

#define shared_malloc( s)		_ldg_malloc( s, ldg_global[2]);
#define shared_realloc( s, b)	_ldg_realloc( s, b, ldg_global[2]);
#define shared_free( s)			_ldg_free( s, ldg_global[2]);

/* MACRO */
#ifndef ABS
	#define ABS(val) ( ( ( val) < 0) ? -( val) : ( val))
#endif

#ifndef XATTR
typedef struct
{
	uint16	mode;
	int32	index;
	uint16	dev;
	uint16	reserved1;
	uint16	nlink;
	uint16	uid;
	uint16	gid;
	int32	size;
	int32	blksize;
	int32	nblocks;
	int16	mtime;
	int16	mdate;
	int16	atime;
	int16	adate;
	int16	ctime;
	int16	cdate;
	int16	attr;
	int16	reserved2;
	int32	reserved3;
	int32	reserved4;
} XATTR;
#endif

typedef struct
{
	uint32	item;
	uint32	size;
} fileinfo;

typedef enum 
{
	ET_DIR,
	ET_FILE,
	ET_IMAGE,
	ET_PDF,
	ET_PRG
} EntryType;


typedef struct _txt_data
{
	int16		lines;					/* Number of lines in the text 						*/
	int16		max_lines_length;		/* The length of the greater line ( in nbr char)	*/
	int8    	*txt[256];
} txt_data;


typedef struct
{
	/* Image Info */
	uint16  	img_w, img_h; 		/* original size													*/
	int16		bits;				/* image's colors bits 												*/
	int8		working_time[12];	/* ex: 15,2s														*/
	uint32		colors;				/* Picture colors's number 											*/
	int8		info[38];			/* Information about the image, for ex: "Portable Network Graphic"	*/
	int8		compression[5];		/* Compression type, ex: "LZW" 										*/
	int16		page;				/* Number of page/image in the file 								*/
	uint16		delay[1024];		/* Animation delay in millise. between each frame					*/

	txt_data	*comments;	

	/* Private Data */
	int16		view_mode;			/* preview or full size view? 			*/
	int16		progress_bar;		/* display the progress bar? 			*/

	/* Private data for the PDF */
	void		*_priv_ptr;			
	void		*_priv_ptr_more;			
	/* ------------------------ */ 

	MFDB 		*image;				/* The Image itself						*/
}IMAGE;


typedef struct _entry 
{
	int8 			name[MAXNAMLEN];		/* The real file's name	 					*/
	int8 			name_shown[100];		/* The file's name shown in the window		*/

	struct stat		stat;					/* Entry's Unix Stat						*/

	int8 			size[15];				/* File size in human readable format		*/	
  	int8           	date[28];         		/* date of last modification     			*/
  	int8           	time[12];         		/* time of last modification     			*/

	int16 			icon_txt_w;				/* Text width for screen output	 			*/

	RECT16 			txt_pos;				/* text position in the window	 			*/
	RECT16 			icn_pos;				/* icon position in the window				*/
	RECT16 			case_pos;				/* case position in the window				*/

	EntryType		type;					/* icon type 								*/

	IMAGE			preview;				/* the preview data and picture				*/

	MFDB	 		*icon;					/* icon										*/

	struct _entry	*next_selected;
	struct _entry	*next;

} Entry;



typedef struct _mini
{
	char 			foldername[MAX_PATH + MAXNAMLEN];	/*  Folder name with the full path		*/
	char 			name[MAXNAMLEN];					/*  Folder's name shown in the window	*/

	RECT16 			icon_position;						/* text position in the window			*/
	RECT16 			arrow_position;						/* icon position in the window			*/

	struct 	_mini	*parent;							/* the parent entry						*/
	struct 	_mini	*child;								/* the child(s) entry					*/

	int16 			nbr_child;							/*  number of valid child's entries		*/
	int16 			icon_txt_w;							/*  Text width for screen output		*/
	int16			state;								/*  Deployed or not ( ON/OFF)			*/
} Mini_Entry;




typedef struct
{
	char			directory[MAX_PATH];	/* current directory							*/

	/* File browser data */
	int16 			border_position[2];		/* frame's border position in the 'x' axe	*/
	int16 			case_w;					/* case width								*/
	int16 			case_h;					/* case height 								*/
	int16 			columns;				/* number of columns 						*/	
	int16 			icons_last_line; 		/* number of icons in the last line			*/
	EDIT 			*edit;					/* Text struct for the edition mode			*/
	Entry			*first_selected;		/* Pointer to the first icon selected		*/
	Entry 			*entry;					/* pointer to 'Entry' structur 				*/
	int16 			nbr_icons;				/* number of valid icons  					*/


	/* "Tree view" data */
	Mini_Entry		*mini_selected;			/* Pointer to 'Mini_Entry' structur			*/
	Mini_Entry		*root;					/* Pointer to 'Mini_Entry' structur			*/
	int16 			nbr_child;				/* Number of valid child's entries			*/
	int32          	ypos;   	 		 	/* relative data position in the window 	*/
	int32          	ypos_max;	 			/* Maximal values of previous variables 	*/
	int16         	h_u;    	   			/* vertical and horizontal scroll offset	*/

} WINDICON;


extern int16 	pic_load( const char *file, char *extention, IMAGE *img);
extern void 	zdebug( const char *format, ...);
extern void 	applexit( void);

/* Globals variables */
extern WINDOW 	*win_catalog;
extern WINDOW 	*win_image;
extern char 	zview_path[MAX_PATH];
extern int16	mint_os, magic_os;

/* Windom's function not defined in windom.h */
extern void	frm_cls( WINDOW *win);
extern void snd_rdw( WINDOW *win);
extern void snd_msg( WINDOW *win, int msg, int par1, int par2, int par3, int par4);
extern void draw_page( WINDOW *win, int x, int y, int w, int h);
