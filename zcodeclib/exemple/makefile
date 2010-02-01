ifeq ($(CROSS),yes)
AS					= m68k-atari-mint-as
CC					= m68k-atari-mint-gcc
STACK				= m68k-atari-mint-stack
INCLUDE_PATH	= /home/zorro/cross-tools/include
LIB_PATH		= /home/zorro/cross-tools/lib
else
AS					= as
CC					= gcc
STACK				= stack
INCLUDE_PATH	= /usr/GEM/include
LIB_PATH		= /usr/GEM/lib
endif

STACKSIZE		= 128k
COMPRESS		= upx
OPTIMISATION	= -O2 -fomit-frame-pointer -finline-functions
CPU				= -m68020-60
LIB				= -lzcodec -lgem -lldg

CFLAGS			= $(CPU) $(OPTIMISATION) -Wall -I$(INCLUDE_PATH)

OBJ 			= main.o

PROGRAM			= exemple.app


all: $(PROGRAM) stack #compress

$(PROGRAM): $(OBJ)
	$(CC) -o$@ $(OBJ) -Xlinker -s -L$(LIB_PATH) $(LIB)
    
stack:
	$(STACK) -S $(STACKSIZE) $(PROGRAM)

compress:
	$(COMPRESS) $(PROGRAM)
	
clean:
	rm -f *.o

