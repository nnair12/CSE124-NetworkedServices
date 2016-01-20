//
// Created by Daniel Kao on 1/19/16.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include "Practical.h"

int main(int argc, char *argv[]) {

    // Test for correct number of arguments
    if (argc != 2)
        DieWithUserMessage("Parameter(s)", "<Server URL>");

    // Local variables setup
    struct addrinfo hints;
    struct addrinfo *result, *rp;
    int dnsResponse, sfd;
    long portColonIndex, serverRootIndex;
    char hostname[50], portNum[60], serverPath[600];

    // Set getAddrInfo struct
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;    /* Allow IPv4 or IPv6 */
    hints.ai_socktype = SOCK_DGRAM; /* Datagram socket */
    hints.ai_flags = 0;
    hints.ai_protocol = 0;          /* Any protocol */

    // Get hostname
    if(strchr(argv[1], ':') != NULL || strchr(argv[1], '/') != NULL) {
        if(strchr(argv[1], ':') != NULL) {
            portColonIndex = strlen(argv[1]) - strlen(strchr(argv[1], ':'));
            memcpy(hostname, argv[1], portColonIndex);
            hostname[portColonIndex] = '\0';
        }
        else {
            serverRootIndex = strlen(argv[1]) - strlen(strchr(argv[1], '/'));
            memcpy(hostname, argv[1], serverRootIndex);
            hostname[serverRootIndex] = '\0';
        }
    }
    else {
        strcpy(hostname, argv[1]);
    }

    // Get port number
    if(strchr(argv[1], ':') != NULL) {
        if(strchr(argv[1], '/') != NULL) {
            serverRootIndex = strlen(argv[1]) - strlen(strchr(argv[1], '/'));
            memcpy(portNum, argv[1], serverRootIndex);
            strcpy(portNum, strchr(portNum, ':'));
        }
        else {
            strcpy(portNum, strchr(argv[1], ':'));
        }
        memmove(portNum, portNum+1, strlen(portNum));
    }
    else {
        strcpy(portNum, "80");
    }

    // Get server path
    if(strchr(argv[1], '/') != NULL) {
        strcpy(serverPath, strchr(argv[1], '/'));
    }

    // Do DNS lookup
    dnsResponse = getaddrinfo(hostname, portNum, &hints, &result);

    // DNS lookup fail
    if (dnsResponse != 0) {
        exit(EXIT_FAILURE);
    }

    // loop through address structs
    for (rp = result; rp != NULL; rp = rp->ai_next) {
        sfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (sfd == -1) {
            continue;
        }
        // Successfully Connected
        if (connect(sfd, rp->ai_addr, rp->ai_addrlen) != -1) {

            // Receive the same string back from the server
            char buffer[BUFSIZE]; // I/O buffer
            /* Receive up to the buffer size (minus 1 to leave space for
             a null terminator) bytes from the sender */
            long numBytes = recv(sfd, buffer, BUFSIZE - 1, 0);
            if (numBytes < 0)
                DieWithSystemMessage("recv() failed");
            else if (numBytes == 0)
                DieWithUserMessage("recv()", "connection closed prematurely");
            buffer[numBytes] = '\0';    // Terminate the string!
            fputs(buffer, stdout);      // Print the echo buffer

            break;
        }
        close(sfd);
    }

    // No address succeeded
    if (rp == NULL) {
        fprintf(stderr, "Could not connect\n");
        exit(EXIT_FAILURE);
    }

    // No longer needed
    freeaddrinfo(result);

    fputc('\n', stdout); // Print a final linefeed
    exit(0);
}
