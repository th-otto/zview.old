STACKSIZE		= 64k
OPTIMISATION		= -O2 -fomit-frame-pointer
CPU			= -m68020-60
LIB			= -ljpeg -lexif -lldgserver

CFLAGS			= $(CPU) $(OPTIMISATION) -Wall -I../common
LDFLAGS			= -nostdlib

DSP_OBJ			= jpgdsp.o 

OBJ			= $(LDG_STARTUP_OBJ) $(DSP_OBJ) jpgldg.o

PROGRAM 		= jpg.ldg

include ../../../common.mak
