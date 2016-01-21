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


struct Host {
    char hostname[50], portNum[60], serverPath[600];
};

struct Request {
    struct Host host;
    char method[10];
};


/**
 * processURL is a function that takes a pointer to a Host struct and the URL of the request to be made.
 *
 * It processes the URL into the parameters within the given Host struct, giving it the hostname, portNumber,
 * and serverPath.
 */
void processURL(struct Host *host, char* url) {
    
    long portColonIndex, serverRootIndex;
    char withoutHTTP[700];

    // Removes http:// from input if input contains http://
    if(strstr(url, "//") != NULL) {
        strcpy(withoutHTTP, strstr(url, "//"));
        memmove(withoutHTTP, withoutHTTP + 2, strlen(withoutHTTP));
    }
    else {
        strcpy(withoutHTTP, url);
    }

    // Get hostname if specified with colon
    if(strchr(withoutHTTP, ':') != NULL || strchr(withoutHTTP, '/') != NULL) {
        if(strchr(withoutHTTP, ':') != NULL) {
            portColonIndex = strlen(withoutHTTP) - strlen(strchr(withoutHTTP, ':'));
            memcpy(host->hostname, withoutHTTP, portColonIndex);
            host->hostname[portColonIndex] = '\0';
        }
        else {
            serverRootIndex = strlen(withoutHTTP) - strlen(strchr(withoutHTTP, '/'));
            memcpy(host->hostname, withoutHTTP, serverRootIndex);
            host->hostname[serverRootIndex] = '\0';
        }
    }
    else {
        strcpy(host->hostname, withoutHTTP);
    }

    // Get port number
    if(strchr(withoutHTTP, ':') != NULL) {
        if(strchr(withoutHTTP, '/') != NULL) {
            serverRootIndex = strlen(withoutHTTP) - strlen(strchr(withoutHTTP, '/'));
            memcpy(host->portNum, withoutHTTP, serverRootIndex);
            strcpy(host->portNum, strchr(host->portNum, ':'));
        }
        else {
            strcpy(host->portNum, strchr(withoutHTTP, ':'));
        }
        memmove(host->portNum, host->portNum+1, strlen(host->portNum));
    }
    else {
        strcpy(host->portNum, "80");
    }

    // Get server path
    if(strchr(withoutHTTP, '/') != NULL) {
        strcpy(host->serverPath, strchr(withoutHTTP, '/'));
    }
    else {
        strcpy(host->serverPath, "/");
    }
};


/**
 * httpReguestString turns a struct object Request into the http string that it can send to the server
 */
void httpRequestString(char **s, struct Request request) {

    size_t size;

    if(strlen(request.method) == 0) {
        strcpy(request.method, "GET");
    }

    size = snprintf(NULL, 0, "%s %s HTTP/1.1\nHost: %s\n\n", request.method, request.host.serverPath, request.host.hostname);
    *s = (char *)malloc(size);
    snprintf(*s, size+1, "%s %s HTTP/1.1\nHost: %s\n\n", request.method, request.host.serverPath, request.host.hostname);
};




///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

int main(int argc, char *argv[]) {

    // Test for correct number of arguments
    if (argc != 2)
        DieWithUserMessage("Parameter(s)", "<Server URL>");

    // Local variables setup
    struct Host requestHost;
    struct Request request;
    struct addrinfo hints, *result, *rp;
    int dnsResponse, sfd;
    char *requestString;

    // Set getAddrInfo struct
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;    /* Allow IPv4 or IPv6 */
    hints.ai_socktype = 0;          /* Datagram socket */
    hints.ai_flags = 0;
    hints.ai_protocol = 0;          /* Any protocol */

    // Processes the URL
    processURL(&requestHost, argv[1]);
    request.host = requestHost;

    // Do DNS lookup
    dnsResponse = getaddrinfo(requestHost.hostname, requestHost.portNum, &hints, &result);

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

            httpRequestString(&requestString, request);

            // Send the HTTP request to the server
            if (send(sfd, requestString, strlen(requestString), 0) < 0)
                DieWithSystemMessage("send() failed");

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
