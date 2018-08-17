/*
 * WinDom: a high level GEM library
 * Copyright (c) 1997-2006 windom authors (see AUTHORS file)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 *
 * $Source: /cvsroot/windom/windom/include/scancode.h,v $
 *
 * CVS info:
 *   $Author: bercegeay $
 *   $Date: 2006/01/03 14:46:23 $
 *   $Revision: 1.4 $
 */


#ifndef __SCANCODE__
#define __SCANCODE__

/* Main keyboard */
#define SC_ESC  	0x01
#define SC_BACK		0x0E
#define SC_TAB		0x0F
#define SC_RETURN	0x1C
#define SC_DEL		0x53
#define SC_SPACE	0x39

/* Move keyboard */
#define SC_HELP		0x62
#define SC_UNDO 	0x61
#define SC_INSERT	0x52
#define SC_HOME		0x47
#define SC_UPARW	0x48
#define SC_LFARW	0x4B
#define SC_DNARW	0x50
#define SC_DWARW SC_DNARW
#define SC_RTARW	0x4D

/* Numeric keyboard */
#define	SC_LFPAR	0x63	
#define SC_RTPAR	0x64
#define SC_SLASH	0x65
#define SC_STAR		0x66
#define SC_SEVEN	0x67
#define SC_EIGHT	0x68
#define SC_NINE		0x69
#define SC_MINUS	0x4A
#define SC_FOUR		0x6A
#define SC_FIVE		0x6B
#define SC_SIX		0x6C
#define SC_PLUS		0x4E
#define SC_ONE		0x6D
#define SC_TWO		0x6E
#define SC_THREE	0x6F
#define SC_ENTER	0x72
#define SC_ZERO		0x70
#define SC_POINT	0x71

/* Available with MagiC Mac and Magic PC */
#define SC_PGUP		0x49
#define SC_PGDN		0x51

/* Function keys */
#define SC_F1		0x3B
#define SC_F2		0x3C
#define SC_F3		0x3D
#define SC_F4		0x3E
#define SC_F5		0x3F
#define SC_F6		0x40
#define SC_F7		0x41
#define SC_F8		0x42
#define SC_F9		0x43
#define SC_F10		0x44
#define SC_F11		0x54
#define SC_F12		0x55
#define SC_F13		0x56
#define SC_F14		0x57
#define SC_F15		0x58
#define SC_F16		0x59
#define SC_F17		0x5A
#define SC_F18		0x5B
#define SC_F19		0x5C
#define SC_F20		0x5D

/* 
 * The previous definitions address the non ASCII
 * keys of the Atari keyboard. We use only the first
 * byte of scancodes (containing 2 bytes) because the
 * the second one is always 0 (non ASCII keys).
 * 
 * Other keys have a different scancode depending on
 * the keyboard (country) type because a scancode identify
 * a key location on a keyboard and not an ASCII value.
 * For example, the key 'a' have as scancode 0x1E61 on
 * english keyboard and 0x1061 on french keyboard
 * ('a' is 0x61 in ASCII code).
 *
 * WinDom provides a function, keybd2ascii(), which can
 * identify ascii value of a scancode (occuring with a
 * keyboard event for example) even the control and
 * alternate keys are depressed. It is very usefull to
 * shortcut keys working on any type of keyboard.
 */

#endif
