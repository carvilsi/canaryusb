CC = gcc
LIBS = -ludev
CCFLAGS =
DEST_BIN = ~/.local/bin/
DEST_CONF = ~/.config/canaryusb/
SRC_CONF = ./configuration/config.toml

SRC := $(shell find . -name '*.c') 

all: canaryusb 

debug: CCFLAGS += -DDEBUG 
debug: CCFLAGS += -g 
debug: canaryusb 

install: canaryusb
	mv canaryusb $(DEST_BIN)
	mkdir -p $(DEST_CONF)
	cp $(SRC_CONF) $(DEST_CONF) 
	source ~/.bashrc

uninstall:
	rm  $(DEST_BIN)canaryusb
	rm -rf $(DEST_CONF)
	
canaryusb:
	$(CC) $(CCFLAGS) -o $@ $(SRC) $(LIBS)

clean:
	rm canaryusb 

