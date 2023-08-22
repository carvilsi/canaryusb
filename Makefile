CC = gcc
LIBS = -ludev
CCFLAGS = -Werror
DEST_BIN = ~/.local/bin/
DEST_CONF = ~/.config/canaryusb/
SRC_CONF = ./configuration/config.toml

SRC := $(shell find ./src/ -name '*.c') 

all: canaryusb 

debug: CCFLAGS += -DDEBUG -g 
debug: canaryusb 

# debug and do not call the canary token 
silence: CCFLAGS += -DSILENCE -DDEBUG 
silence: canaryusb

install: canaryusb
	mv canaryusb $(DEST_BIN)
	mkdir -p $(DEST_CONF)
	-cp -n $(SRC_CONF) $(DEST_CONF) 
	source ~/.bashrc

uninstall:
	rm  $(DEST_BIN)canaryusb
	rm -rf $(DEST_CONF)
	
canaryusb:
	$(CC) $(CCFLAGS) -o $@ $(SRC) $(LIBS)

clean:
	rm canaryusb 

