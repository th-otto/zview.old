STACKSIZE		= 64k
OPTIMISATION		= -O2 -fomit-frame-pointer -finline-functions
CPU			= -m68020-60
LIB			= -ltiff -ljpeg -lz -lldgserver -lgcc

CFLAGS			= $(CPU) $(OPTIMISATION) -Wall -I. -I../common
LDFLAGS			= -nostdlib

OBJ			= $(LDG_STARTUP_OBJ) tiffldg.o

PROGRAM 		= tiff.ldg

include ../../../common.mak
