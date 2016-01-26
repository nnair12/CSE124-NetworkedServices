#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <inttypes.h>
#include <ctype.h>
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
    char * hostname;
    char * portNum;
    char * serverPath;
};

struct Request {
    struct Host host;
    char * method;
};

struct Header {
    char * contentType;
    unsigned long contentLength, responseCode;
    char * header;
};

struct Response {
    struct Header header;
    bool headerRead;
    char * body;
};


/**
 * equalsIgnoreCase takes two char arrays and returns true if they are equal, ignoring the case.
 */
bool equalsIgnoreCase(char * a, char * b) {
    if(strlen(a) != strlen(b)) {
        return false;
    }
    for(int i = 0; i < strlen(a); i++) {
        if(tolower(a[i]) != tolower(b[i])) {
            return false;
        }
    }
    return true;
};

/**
 * equalsIgnoreCase takes two char arrays and returns true if they are equal.
 */
bool equals(char * a, char * b) {
    if(strlen(a) != strlen(b)) {
        return false;
    }
    for(int i = 0; i < strlen(a); i++) {
        if(a[i] != b[i]) {
            return false;
        }
    }
    return true;
};

/**
 * indexOf returns the index where b is found in a.
 */
int indexOf(char * a, char * b) {
    if(strlen(b) > strlen(a)) {
        return -1;
    }
    for(int i = 0; i < strlen(a); i++) {
        if(a[i] == b[0]) {
            for(int j = 0; j < strlen(b); j++) {
                if(a[i+j] != b[j]) {
                    break;
                }
                if(a[i+j] == b[j] && j == strlen(b) - 1) {
                    return i;
                }
            }
        }
    }
    return -1;
};

/**
 * indexOfIgnoreCase returns the index where b is found in a, ignoring the case.
 */
int indexOfIgnoreCase(char * a, char * b) {
    if(strlen(b) > strlen(a)) {
        return -1;
    }
    for(int i = 0; i < strlen(a); i++) {
        if(tolower(a[i]) == tolower(b[0])) {
            for(int j = 0; j < strlen(b); j++) {
                if(tolower(a[i+j]) != tolower(b[j])) {
                    break;
                }
                if(tolower(a[i+j]) == tolower(b[j]) && j == (strlen(b) - 1)) {
                    return i;
                }
            }
        }
    }
    return -1;
};

/**
 * contains returns true if char array b can be found inside of a.
 */
bool contains(char * a, char * b) {
    return (indexOf(a, b) >= 0);
};

/**
 * containsIgnoreCase returns true if char array can be found inside of a, ignoring case
 */
bool containsIgnoreCase(char * a, char * b) {
    return (indexOfIgnoreCase(a, b) >= 0);
};

/**
 * processURL is a function that takes a pointer to a Host struct and the URL of the request to be made.
 *
 * It processes the URL into the parameters within the given Host struct, giving it the hostname, portNumber,
 * and serverPath.
 */
void processURL(struct Host * host, char * url) {
    
    size_t portColonIndex, serverRootIndex;
    char * withoutHTTP;

    // Removes http:// from input if input contains http://
    if(strstr(url, "//") != NULL) {
        withoutHTTP = (char *)malloc(strlen(strstr(url, "//")));
        strcpy(withoutHTTP, strstr(url, "//"));
        memmove(withoutHTTP, withoutHTTP + 2, strlen(withoutHTTP));
    }
    else {
        withoutHTTP = (char *)malloc(strlen(url));
        strcpy(withoutHTTP, url);
    }

    // Get hostname if specified with colon
    if(strchr(withoutHTTP, ':') != NULL || strchr(withoutHTTP, '/') != NULL) {
        if(strchr(withoutHTTP, ':') != NULL) {
            portColonIndex = strlen(withoutHTTP) - strlen(strchr(withoutHTTP, ':'));
            host->hostname = (char *)malloc(portColonIndex);
            memcpy(host->hostname, withoutHTTP, portColonIndex);
            host->hostname[portColonIndex] = '\0';
        }
        else {
            serverRootIndex = strlen(withoutHTTP) - strlen(strchr(withoutHTTP, '/'));
            host->hostname = (char *)malloc(serverRootIndex);
            memcpy(host->hostname, withoutHTTP, serverRootIndex);
            host->hostname[serverRootIndex] = '\0';
        }
    }
    else {
        host->hostname = (char *)malloc(strlen(withoutHTTP));
        strcpy(host->hostname, withoutHTTP);
    }

    // Get port number
    if(strchr(withoutHTTP, ':') != NULL) {
        if(strchr(withoutHTTP, '/') != NULL) {
            serverRootIndex = strlen(withoutHTTP) - strlen(strchr(withoutHTTP, '/'));
            char * temp = (char *)malloc(serverRootIndex);
            memcpy(temp, withoutHTTP, serverRootIndex);
            host->portNum = (char *)malloc(strlen(strchr(temp, ':')));
            strcpy(host->portNum, strchr(temp, ':'));
            free(temp);
        }
        else {
            host->portNum = (char *)malloc(strlen(strchr(withoutHTTP, ':')));
            strcpy(host->portNum, strchr(withoutHTTP, ':'));
        }
        memmove(host->portNum, host->portNum + 1, strlen(host->portNum));
        host->portNum[strlen(host->portNum)] = '\0';
    }
    else {
        host->portNum = (char *)malloc(3);
        strcpy(host->portNum, "80");
    }

    // Get server path
    if(strchr(withoutHTTP, '/') != NULL) {
        host->serverPath = (char *)malloc(strlen(strchr(withoutHTTP, '/')));
        strcpy(host->serverPath, strchr(withoutHTTP, '/'));
    }
    else {
        host->serverPath = (char *)malloc(2);
        strcpy(host->serverPath, "/");
    }

    free(withoutHTTP);
};


/**
 * httpResponseComplete takes a response struct and returns whether or not the struct is complete, valid
 * response.
 */
bool httpResponseComplete(struct Response * response) {

    // If header has not been fully read
    if(!response->headerRead) {
        return false;
    }

    // If header has been read, contentLength has been read, and the body is the same length.
    if(response->header.contentLength > 0 && strlen(response->body) == response->header.contentLength) {
        return true;
    }

    // Otherwise return false
    return false;
};


/**
 * procHeader takes a Response object and processes the string in the header object into the appropriate
 * fields.
 */
void procHeader(struct Response * response, struct Request * request) {

    char contentLength[] = "Content-Length:";
    char contentType[] = "Content-Type:";

    // Make sure header string is set
    if(response->header.header == NULL) {
        return;
    }

    // Find Content-Length
    if(containsIgnoreCase(response->header.header, contentLength)) {

        // Cut off everything before "Content-Length"
        char * choppedContent = (char *)malloc(strlen(&response->header.header[indexOfIgnoreCase(response->header.header, contentLength)]));
        strcpy(choppedContent, &response->header.header[indexOfIgnoreCase(response->header.header, contentLength)]);

        char * cl;

        // Cut off everything after the CRLF, if it exists
        if(strstr(choppedContent, "\r\n")) {
            cl = (char *)malloc(strlen(choppedContent) - strlen(strstr(choppedContent, "\r\n")) - strlen(contentLength) + 1);
            memcpy(cl, choppedContent + strlen(contentLength), strlen(choppedContent) - strlen(strstr(choppedContent, "\r\n")) - strlen(contentLength));
        }
        else {
            cl = (char *)malloc(strlen(choppedContent) - strlen(contentLength) + 1);
            memcpy(cl, choppedContent + strlen(contentLength), strlen(choppedContent) - strlen(contentLength));
        }

        cl[strlen(cl)] = '\0';

        // Assign the cl into the header content length
        response->header.contentLength = strtoumax(cl, NULL, 10);
    }

    // Get Content-Type
    if(containsIgnoreCase(response->header.header, contentType)) {

        // Cut off everything before "Content-Type"
        char * choppedContent = (char *)malloc(strlen(&response->header.header[indexOfIgnoreCase(response->header.header, contentType)]));
        strcpy(choppedContent, &response->header.header[indexOfIgnoreCase(response->header.header, contentType)]);

        char * cl;

        // Cut off everything after the CRLF, if it exists
        if(strstr(choppedContent, "\r\n")) {
            cl = (char *)malloc(strlen(choppedContent) - strlen(strstr(choppedContent, "\r\n")) - strlen(contentType) + 1);
            memcpy(cl, choppedContent + strlen(contentType), strlen(choppedContent) - strlen(strstr(choppedContent, "\r\n")) - strlen(contentType));
        }
        else {
            cl = (char *)malloc(strlen(choppedContent) - strlen(contentType) + 1);
            memcpy(cl, choppedContent + strlen(contentType), strlen(choppedContent) - strlen(contentType));
        }

        cl[strlen(cl)] = '\0';

        // Assign the cl into the header content length
        response->header.contentType = (char *)malloc(strlen(cl));
        strcpy(response->header.contentType, cl);

        // check if content-type is valid
        if(contains(response->header.contentType, "jpeg") || contains(request->host.serverPath, "jpg")) {
            if(!(contains(request->host.serverPath, ".jpg") || contains(request->host.serverPath, ".jpeg"))) {
                printf("7\n");
                exit(7);
            }
        }
        if(contains(response->header.contentType, "png")) {
            if(!(contains(request->host.serverPath, ".png"))) {
                printf("7\n");
                exit(7);
            }
        }
    }

    // Get HTTP response code
    size_t protocolLineLength = strlen(response->header.header) - strlen(strstr(response->header.header, "\n"));
    char * protocolLine = (char *)malloc(protocolLineLength + 1);

    // Get the first line
    protocolLine = memcpy(protocolLine, response->header.header, protocolLineLength);
    protocolLine[protocolLineLength] = '\0';

    for(int i = 0; i < protocolLineLength; i++) {
        if(isspace(protocolLine[i])) {
            response->header.responseCode = strtoumax(&protocolLine[i], NULL, 10);

            if(!(response->header.responseCode == 200
               || response->header.responseCode == 400
               || response->header.responseCode == 403
               || response->header.responseCode == 404)) {
                printf("3\n");
                exit(3);
            }
            break;
        }
    }

    free(protocolLine);

    // Malformed header was passed in
    if(!response->header.contentLength || response->header.responseCode == 0) {
        // exit program with proper code
        printf("4\n");
        exit(4);
    }
}


/**
 * procBuffer takes a response struct and a buffer and updates the response struct to appropriately
 * reflect the new information.
 */
void procBuffer(struct Response * response, char buffer[], struct Request * request) {

    // Carriage Return Line Feed
    char doublecrlf[] = "\r\n\r\n";

    // Base case: do nothing if buffer is empty.
    if(strlen(buffer) == 0) {
        return;
    }

    bool proccesedHeader = false;

    // Header has not been completely read yet
    if(!response->headerRead) {

        // Header has not been started yet
        if((response->header.header == NULL || strlen(response->header.header) == 0) && !strstr(buffer, doublecrlf)) {

            // Initialize header text and copy buffer into it.
            response->header.header = (char *)malloc(BUFSIZE);
            strcpy(response->header.header, buffer);
        }

        // Header has ending within the buffer
        else if((response->header.header == NULL || strlen(response->header.header) == 0) && strstr(buffer, doublecrlf)) {

            // Copy over only the header data
            size_t headerEndIndex = strlen(buffer) - strlen(strstr(buffer, doublecrlf)) + 1;
            response->header.header = (char *)malloc(headerEndIndex);
            memcpy(response->header.header, buffer, headerEndIndex);
            response->header.header[headerEndIndex] = '\0';

            // Set read header to true
            response->headerRead = true;

            procHeader(response, request);
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

                procHeader(response, request);
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
            strcpy(temp, response->header.header);
            strcat(temp, buffer);

            // Free the old string
            free(response->header.header);

            // Append the entire buffer to the end of the existing one
            size_t headerEndIndex = strlen(temp) - strlen(strstr(temp, doublecrlf)) + 1;
            response->header.header = (char *)malloc(headerEndIndex);
            memcpy(response->header.header, temp, headerEndIndex);
            response->header.header[headerEndIndex] = '\0';

            // Set read header to true
            response->headerRead = true;

            procHeader(response, request);

            free(temp);
        }

        proccesedHeader = true;
    }

    // Reading the body
    if(response->headerRead) {

        // Body has not yet been set
        if(response->body == NULL) {
            response->body = (char *)malloc(1);
        }

        char * temp;

        // Header ending is in the buffer
        if(strstr(buffer, doublecrlf) && proccesedHeader) {
            temp = (char *)malloc(strlen(response->body) + strlen(strstr(buffer, doublecrlf)) - strlen(doublecrlf));
            memcpy(temp, response->body, strlen(response->body));
            memcpy(temp + strlen(response->body), strstr(buffer, doublecrlf) + strlen(doublecrlf), strlen(strstr(buffer, doublecrlf)) - strlen(doublecrlf));
        }

        // Header ending is not in the buffer
        else {
            temp = (char *)malloc(strlen(response->body) + strlen(buffer));
            memcpy(temp, response->body, strlen(response->body));
            memcpy(temp + strlen(response->body), buffer, strlen(buffer));
        }

        // Save to response body
        free(response->body);
        response->body = (char *)malloc(strlen(temp));
        strcpy(response->body, temp);

        free(temp);
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
    struct addrinfo hints, *result, *rp;
    struct Host requestHost;
    struct Request request;
    struct timeval tv;
    struct Response response;

    int dnsResponse, sfd;
    char * requestString;
    size_t requestStringSize;

    tv.tv_sec = 3;      // 3 Secs Timeout
    tv.tv_usec = 0;     // Not init'ing this can cause strange errors

    // Set getAddrInfo struct
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;    /* Allow IPv4 or IPv6 */
    hints.ai_socktype = 0;          /* Datagram socket */
    hints.ai_flags = 0;
    hints.ai_protocol = 0;          /* Any protocol */

    // Processes the URL
    processURL(&requestHost, argv[1]);
    memset(&request, 0, sizeof(struct Request));
    request.host = requestHost;

    // Do DNS lookup
    dnsResponse = getaddrinfo(requestHost.hostname, requestHost.portNum, &hints, &result);

    // DNS lookup fail
    if (dnsResponse != 0) {
        DieWithSystemMessage("getaddrinfo() failed");
    }

    // Generate the request string
    if(!request.method) {
        request.method = (char *)malloc(4);
        strcpy(request.method, "GET");
    }
    requestStringSize = snprintf(NULL, 0, "%s %s HTTP/1.1\r\nHost: %s\r\n\r\n", request.method, request.host.serverPath, request.host.hostname) + 1;
    requestString = (char *)malloc(requestStringSize);
    snprintf(requestString, requestStringSize, "%s %s HTTP/1.1\r\nHost: %s\r\n\r\n", request.method, request.host.serverPath, request.host.hostname);

    // loop through address structs
    for (rp = result; rp != NULL; rp = rp->ai_next) {

        sfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (sfd == -1) {
            continue;
        }

        // set timeout for socket
        setsockopt(sfd, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv, sizeof(struct timeval));

        // Successfully Connected
        if (connect(sfd, rp->ai_addr, rp->ai_addrlen) == 0) {
            //printf("errno: %s\n", strerror(errno));

            if(equals(strerror(errno), "Connection refused")) {
                printf("1\n");
                exit(1);
            }

            // Initialize response struct
            memset(&response, 0, sizeof(struct Response));
            response.headerRead = false;
            //response.body = (char *)malloc(1);

            // Send the HTTP request to the server
            if (send(sfd, requestString, strlen(requestString), 0) < 0) {
                DieWithSystemMessage("send() failed");
            }

            // I/O buffer
            char buffer[BUFSIZE];

            int numtries = 0;

            // Continue to recv until the data is complete
            while (!httpResponseComplete(&response)) {

                // Receive up to the buffer size
                ssize_t numBytes = recv(sfd, buffer, BUFSIZE, 0);
                if(numBytes >= 0) {
                    buffer[numBytes] = '\0';
                }

                procBuffer(&response, buffer, &request);

                // If the server is no longer returning any more but the system expects more
                if (numBytes == 0) {
                    numtries++;

                    if(numtries == 20) {

                        //printf("%ld %ld\n", response.header.contentLength, strlen(response.body));

                        if(response.headerRead) {
                            if(strlen(response.body) == 0) {
                                fputs("5\n", stdout);     // Print the echo buffer
                                exit(5);
                            }
                            else {
                                fputs("6\n", stdout);     // Print the echo buffer
                                exit(6);
                            }
                        }
                        else {
                            fputs("2\n", stdout);     // Print the echo buffer
                            exit(2);
                        }

                    }
                }
                else {
                    numtries = 0;
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


    //printf("header: %s\n", response.header.header);
    //printf("body: %s\n", response.body);
    //printf("%ld %ld\n", response.header.contentLength, strlen(response.body));

    // No longer needed
    freeaddrinfo(result);
    free(requestString);

    fputs("0", stdout);     // Print the echo buffer
    fputc('\n', stdout); // Print a final linefeed
    return 0;
}
