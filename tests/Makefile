CC = gcc
LIBS = -ludev
CCFLAGS = -Werror -Wno-discarded-qualifiers -DTESTS -DSILENCE

SRC := $(shell find ./../src/ -type f -name '*.c' ! -name 'main.c') 

all: test 

test:
	$(CC) $(CCFLAGS) -o $@ $(SRC) ct.c canaryusb_test.c $(LIBS)
	./test

clean:
	rm test 

