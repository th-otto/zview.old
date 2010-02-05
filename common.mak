ifeq ($(CROSS),yes)
AS			= m68k-atari-mint-as
CC                      = m68k-atari-mint-gcc
CXX			= m68k-atari-mint-g++
AR                      = m68k-atari-mint-ar
RANLIB                  = m68k-atari-mint-ranlib
STRIP                   = m68k-atari-mint-strip -s
STACK                   = m68k-atari-mint-stack
FLAGS                   = m68k-atari-mint-flags -r
else
AS			= as
CC                      = gcc
CXX			= g++
AR			= ar
RANLIB			= ranlib
STRIP                   = strip -s
STACK                   = stack
FLAGS                   = flags -r
endif
COMPRESS                = upx -9

all: $(PROGRAM) stack strip compress

$(PROGRAM): $(OBJ)
	$(CC) $(CPU) $(OPTIMISATION) -o $@ $(OBJ) $(LDFLAGS) $(LIB)

strip: $(PROGRAM)
	$(STRIP) $(PROGRAM)

stack: $(PROGRAM)
	$(STACK) -S $(STACKSIZE) $(PROGRAM)

compress: $(PROGRAM)
	$(COMPRESS) $(PROGRAM)
	$(FLAGS) $(PROGRAM)

clean:
	rm -f $(OBJ) $(PROGRAM) *~
