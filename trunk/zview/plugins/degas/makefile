CC				= gcc3
STRIP			= strip
STACK			= stack
STACKSIZE		= 8k
COMPRESS		= upx
OPTIMISATION	= -O2 -fomit-frame-pointer -finline-functions
CPU				= -m68000
INCLUDE_PATH	= /usr/GEM/include
LIB_PATH		= /usr/GEM/lib
LIB				= -lshare -lldg

CFLAGS			= $(CPU) $(OPTIMISATION) -Wall -I$(INCLUDE_PATH)

OBJ 			= degasldg.o

PROGRAM 		= degas.ldg


all: $(PROGRAM) strip stack compress

$(PROGRAM): $(OBJ)
	$(CC) -o$@ $(OBJ) -Xlinker -L$(LIB_PATH) $(LIB)

strip:    
	$(STRIP) $(PROGRAM)
    
stack:
	$(STACK) -S $(STACKSIZE) $(PROGRAM)

compress:
	$(COMPRESS) $(PROGRAM)

clean: 
	rm -f *.o *.ldg	 