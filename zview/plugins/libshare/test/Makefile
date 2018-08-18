#  makefile for libshare/test.ldg:
#  by Arnaud BERCEGEAY (Feb 2004)

include ../../../../configvars

LIBOBJ = libtest.o
PRGOBJ = prgtest.o

LIBTARGET = test.ldg 
PRGTARGET = test.tos

all: $(LIBTARGET) $(PRGTARGET)

$(LIBTARGET): $(LIBOBJ)
	$(CC) -o $@ $(LIBOBJ) ../libshare.a -lldg -lgem

$(PRGTARGET): $(PRGOBJ)
	$(CC) -o $@ $(PRGOBJ) -lldg -lgem

clean:
	rm -f *.o $(LIBTARGET) $(PRGTARGET)
