LIBXML2_INC		= $$HOME/gnu-tools/m68k-atari-mint/include/libxml2

STACKSIZE		= 64k
OPTIMISATION		= -O2 -fomit-frame-pointer -finline-functions
CPU			= -m68020-60
LIB			= -lwindom -lzcodec -lldg -lgem -lxml2 -lm -lz
CFLAGS			= $(CPU) $(OPTIMISATION) -Wall -Wshadow -I$(LIBXML2_INC) -I../common
OBJ 			= inet.o http_client.o parsers.o get_data.o \
			  debug.o zstring.o dialog.o prefs.o zaes.o icon.o main.o

PROGRAM 		= zweather.app

include ../common.mak
