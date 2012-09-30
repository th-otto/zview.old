STACKSIZE		= 64k
OPTIMISATION		= -O2 -fomit-frame-pointer
CPU			= -m68020-60
LIB			= -lshare -lldg

CFLAGS			= $(CPU) $(OPTIMISATION) -Wall -I../common

OBJ 			= gifldg.o

LIB			+= -lgif

PROGRAM 		= gif.ldg

include ../../../common.mak
