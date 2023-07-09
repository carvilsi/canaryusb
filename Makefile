CC = gcc
LIBS = -ludev
CCFLAGS =

all: executable

debug: CCFLAGS += -DDEBUG 
debug: executable
	
executable:
	$(CC) $(CCFLAGS) -o canaryusb src/canaryusb.c src/base32.c src/trusted_list.c src/util.c src/canaries.c src/usbs.c $(LIBS)
