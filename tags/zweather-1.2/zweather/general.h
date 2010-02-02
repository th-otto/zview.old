/*
 * zView.
 * Copyright (c) 2004 Zorro ( zorro270@yahoo.fr)
 *
 * This application is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This application is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 */
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
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
#ifdef DEBUG
#include <wout.h>
#endif
#include <libxml/parser.h>
#include <types2b.h>
#include <zcodec.h>
#include "zstring.h"
#include "zaes.h"
#include "prefs.h"
#include "zweather.rsh"

/* Custom message code */
#define E_RSC	10002
#define E_OK 	0

/* Option */
// #define DEBUG


#ifndef MAXNAMLEN
	#define MAXNAMLEN	255
#endif

#ifndef MAX_PATH
	#define MAX_PATH	1024
#endif

extern WINDOW *windialog;
extern void timer_function( WINDOW *win);
extern void	applexit( void);
extern void snd_rdw( WINDOW *win);
extern struct xml_weather *update_weatherdata( int8 *location);
extern void snd_msg( WINDOW *win, int msg, int par1, int par2, int par3, int par4);
extern void zdebug( const char *format, ...);

