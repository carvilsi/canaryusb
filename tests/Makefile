CC = gcc
LIBS = -ludev
CCFLAGS = -Werror -Wno-discarded-qualifiers

SRC := $(shell find ./../src/ -type f -name '*.c' ! -name 'main.c') 

all: test 

test:
	$(CC) $(CCFLAGS) -o $@ $(SRC) ct.c canaryusb_test.c $(LIBS)

clean:
	rm test 

