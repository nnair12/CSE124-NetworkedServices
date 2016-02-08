#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <inttypes.h>
#include <ctype.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <sys/sendfile.h>
#include "Practical.h"

/**
 * http-server.c
 *
 * At a high level, a web server listens for connections on a socket
 * (bound to a specific port on a host machine). Clients connect to
 * this socket and use a simple text-based protocol to retrieve files
 * from the server.
 *
 * Author: Daniel Kao
 * PID: A10546439
 * Date: 2/2/16
 *
 */

int servPort;
struct stat documentRoot;
char * documentRootPath;

struct Host {
    char * hostname;
    char * portNum;
    char * serverPath;
    char * contentType;
};

struct Request {
    struct Host host;
    char * method;
    char * userAgent;
    char * requestHeader;
    char * requestBody;
};

// Structure of arguments to pass to client thread
struct ThreadArgs {
    int clntSock; // Socket descriptor for client
};

// Maximum outstanding connection requests
static const int MAXPENDING = 500;


/**
 * equals takes two char arrays and returns true if they are equal.
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
 *
 */
void procBuffer(struct Request* request, char buffer[], ssize_t numBytes, bool * headerComplete) {

    // Save buffer to request header
    if(request->requestHeader == NULL || !strstr(request->requestHeader, "\r\n\r\n")) {

        // If buffer contains header end
        if(strstr(buffer, "\r\n\r\n")) {

            char * end = strstr(buffer, "\r\n\r\n");

            // if the request has not yet been set
            // Copy buffer up to the double crlf into the header
            if(request->requestHeader == NULL) {
                request->requestHeader = malloc(end - buffer);
                memcpy(request->requestHeader, buffer, end - buffer);
            }
            else {
                size_t newlen = (end - buffer) + strlen(request->requestHeader);
                char * temp = malloc(newlen);
                memcpy(temp, request->requestHeader, strlen(request->requestHeader));
                free(request->requestHeader);
                request->requestHeader = temp;
            }
            *headerComplete = true;
        }

        // If buffer does not contain header end
        else {

            // if the request has not yet been set
            // Copy buffer into request header
            if(request->requestHeader == NULL) {
                request->requestHeader = malloc((size_t)numBytes);
                memcpy(request->requestHeader, buffer, numBytes);
            }
            else {
                size_t newlen = numBytes + strlen(request->requestHeader);
                char * temp = malloc(newlen);

                memcpy(temp, request->requestHeader, strlen(request->requestHeader));
                strcpy(temp + strlen(request->requestHeader), buffer);

                request->requestHeader = temp;
            }

            if(strstr(request->requestHeader, "\r\n\r\n")) {
                *headerComplete = true;
            }
        }
    }

}

/**
 * Processes the header string in the request object.
 *
 */
void procHeader(struct Request* request, bool * headerValid) {

    // Read header into Request struct
    const char crlf[3] = "\r\n";
    const char sp[2] = " ";
    char *requestLine;
    char *line;

    // Get the first line
    requestLine = strtok(request->requestHeader, crlf);

    // Get subsequent lines
    line = strtok(NULL, crlf);

    // Walk through other lines
    while( line != NULL )
    {
        // Process line host
        if(indexOfIgnoreCase(line, "host:") == 0) {

            // Copy host
            char* host = malloc(strlen(line) - indexOfIgnoreCase(line, ":") + 1);
            strcpy(host, &line[indexOfIgnoreCase(line, ":") + 1]);

            if(isspace(host[0])) {
                host++;
            }

            // Check for port number
            if(indexOfIgnoreCase(host, ":") >= 0) {
                request->host.hostname = malloc((size_t)indexOfIgnoreCase(host, ":"));
                memcpy(request->host.hostname, host, indexOfIgnoreCase(host, ":"));
                request->host.portNum = malloc(strlen(&host[indexOfIgnoreCase(host, ":") + 1]));
                strcpy(request->host.portNum, &host[indexOfIgnoreCase(host, ":") + 1]);
            }
            else {
                request->host.hostname = malloc(strlen(host));
                strcpy(request->host.hostname, host);
                request->host.portNum = malloc(3);
                strcpy(request->host.portNum, "80");
            }
        }

        // Process line host
        if(indexOfIgnoreCase(line, "user-agent:") == 0) {
            request->userAgent = malloc(strlen(line) - indexOfIgnoreCase(line, ":") + 1);
            strcpy(request->userAgent, &line[indexOfIgnoreCase(line, ":") + 1]);
        }

        // TODO make sure that the line is syntactically valid

        line = strtok(NULL, crlf);
    }

    char *method = strtok(requestLine, sp);
    char *path = strtok(NULL, sp);
    char *httpVersion = strtok(NULL, sp);

    // If header info is incomplete
    if(method == NULL || path == NULL || httpVersion == NULL
       || request->host.hostname == NULL || request->userAgent == NULL || strtok(NULL, sp)) {
        return;
    }

    if(equals(path, "/")) {
        request->host.serverPath = malloc(12);
        strcpy(request->host.serverPath, "/index.html");
        request->host.contentType = malloc(10);
        strcpy(request->host.contentType, "text/html");
    }
    else {
        request->host.serverPath = path;

        if(indexOfIgnoreCase(request->host.serverPath, ".html") > 0) {
            request->host.contentType = malloc(10);
            strcpy(request->host.contentType, "text/html");
        }

        if(indexOfIgnoreCase(request->host.serverPath, ".jpg") > 0 || indexOfIgnoreCase(request->host.serverPath, ".jpeg") > 0) {
            request->host.contentType = malloc(11);
            strcpy(request->host.contentType, "image/jpeg");
        }

        if(indexOfIgnoreCase(request->host.serverPath, ".png") > 0) {
            request->host.contentType = malloc(10);
            strcpy(request->host.contentType, "image/png");
        }
    }
    request->method = method;

    *headerValid = true;
}


void sendHeader(int clntSocket, int errorCode, int contentLength, char * contentType) {

    char * codeString;

    switch (errorCode) {
        case 200:
            codeString = malloc(strlen("OK"));
            strcpy(codeString, "OK");
            break;
        case 400:
            codeString = malloc(strlen("Bad Request"));
            strcpy(codeString, "Bad Request");
            break;
        case 403:
            codeString = malloc(strlen("Forbidden"));
            strcpy(codeString, "Forbidden");
            break;
        case 404:
            codeString = malloc(strlen("Not Found"));
            strcpy(codeString, "Not Found");
            break;
    }

    // Send header
    int responseHeaderSize = snprintf(NULL, 0, "HTTP/1.1 %d %s\r\nServer: http-server-dckao\r\nContent-Type: %s\r\nContent-Length: %d\r\nConnection: close\r\n\r\n", errorCode, codeString, contentType, contentLength) + 1;
    char * responseHeader = (char *)malloc(responseHeaderSize);
    snprintf(responseHeader, responseHeaderSize, "HTTP/1.1 %d %s\r\nServer: http-server-dckao\r\nContent-Type: %s\r\nContent-Length: %d\r\nConnection: close\r\n\r\n", errorCode, codeString, contentType, contentLength);

    // Send the HTTP request to the server
    send(clntSocket, responseHeader, responseHeaderSize - 1, 0);

    free(codeString);
}


/**
 *
 */
void sendError(int clntSocket, int errorCode) {

    int bodySize = snprintf(NULL, 0, "<h1>%d Error</h1>", errorCode) + 1;
    char * responseBody = (char *)malloc(bodySize);
    snprintf(responseBody, bodySize, "<h1>%d Error</h1>", errorCode);

    // Sends the specified error code to the client
    sendHeader(clntSocket, errorCode, bodySize, "text/html");

    // send body
    send(clntSocket, responseBody, bodySize, 0);

    close(clntSocket); // Close client socket
}

/**
 * Accepts the connection to the client.
 *
 */
int AcceptTCPConnection(int servSock) {

    // Client address
    struct sockaddr_storage clntAddr;

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


/**
 * Takes a pointer to a client socket, and receives and processes the request passed.
 *
 */
void HandleTCPClient(int clntSocket) {

    // Buffer for echo string
    char buffer[BUFSIZE];
    bool headerComplete = false;
    bool headerValid = false;

    ssize_t numBytesRcvd;
    struct Request request;
    memset(&request, 0, sizeof(struct Request));

    // Run recv in loop to get the whole request
    while(!headerComplete) {
        memset(&buffer, '\0', BUFSIZE);
        // Receive message from client
        numBytesRcvd = recv(clntSocket, buffer, BUFSIZE, 0);
        if (numBytesRcvd < 0)
            DieWithSystemMessage("recv() failed");

        // Parse request
        procBuffer(&request, buffer, numBytesRcvd, &headerComplete);
    }

    procHeader(&request, &headerValid);

    // Send 400 error if request is malformed
    if(!headerValid) {
        sendError(clntSocket, 400);
        return;
    }

    // Send 404 error if file does not exist
    struct stat document;
    memset(&document, 0, sizeof(stat));

    // Append documentRoot to serverPath
    char * filepath = malloc(strlen(documentRootPath) + strlen(request.host.serverPath));
    memcpy(filepath, documentRootPath, strlen(documentRootPath));
    strcpy(filepath + strlen(documentRootPath), request.host.serverPath);

    // Get file descriptor of the file that will be sent
    char abspath[1024];
    realpath(filepath, abspath);

    if (stat(abspath, &document) == -1) {
        sendError(clntSocket, 404);
        return;
    }

    // Send 403 error if file is not world readable
    if(!(document.st_mode & S_IROTH)) {
        sendError(clntSocket, 403);
        return;
    }

    if(access(abspath, F_OK ) == -1 ) {
        sendError(clntSocket, 404);
        return;
    }

    FILE * file = fopen(abspath, "r");

    // Send response header to the client
    sendHeader(clntSocket, 200, document.st_size, request.host.contentType);

    // Use sendFile() to send the file to the client
    off_t sent = 0;
    sendfile(clntSocket, fileno(file), &sent, document.st_size);

    close(clntSocket); // Close client socket
}


/**
 * The thread that gets spawned whenever a client connects.
 *
 */
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

    if (argc != 3) // Test for correct number of arguments
        DieWithUserMessage("Parameter(s)", "<Server Port> <Document Root>");

    servPort = atoi(argv[1]); // First arg:  local port

    // Save the document root.
    documentRootPath = argv[2];
    memset(&documentRoot, 0, sizeof(stat));       // Zero out structure
    if (stat(argv[2], &documentRoot) == -1) {
        DieWithSystemMessage("stat() failed");
    }

    // Error if root is not a directory
    if(!S_ISDIR(documentRoot.st_mode)) {
        DieWithSystemMessage("Root is not a Directory");
    }

    // Error if root is not world readable
    if(!(documentRoot.st_mode & S_IROTH)) {
        DieWithSystemMessage("Root is not a World Readable");
    }

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

    while (true) { // Run forever

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
