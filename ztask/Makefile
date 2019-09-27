USE_WINDOM2=1
top_srcdir=..
include $(top_srcdir)/configvars

STACKSIZE		= 64k
LIB			= $(WINDOM_LIBS) -lgem -lldg -lm #-lwout
CFLAGS			+= -I../common

OBJ 			= debug.o gmem.o prefs.o color_selector.o taskman.o pref_panel.o shutdown.o quit.o app.o process.o cpu.o string.o popup.o win.o main.o

PROGRAM 		= ztask.app

all:: $(PROGRAM)

$(PROGRAM): $(OBJ)
	$(CC) $(CPU_CFLAGS) $(OPTIMISATION) -o $@ $(OBJ) $(LDFLAGS) -Wl,--stack,$(STACKSIZE) $(LIB)
	: cp $(PROGRAM) _dist/$(PROGRAM)
	$(COMPRESS) $(PROGRAM)


clean::
	rm -f $(OBJ) $(PROGRAM)
