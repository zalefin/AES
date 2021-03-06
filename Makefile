CC=gcc
LFLAGS=-g
CFLAGS=-c -g

.PHONY: clean

all: aes_encrypt aes_decrypt

aes_encrypt: aes_encrypt.c aes.o
	${CC} $^ -o $@

aes_decrypt: aes_decrypt.c aes.o
	${CC} $^ -o $@

tests: tests.c aes.o
	${CC} $^ -o $@

aes.o: aes.h aes.c
	${CC} ${LIBS} ${CFLAGS} $^

clean:
	rm -f *.o *.gch aes_encrypt aes_decrypt tests
