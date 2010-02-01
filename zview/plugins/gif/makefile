CC				= m68k-atari-mint-gcc
STACK			= m68k-atari-mint-stack
STACKSIZE		= 8k
OPTIMISATION	= -O2 -fomit-frame-pointer
CPU				= -m68020-60
INCLUDE_PATH	= /home/zorro/cross-tools/include
LIB_PATH		= /home/zorro/cross-tools/lib
LIB				= -lshare -lldg

CFLAGS			= $(CPU) $(OPTIMISATION) -Wall -I$(INCLUDE_PATH)

OBJ 			= gifldg.o gif_hash.o gifalloc.o dgif_lib.o egif_lib.o

PROGRAM 		= gif.ldg


all: $(PROGRAM) stack

$(PROGRAM): $(OBJ)
	$(CC) -o$@ $(OBJ) -s -L$(LIB_PATH) $(LIB)
    
stack:
	$(STACK) -S $(STACKSIZE) $(PROGRAM)

compress:
	$(COMPRESS) $(PROGRAM)

clean: 
	rm -f *.o *.ldg	 
	