include ../../../configvars

LIB			= ../libshare/libshare.a -lldg -lpng -lm -lz

CFLAGS			+= -I../../../common -I../common

OBJ			= pngldg.o

PROGRAM 		= png.ldg

all:: $(PROGRAM)

$(PROGRAM): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ) $(LDFLAGS) -Wl,--stack,64k $(LIB)

clean:
	rm -f *.o *.a *.ldg
