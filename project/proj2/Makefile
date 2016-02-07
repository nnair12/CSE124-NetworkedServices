CC=gcc
CFLAGS=-ggdb -std=c99 -D_GNU_SOURCE -pthread

all: http-server

http-server: http-server.c DieWithMessage.c AddressUtility.c
	$(CC) $(CFLAGS) -o http-server http-server.c DieWithMessage.c AddressUtility.c

clean:
	rm -rf http-server http-server.dSYM