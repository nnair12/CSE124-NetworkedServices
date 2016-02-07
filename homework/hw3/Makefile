CC=gcc
CFLAGS=-ggdb -std=c99 -D_GNU_SOURCE -pthread

all: client server

client: TCPDateTimeClient4.c DieWithMessage.c
	$(CC) $(CFLAGS) -o client TCPDateTimeClient4.c DieWithMessage.c

server: TCPDateTimeServer4.c DieWithMessage.c TCPServerUtility.c AddressUtility.c
	$(CC) $(CFLAGS) -o server TCPDateTimeServer4.c DieWithMessage.c TCPServerUtility.c AddressUtility.c

clean:
	rm -rf client server client.dSYM server.dSYM