CC				= gcc3
STRIP			= strip
STACK			= stack
STACKSIZE		= 4k
OPTIMISATION	= -O3 -fomit-frame-pointer -finline-functions
CPU				= -m68020-60
INCLUDE_PATH	= /usr/GEM/include
LIB_PATH		= /usr/GEM/lib
LIB				= -lshare -lldg

CFLAGS			= $(CPU) $(OPTIMISATION) -Wall -I$(INCLUDE_PATH)

OBJ 			= neoldg.o

PROGRAM 		= neo.ldg


all: $(PROGRAM) strip stack

$(PROGRAM): $(OBJ)
	$(CC) -o$@ $(OBJ) -Xlinker -L$(LIB_PATH) $(LIB)

strip:    
	$(STRIP) $(PROGRAM)
    
stack:
	$(STACK) -S $(STACKSIZE) $(PROGRAM)

clean: 
	rm -f *.o	 
