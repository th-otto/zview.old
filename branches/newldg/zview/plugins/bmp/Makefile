STACKSIZE		= 64k
OPTIMISATION		= -O2 -fomit-frame-pointer -finline-functions
CPU			= -m68020-60
LIB			= -lldgserver

CFLAGS			= $(CPU) $(OPTIMISATION) -Wall -I../common
LDFLAGS			= -nostdlib

OBJ 			= $(LDG_STARTUP_OBJ) bmpldg.o

PROGRAM 		= bmp.ldg

include ../../../common.mak
