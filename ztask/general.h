/*
 * zbar.
 * Copyright (c) 2005 Zorro ( zorro270@yahoo.fr)
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
#include <osbind.h>
#include <mintbind.h>
#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#ifdef USE_WINDOM2
#include <windom.h>
#else
#include <windom1.h>
#endif
#include <ldg.h>
#include <scancode.h>
#include "types2b.h"
#include "prefs.h"
#include "ztask.rsh"

/* Option */

#ifndef MAXNAMLEN
	#define MAXNAMLEN	255
#endif

#ifndef MAX_PATH
	#define MAX_PATH	1024
#endif

/* xfont text attrib. */
#define BOLD		0x01
#define LIGHT		0x02
#define ITALIC		0x04
#define ULINE		0x08
#define INVERSE		0x10
#define SHADOW		0x20
#define MONOSPACE	0x40

#define	POPUP_ITEM	64

#define BOLD		0x01
#define LIGHT		0x02
#define ITALIC		0x04
#define ULINE		0x08
#define INVERSE		0x10
#define SHADOW		0x20
#define MONOSPACE	0x40

#define F_HIDE			1
#define F_QUIT			2
#define F_KILL			3
#define F_TASKMANAGER	4
#define F_PREFERENCES	5
#define F_SHUTDOWN		6
#define F_QUIT_ZBAR		7

typedef struct _app_data
{
	int16				id, pid;
	char				name[9];
	char				name_shown[MAXNAMLEN];
	int16				name_shown_width;	
	int16				x_pos, y_pos;
	struct _app_data	*next;
} app_data;


typedef struct
{
	char		item_name[POPUP_ITEM][64];
	RECT16		item_pos[POPUP_ITEM];
	int16		icon[POPUP_ITEM];		
	int16		selectable[POPUP_ITEM];
	int16		function[POPUP_ITEM];
	int16		item_nbr;
	int16		selected, old_selected;	
	int16		x_pos, y_pos, w_pos, h_pos;
	app_data	*entry;	
	WINDOW  	*win;
} popup_data;


typedef struct _process
{
	int					pid;
	char				cpu_usage[5];
	char				ram_usage[30];
	char				name[MAXNAMLEN];
	uint32				cpu_time;
	int16				ram_usage_txt_width;	
	int16				y_pos;
	struct _process		*next;
} process;


void 	zdebug( const char *format, ...);
OBJECT	*get_tree( int16 obj_index); 
char		*get_string( int16 str_index);
void 	shutdown_dialog( void);
void 	quit_dialog( void);
void 	pref_dialog( void);
void vdi_color_selector( int16 xw, int16 yw, int16 color_selected, int16 parent_object);
