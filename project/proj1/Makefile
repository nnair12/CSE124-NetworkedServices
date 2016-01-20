CC=gcc
CFLAGS=-ggdb -std=c99

all: http-client

http-client: http-client.c DieWithMessage.c
	$(CC) $(CFLAGS) -o http-client http-client.c DieWithMessage.c

clean:
	rm -rf http-client http-client.dSYM