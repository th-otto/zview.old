include ../../../configvars

LIB			= ../libshare/libshare.a -lldg

CFLAGS			+= -I../../../common -I../common

OBJ 			= tgaldg.o

PROGRAM 		= tga.ldg

all:: $(PROGRAM)

$(PROGRAM): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ) $(LDFLAGS) -Wl,--stack,64k $(LIB)

clean:
	rm -f *.o *.a *.ldg
