#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <inttypes.h>
#include <ctype.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "Practical.h"

/**
 * http-server.c
 *
 * Author: Daniel Kao
 * PID: A10546439
 * Date: 2/2/16
 *
 */



static const int MAXPENDING = 500; // Maximum outstanding connection requests

// Structure of arguments to pass to client thread
struct ThreadArgs {
    int clntSock; // Socket descriptor for client
};



int SetupTCPServerSocket(const char *service) {
    // Construct the server address structure
    struct addrinfo addrCriteria;                   // Criteria for address match
    memset(&addrCriteria, 0, sizeof(addrCriteria)); // Zero out structure
    addrCriteria.ai_family = AF_UNSPEC;             // Any address family
    addrCriteria.ai_flags = AI_PASSIVE;             // Accept on any address/port
    addrCriteria.ai_socktype = SOCK_STREAM;         // Only stream sockets
    addrCriteria.ai_protocol = IPPROTO_TCP;         // Only TCP protocol

    struct addrinfo *servAddr; // List of server addresses
    int rtnVal = getaddrinfo(NULL, service, &addrCriteria, &servAddr);
    if (rtnVal != 0)
        DieWithUserMessage("getaddrinfo() failed", gai_strerror(rtnVal));

    int servSock = -1;
    for (struct addrinfo *addr = servAddr; addr != NULL; addr = addr->ai_next) {
        // Create a TCP socket
        servSock = socket(addr->ai_family, addr->ai_socktype,
                          addr->ai_protocol);
        if (servSock < 0)
            continue;       // Socket creation failed; try next address

        // Bind to the local address and set socket to listen
        if ((bind(servSock, addr->ai_addr, addr->ai_addrlen) == 0) &&
            (listen(servSock, MAXPENDING) == 0)) {
            // Print local address of socket
            struct sockaddr_storage localAddr;
            socklen_t addrSize = sizeof(localAddr);
            if (getsockname(servSock, (struct sockaddr *) &localAddr, &addrSize) < 0)
                DieWithSystemMessage("getsockname() failed");
            fputs("Binding to ", stdout);
            PrintSocketAddress((struct sockaddr *) &localAddr, stdout);
            fputc('\n', stdout);
            break;       // Bind and listen successful
        }

        close(servSock);  // Close and try again
        servSock = -1;
    }

    // Free address list allocated by getaddrinfo()
    freeaddrinfo(servAddr);

    return servSock;
}

int AcceptTCPConnection(int servSock) {
    struct sockaddr_storage clntAddr; // Client address
    // Set length of client address structure (in-out parameter)
    socklen_t clntAddrLen = sizeof(clntAddr);

    // Wait for a client to connect
    int clntSock = accept(servSock, (struct sockaddr *) &clntAddr, &clntAddrLen);
    if (clntSock < 0)
        DieWithSystemMessage("accept() failed");

    // clntSock is connected to a client!

    fputs("Handling client ", stdout);
    PrintSocketAddress((struct sockaddr *) &clntAddr, stdout);
    fputc('\n', stdout);

    return clntSock;
}

void HandleTCPClient(int clntSocket) {
    char datestring[5] = "date\0";
    char timestring[5] = "time\0";
    char buffer[BUFSIZE]; // Buffer for echo string
    char ret[11];

    // Get current date and time
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);

    // Receive message from client
    ssize_t numBytesRcvd = recv(clntSocket, buffer, BUFSIZE, 0);
    if (numBytesRcvd < 0)
        DieWithSystemMessage("recv() failed");

    // Figure out which string was given
    if(strcmp(buffer, datestring) == 0) {
        strftime(ret, 11, "%Y-%m-%d", &tm);
    }
    if(strcmp(buffer, timestring) == 0) {
        strftime(ret, 11, "%H:%M:%S", &tm);
    }

    // Echo message back to client
    ssize_t numBytesSent = send(clntSocket, ret, strlen(ret), 0);
    if (numBytesSent < 0)
        DieWithSystemMessage("send() failed");
    else if (numBytesSent != strlen(ret))
        DieWithUserMessage("send()", "sent unexpected number of bytes");

    close(clntSocket); // Close client socket
}



void *thread_main(void *args)
{
    // Guarantees that thread resources are deallocated upon return
    pthread_detach(pthread_self());

    // Extract socket file descriptor from argument
    int clntSock = ((struct ThreadArgs *) args)->clntSock;
    free(args); // Deallocate memory for argument

    HandleTCPClient(clntSock);

    return (NULL);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


int main(int argc, char *argv[]) {

    if (argc != 2) // Test for correct number of arguments
        DieWithUserMessage("Parameter(s)", "<Server Port>");

    in_port_t servPort = atoi(argv[1]); // First arg:  local port

    // Create socket for incoming connections
    int servSock; // Socket descriptor for server
    if ((servSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
        DieWithSystemMessage("socket() failed");

    // Construct local address structure
    struct sockaddr_in servAddr;                  // Local address
    memset(&servAddr, 0, sizeof(servAddr));       // Zero out structure
    servAddr.sin_family = AF_INET;                // IPv4 address family
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY); // Any incoming interface
    servAddr.sin_port = htons(servPort);          // Local port

    // Bind to the local address
    if (bind(servSock, (struct sockaddr *) &servAddr, sizeof(servAddr)) < 0)
        DieWithSystemMessage("bind() failed");

    // Mark the socket so it will listen for incoming connections
    if (listen(servSock, MAXPENDING) < 0)
        DieWithSystemMessage("listen() failed");

    for (;;) { // Run forever

        // Create separate memory for client argument
        struct ThreadArgs *threadArgs = (struct ThreadArgs *) malloc(
                sizeof(struct ThreadArgs));
        if (threadArgs == NULL)
            DieWithSystemMessage("malloc() failed");

        struct sockaddr_in clntAddr; // Client address
        // Set length of client address structure (in-out parameter)
        socklen_t clntAddrLen = sizeof(clntAddr);

        // Wait for a client to connect
        int clntSock = AcceptTCPConnection(servSock);

        threadArgs->clntSock = clntSock;

        // Create client thread
        pthread_t threadID;
        int returnValue = pthread_create(&threadID, NULL, thread_main, threadArgs);

        if (returnValue != 0)
            DieWithUserMessage("pthread_create() failed", strerror(returnValue));

        printf("with thread %ld\n", (long int) threadID);

    }
}
