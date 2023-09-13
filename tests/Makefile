CC = gcc
LDFLAGS = `pkg-config --cflags --libs libsystemd`
CCFLAGS = -Werror -Wno-discarded-qualifiers -Wno-format-overflow -DTESTS -DSILENCE

SRC := $(shell find ./../src/ -type f -name '*.c' ! -name 'main.c') 

all: test 

test:
	$(CC) $(CCFLAGS) -o $@ $(SRC) cst.c canaryusb_test.c $(LDFLAGS)
	./test

clean:
	rm test 

