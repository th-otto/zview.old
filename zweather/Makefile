CROSS			= no

ifeq ($(CROSS),yes)
CC			= m68k-atari-mint-gcc
STRIP			= m68k-atari-mint-strip
STACK			= m68k-atari-mint-stack
LIBXML			= /usr/m68k-atari-mint/include/libxml2
FLAGS			= m68k-atari-mint-flags -r
else
CC			= gcc
STRIP			= strip
STACK			= stack
LIBXML			= /usr/include/libxml2
FLAGS			= flags -r
endif
COMPRESS		= upx -9

STACKSIZE		= 64k
OPTIMISATION		= -O2 -fomit-frame-pointer -finline-functions
CPU			= -m68020-60
LIB			= -lwindom1 -lzcodec -lldg -lgem -lxml2 -lm -lmem -lz
CFLAGS			= $(CPU) $(OPTIMISATION) -Wall -Wshadow -I$(LIBXML)
ifeq ($(CROSS),no)
CFLAGS			+= -I/usr/local/include
LDFLAGS			+= -L/usr/local/lib/m68020-60
endif
OBJ 			= inet.o http_client.o parsers.o get_data.o \
			  debug.o zstring.o dialog.o prefs.o zaes.o icon.o main.o

PROGRAM 		= zweather.app


all: $(PROGRAM) stack strip compress

$(PROGRAM): $(OBJ)
	$(CC) $(CPU) $(OPTIMISATION) -o $@ $(OBJ) $(LDFLAGS) $(LIB)

strip:
	$(STRIP) -s $(PROGRAM)

stack:
	$(STACK) -S $(STACKSIZE) $(PROGRAM)

compress:
	$(COMPRESS) $(PROGRAM)
	$(FLAGS) $(PROGRAM)
	
clean:
	rm -f *.o *.app *~
