#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include "Practical.h"

/**
 * http-client.c
 *
 * Author: Daniel Kao
 * PID: A10546439
 * Date: 1/19/16
 *
 */

struct Host {
    char hostname[50], portNum[60], serverPath[600];
};

struct Request {
    struct Host host;
    char method[10];
};

struct Header {
    char httpVersion[20], contentType[50];
    unsigned long contentLength;
    char * header;
};

struct Response {
    struct Header header;
    bool headerRead;
    char * body;
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
        host->portNum[strlen(host->portNum) - 2] = '\0';
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
 * httpReguestString turns a struct object Request into the http string that it can send to the server.
 */
void httpRequestString(char **s, struct Request request) {

    size_t size;

    if(strlen(request.method) == 0) {
        strcpy(request.method, "GET");
    }

    size = snprintf(NULL, 0, "%s %s HTTP/1.1\r\nHost: %s\r\nConnection: close\r\n\r\n", request.method, request.host.serverPath, request.host.hostname);
    *s = (char *)malloc(size);
    snprintf(*s, size + 1, "%s %s HTTP/1.1\r\nHost: %s\r\nConnection: close\r\n\r\n", request.method, request.host.serverPath, request.host.hostname);
};


/**
 * httpResponseComplete takes a response struct and returns whether or not the struct is complete, valid
 * response.
 */
bool httpResponseComplete(struct Response response) {

    // If header has not been fully read
    if(!response.headerRead) {
        return false;
    }

    // If header has been read, contentLength has been read, and the body is the same length.
    if(response.header.contentLength > 0 && strlen(response.body) == response.header.contentLength) {
        return true;
    }

    // Otherwise return false
    return false;
};


/**
 * procBuffer takes a response struct and a buffer and updates the response struct to appropriately
 * reflect the new information.
 */
void procBuffer(struct Response * response, char buffer[]) {

    // Carriage Return Line Feed
    char doublecrlf[] = "\r\n\r\n";

    // Base case: do nothing if buffer is empty.
    if(strlen(buffer) == 0) {
        return;
    }

    // Header has not been completely read yet
    if(!response->headerRead) {

        // Header has not been started yet
        if(strlen(response->header.header) == 0 && !strstr(buffer, doublecrlf)) {

            // Initialize header text and copy buffer into it.
            response->header.header = (char *)malloc(BUFSIZE);
            strcpy(response->header.header, buffer);
        }

        // Header has ending within the buffer
        else if(strlen(response->header.header) == 0 && strstr(buffer, doublecrlf)) {

            // Copy over only the header data
            size_t headerEndIndex = strlen(buffer) - strlen(strstr(buffer, doublecrlf)) + 1;
            response->header.header = (char *)malloc(headerEndIndex);
            memcpy(response->header.header, buffer, headerEndIndex);
            response->header.header[headerEndIndex] = '\0';

            // Set read header to true
            response->headerRead = true;
        }

        // Header does not have ending within the buffer
        else if(strlen(response->header.header) != 0 && !strstr(buffer, doublecrlf)) {

            // Append the entire buffer to the end of the existing one
            char * temp = malloc(strlen(response->header.header) + strlen(buffer));
            memcpy(temp, response->header.header, 0);
            memcpy(temp, buffer, strlen(response->header.header));

            // Check if new header has ending
            if(strstr(temp, doublecrlf)) {

                // Free the old string
                free(response->header.header);

                // Copy the appropriate part of the new string into the header
                size_t headerEndIndex = strlen(temp) - strlen(strstr(temp, doublecrlf)) + 1;
                response->header.header = (char *)malloc(headerEndIndex);
                memcpy(response->header.header, temp, headerEndIndex);
                response->header.header[headerEndIndex] = '\0';

                // Set read header to true
                response->headerRead = true;
            }

            // New string has no ending and will be copied back into the response object
            else {

                // Free the old string
                free(response->header.header);

                // Resize the new string
                response->header.header = malloc(strlen(temp));

                // Copy temp into the new string
                strcpy(response->header.header, temp);
            }

            free(temp);
        }

        // Header has already been started and has ending in the buffer
        else if(strlen(response->header.header) != 0 && strstr(buffer, doublecrlf)) {

            // Append the entire buffer to the end of the existing one
            char * temp = malloc(strlen(response->header.header) + strlen(buffer));
            memcpy(temp, response->header.header, 0);
            memcpy(temp, buffer, strlen(response->header.header));

            // Free the old string
            free(response->header.header);

            // Append the entire buffer to the end of the existing one
            size_t headerEndIndex = strlen(temp) - strlen(strstr(temp, doublecrlf)) + 1;
            response->header.header = (char *)malloc(headerEndIndex);
            memcpy(response->header.header, temp, headerEndIndex);
            response->header.header[headerEndIndex] = '\0';

            // Set read header to true
            response->headerRead = true;

            free(temp);
        }
    }

    // Reading the body
    if(response->headerRead) {

        // TODO Header ending is in the buffer
        if(strstr(buffer, doublecrlf)) {

        }

        // TODO Header ending is not in the buffer
        else if (!strstr(buffer, doublecrlf)) {

        }
    }
}


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
    struct Response response;
    struct addrinfo hints, *result, *rp;
    int dnsResponse, sfd;
    char *requestString;
    struct timeval tv;
    tv.tv_sec = 3;  /* 30 Secs Timeout */
    tv.tv_usec = 0;  // Not init'ing this can cause strange errors

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
        DieWithSystemMessage("getaddrinfo() failed");
    }

    // Generate the request string
    httpRequestString(&requestString, request);

    // loop through address structs
    for (rp = result; rp != NULL; rp = rp->ai_next) {

        sfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (sfd == -1) {
            continue;
        }

        // set timeout for socket
        setsockopt(sfd, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv,sizeof(struct timeval));

        // Successfully Connected
        if (connect(sfd, rp->ai_addr, rp->ai_addrlen) != -1) {

            // Initialize response struct
            memset(&response, 0, sizeof(struct Response));
            response.headerRead = false;

            // Send the HTTP request to the server
            if (send(sfd, requestString, strlen(requestString), 0) < 0) {
                DieWithSystemMessage("send() failed");
            }

            // Continue to recv until the data is complete
            while (!httpResponseComplete(response)) {

                printf("%d %ld %ld\n", response.headerRead, strlen(response.body), response.header.contentLength);

                // I/O buffer
                char buffer[BUFSIZE];

                // Receive up to the buffer size (minus 1 to leave space for
                // a null terminator) bytes from the sender
                ssize_t numBytes = recv(sfd, buffer, BUFSIZE, 0);

                procBuffer(&response, buffer);

                // If the server is no longer returning any more but the system expects more
                if (numBytes == 0) {
                    fputs("2", stdout);     // Print the echo buffer
                    fputc('\n', stdout);    // Print a final linefeed
                    exit(2);
                }
            }

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
    free(requestString);

    fputc('\n', stdout); // Print a final linefeed
    return 0;
}
