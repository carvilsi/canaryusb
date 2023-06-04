CC = gcc
LIBS = -ludev
CCFLAGS =

all: executable

debug: CCFLAGS += -DDEBUG 
debug: executable
	
executable:
	$(CC) $(CCFLAGS) -o canaryusb canaryusb.c base32.c trusted_list.c util.c canaries.c usbs.c $(LIBS)
