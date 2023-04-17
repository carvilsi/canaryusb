CC = gcc
LIBS = -ludev
CCFLAGS =

all: executable

debug: CCFLAGS += -DDEBUG 
debug: executable
	
executable:
	$(CC) $(CCFLAGS) -o canaryusb canaryusb.c base32.c utils/trusted_list.c $(LIBS)
