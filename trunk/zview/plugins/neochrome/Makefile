STACKSIZE		= 64k
OPTIMISATION		= -O2 -fomit-frame-pointer -finline-functions
CPU			= -m68020-60
LIB			= -lldgserver

CFLAGS			= $(CPU) $(OPTIMISATION) -Wall -I../common
LDFLAGS			= -nostdlib

OBJ 			= $(LDG_STARTUP_OBJ) neoldg.o

PROGRAM 		= neo.ldg

include ../../../common.mak
