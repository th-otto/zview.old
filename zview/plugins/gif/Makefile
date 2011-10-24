STACKSIZE		= 8k
OPTIMISATION		= -O2 -fomit-frame-pointer
CPU			= -m68020-60
LIB			= -lshare -lldg

CFLAGS			= $(CPU) $(OPTIMISATION) -Wall -I../common

OBJ 			= gifldg.o gif_hash.o gifalloc.o dgif_lib.o egif_lib.o

PROGRAM 		= gif.ldg

include ../../../common.mak
