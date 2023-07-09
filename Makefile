CC = gcc
LIBS = -ludev
CCFLAGS =

all: executable

debug: CCFLAGS += -DDEBUG 
debug: executable
	
executable:
	$(CC) $(CCFLAGS) -o canaryusb src/canaryusb.c src/utils/base32.c src/utils/trusted_list.c src/utils/util.c src/canary/canaries.c src/usbs/usbs.c $(LIBS)
