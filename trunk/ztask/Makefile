DEBUG			= # -DDEBUG
STACKSIZE		= 64k
OPTIMISATION		= -O2 -fomit-frame-pointer
CPU			= -m68020-60
LIB			= -lwindom -lgem -lldg -lm #-lwout
CFLAGS			= $(CPU) $(OPTIMISATION) $(DEBUG) -Wall -Wshadow -I../common

OBJ 			= debug.o gmem.o prefs.o color_selector.o taskman.o pref_panel.o shutdown.o quit.o app.o process.o cpu.o string.o popup.o win.o main.o

PROGRAM 		= ztask.app

include ../common.mak
