STACKSIZE		= 64k
OPTIMISATION		= -O2 -fomit-frame-pointer
CPU			= -m68020-60
LIB			= -lldg -ljpeg -lexif -lm

CFLAGS			= $(CPU) $(OPTIMISATION) -Wall -I../common

DSP_OBJ			= jpgdsp.o 

OBJ			= $(DSP_OBJ) jpgldg.o 

PROGRAM 		= jpg.ldg

include ../../../common.mak
