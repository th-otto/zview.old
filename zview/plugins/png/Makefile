STACKSIZE		= 16k
OPTIMISATION		= -O2 -fomit-frame-pointer
CPU			= -m68020-60
LIB			=  -lshare -lldg -lpng -lm -lz

CFLAGS			= $(CPU) $(OPTIMISATION) -Wall -I../common

MEM_OBJ			= mem/mem_util.o

PNG_OBJ			= libpng/png.o libpng/pngset.o libpng/pngget.o libpng/pngrutil.o \
			  libpng/pngtrans.o libpng/pngread.o libpng/pngerror.o libpng/pngrtran.o \
    	   		  libpng/pngmem.o libpng/pngrio.o libpng/pngpread.o

OBJ			=  $(MEM_OBJ) pngldg.o

PROGRAM 		= png.ldg

include ../../../common.mak
