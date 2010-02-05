STACKSIZE		= 16k
OPTIMISATION		= -O2 -fomit-frame-pointer
CPU			= -m68020-60
LIB			=  -lm -lshare -lldg -lz

CFLAGS			= $(CPU) $(OPTIMISATION) -Wall

MEM_OBJ			= mem/mem_util.o

PNG_OBJ			= libpng/png.o libpng/pngset.o libpng/pngget.o libpng/pngrutil.o \
			  libpng/pngtrans.o libpng/pngread.o libpng/pngerror.o libpng/pngrtran.o \
    	   		  libpng/pngmem.o libpng/pngrio.o libpng/pngpread.o  

OBJ			=  $(PNG_OBJ) $(MEM_OBJ) pngldg.o

PROGRAM 		= png.ldg

include ../../../common.mak
