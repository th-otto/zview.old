STACKSIZE		= 64k
OPTIMISATION		= -O2 -fomit-frame-pointer -finline-functions
CPU			= -m68020-60
LIB			= -lldgserver

CFLAGS			= $(CPU) $(OPTIMISATION) -Wall -I../common
LDFLAGS			= -nostdlib

OBJ 			= $(LDG_STARTUP_OBJ) imgldg.o xp32.o

PROGRAM 		= img.ldg

include ../../../common.mak
