CC				= gcc3
STRIP			= strip
STACK			= stack
STACKSIZE		= 16k
COMPRESS		= upx
OPTIMISATION	= -O2 -fomit-frame-pointer
CPU				= -m68020-60
INCLUDE_PATH	= /usr/GEM/include
LIB_PATH		= /usr/GEM/lib
LIB				=  -lm2060 -lshare -lldg

CFLAGS			= $(CPU) $(OPTIMISATION) -Wall -I$(INCLUDE_PATH)

MEM_OBJ			= mem/mem_util.o

Z_OBJ			= zlib/adler32.o zlib/crc32.o zlib/deflate.o zlib/inffast.o \
				  zlib/inflate.o zlib/infback.o  zlib/inftrees.o zlib/trees.o \
    	   		  zlib/uncompr.o zlib/zutil.o  

PNG_OBJ			= libpng/png.o libpng/pngset.o libpng/pngget.o libpng/pngrutil.o \
				  libpng/pngtrans.o libpng/pngread.o libpng/pngerror.o libpng/pngrtran.o \
    	   		  libpng/pngmem.o libpng/pngrio.o libpng/pngpread.o  

OBJ				=  $(Z_OBJ) $(PNG_OBJ) $(MEM_OBJ) pngldg.o

PROGRAM 		= png.ldg


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
	rm -f *.o mem/*.o zlib/*.o libpng/*.o *.ldg	 
