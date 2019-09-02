/*
 * zWeather.
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
#include <scancode.h>
#include <ldg.h>
#include <libxml/parser.h>
#include "types2b.h"
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

#undef _AESapid
#if __WINDOM_MAJOR__ >= 2
#define WIN_GRAF_HANDLE(win) (win)->graf->handle
#define APP_GRAF_HANDLE app.graf.handle
#define WIN_GRAF_CLIP(win) &(win)->graf->clip
#define WinClipOn(win, r) ClipOn((win)->graf, r)
#define WinClipOff(win) ClipOff((win)->graf)
#define _AESapid mt_AppAESapid(gl_appvar)
#define EVNT_BUFF_PARAM , short buff[8]
#define EVNT_BUFF_ARG , buff
#define EVNT_BUFF_NULL , NULL
#define EVNT_BUFF buff
#else
#define WIN_GRAF_HANDLE(win) (win)->graf.handle
#define APP_GRAF_HANDLE app.handle
#define WIN_GRAF_CLIP(win) &clip
#define WinClipOn(win, r) rc_clip_on(WIN_GRAF_HANDLE(win), r)
#define WinClipOff(win) rc_clip_off(WIN_GRAF_HANDLE(win))
#define _AESapid app.id
#define EVNT_BUFF_PARAM
#define EVNT_BUFF_ARG
#define EVNT_BUFF_NULL
#define EVNT_BUFF evnt.buff

#define ObjcAttachFormFunc(win, index, func, data) ObjcAttach(OC_FORM, win, index, BIND_FUNC, func)
#define ObjcAttachMenuFunc(win, index, func, data) ObjcAttach(OC_MENU, win, index, BIND_FUNC, func)
#define ObjcAttachVar(mode, win, index, var, val) ObjcAttach(OC_FORM, win, index, BIND_VAR, var)

#endif

extern int	use_sting;
extern WINDOW *windialog;
extern void timer_function( WINDOW *win, short buff[8]);
extern void snd_rdw( WINDOW *win);
extern struct xml_weather *update_weatherdata( int8 *location);
extern void snd_msg( WINDOW *win, int msg, int par1, int par2, int par3, int par4);
extern void zdebug( const char *format, ...);

