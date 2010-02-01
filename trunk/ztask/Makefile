ifeq ($(CROSS),yes)
CC			= m68k-atari-mint-gcc
STACK			= m68k-atari-mint-stack
STRIP			= m68k-atari-mint-strip
else
CC			= gcc
STACK			= stack
STRIP			= strip
endif

DEBUG			= # -DDEBUG
STACKSIZE		= 64k
OPTIMISATION		= -O2 -fomit-frame-pointer
CPU			= -m68020-60
LIB			= -lwindom -lgem -lldg -lm #-lwout
CFLAGS			= $(CPU) $(OPTIMISATION) $(DEBUG) -Wall -Wshadow

OBJ 			= debug.o gmem.o prefs.o color_selector.o taskman.o pref_panel.o shutdown.o quit.o app.o process.o cpu.o string.o popup.o win.o main.o

PROGRAM 		= ztask.app


all: $(PROGRAM) stack strip

$(PROGRAM): $(OBJ)
	$(CC) $(CPU) $(OPTIMISATION) -o$@ $(OBJ) $(LIB)

stack:
	$(STACK) -S $(STACKSIZE) $(PROGRAM)
	
strip:
	$(STRIP) -s $(PROGRAM)
        
clean:
	rm -f *.o *.app *~
