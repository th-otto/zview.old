ifeq ($(CROSS),yes)
LIBXML			= /usr/m68k-atari-mint/include/libxml2
else
LIBXML			= /usr/include/libxml2
endif

STACKSIZE		= 64k
OPTIMISATION		= -O2 -fomit-frame-pointer -finline-functions
CPU			= -m68020-60
LIB			= -lwindom1 -lzcodec -lldg -lgem -lxml2 -lm -lmem -lz
CFLAGS			= $(CPU) $(OPTIMISATION) -Wall -Wshadow -I$(LIBXML)
ifeq ($(CROSS),no)
CFLAGS			+= -I/usr/local/include
LDFLAGS			+= -L/usr/local/lib/m68020-60
endif
OBJ 			= inet.o http_client.o parsers.o get_data.o \
			  debug.o zstring.o dialog.o prefs.o zaes.o icon.o main.o

PROGRAM 		= zweather.app

include ../common.mak