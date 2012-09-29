STACKSIZE		= 64k
OPTIMISATION		= -O2 -fomit-frame-pointer
CPU			= -m68020-60
LIB			= -lgif -lldgserver

CFLAGS			= $(CPU) $(OPTIMISATION) -Wall -I../common
LDFLAGS			= -nostdlib

OBJ 			= $(LDG_STARTUP_OBJ) gifldg.o

PROGRAM 		= gif.ldg

include ../../../common.mak
