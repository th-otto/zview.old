include ../../../configvars

LIB			= ../libshare/libshare.a -lldg

CFLAGS			+= -I../../../common -I../common

OBJ 			= bmpldg.o

PROGRAM 		= bmp.ldg

all:: $(PROGRAM)

$(PROGRAM): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ) $(LDFLAGS) -Wl,--stack,64k $(LIB)

clean:
	rm -f *.o *.a *.ldg
