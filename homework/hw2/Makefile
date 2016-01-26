CC=gcc
CFLAGS=-ggdb -std=c99 -D_GNU_SOURCE

all: client server

client: TCPEchoClient4.c DieWithMessage.c
	$(CC) $(CFLAGS) -o client TCPEchoClient4.c DieWithMessage.c

server: TCPEchoServer4.c DieWithMessage.c TCPServerUtility.c AddressUtility.c
	$(CC) $(CFLAGS) -o server TCPEchoServer4.c DieWithMessage.c TCPServerUtility.c AddressUtility.c

clean:
	rm -rf client server client.dSYM server.dSYM