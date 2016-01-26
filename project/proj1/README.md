# CSE 124 : Winter 2016 : Project 1 : Build a "testing" HTTP client

## Project overview

The goal of this project is to build a simple web client that can send requests (and process responses) from an HTTP web server. What differentiates your client from existing ones (such as curl or wget) is that your client is going to check to make sure the HTTP protocol is strictly enforced. In other words, your client will test the server to make sure it is implementing HTTP correctly. This tool will come in handy when you build your own server during project 2.

### General learning objectives:

- How to implement a correct API
- Methodology for testing API correctness

### Specific learning objectives:

- Program UNIX network sockets (send, recv, connect, close, ...)
- Reading and interpreting a protocol spec and Internet RFC
- Developing test cases for protocol correctness
- Using git for source code management

This project should be done individually. The due date is listed on the course syllabus.

## Project details

At a high level, a web client connects to a server socket on a web server, and uses a simple text-based protocol to retrieve files or other information from the server. For example, you might try the following command from a UNIX machine:

```
$ telnet www.ucsd.edu 80 
GET /index.html HTTP/1.1\n 
Host: www.ucsd.edu\n
\n
```

(type two carriage returns after the "Host" header). This will return to you (on the command line) the html representing the "front page" of the UCSD web page:

```
HTTP/1.1 200 OK
Date: Thu, 07 Jan 2016 04:09:11 GMT
Server: Apache/2
Last-Modified: Wed, 06 Jan 2016 18:00:13 GMT
ETag: "6a46-528ae20302540"
Accept-Ranges: bytes
Content-Length: 27206
Content-Type: text/html; charset=UTF-8

<!DOCTYPE html>
<html lang="en">
(rest of html web page follows...)
```

For this assignment, you will need to support a (pretty small) subset of the HTTP 1.1 protocol to interact with existing web servers. Your client will need to be able to request HTML files as well as in-line images (jpg and png).

At a high level, your web client will be structured something like the following:

```
Initialize:
	Take an HTTP URL as a command-line argument,
	including the TCP port number (80 is assumed if not provided).
	$ ./http-client http://www.ucsd.edu:80/index.html

Connect and send:
	Create a TCP connection to the web server and issue a well-formed HTTP
	request for the appropriate content.

Receive:
	Receive the HTTP response headers and content (e.g., HTML page or image).
	Write the contents to a local file with the appropriate name (e.g.,
	index.html or foo.jpg)

Close:
	When all the content is requested and received, close the connection.

Error reporting:
	Your program should print a '0' to stdout if the web server followed
	the HTTP spec correctly, or else your program should print
	a positive, non-zero error code that signals the type of error
	that you found (see below).  Your program may print whatever
	messages you want to stderr (we won't check those).
```

Your client does not need to support concurrency or multithreading. You may choose from C or C++ to build your web client but you must do it in a Unix-like environment with the sockets API we've been using in class (e.g., no HTTP libraries).

Your client should support:

- The GET request method (no other methods need be supported)
- The 'Host' and 'User-Agent' request headers
- The following HTTP response status codes:
	- 200 OK
	- 400 Bad request
	- 403 Forbidden
	- 404 Not found
- The 'Server', 'Content-Type', and 'Content-Length' response headers
- The content of the response (either HTML or binary data if the response is an image)

Your client program will only need to support this subset of HTTP/1.1.

## Resources

There are a variety of resources online that can be of help:

1. Peterson and Davie, Computer Networks, Chapter 9.1.2. I've put a PDF copy of this section of the book in the Piazza Resources section.
2. The authoritative source for all the details of the HTTP implementation is RFC 7230 Hypertext Transfer Protocol (HTTP/1.1): Message Syntax and Routing. This document is very long, and you only need to restrict your attention to these relevant sections:
	- 1: Introduction
	- 2.1: Client/server messenging
	- 3.0: Message format
	- 3.1: Start line
	- 3.2.0 Header fields (only... don't worry about all the corner cases and details in subsections 3.2.1, 3.2.2, etc)
	- 3.3.0 and 3.3.2: Message body and Content-Length. You only need to support Content-Length as we are not going to use transfer encodings
	- 5.4: Host header field
3. RFC 7231 Hypertext Transfer Protocol (HTTP/1.1): Semantics and Content. Only these subsections:
	- 4.3.1: GET
	- 5.5.3: User-Agent
	- 6.3.1: 200 OK
	- 6.5.1: 400 Bad request
	- 6.5.3: 403 Forbidden
	- 6.5.4: 404 Not found
4. HTTP Made Really Easy
	- http://www.jmarshall.com/easy/http/
5. Eddie Kohler's guide to using Git
	- http://cs61.seas.harvard.edu/wiki/2015/Git

## Grading

The course staff will provide 10 instances of a web server, each on a different TCP port (details to be provided shortly). For example, we may run server instances on ports 8000, 8001, 8002, ..., 8009. You are to run your program against each port and determine whether the web server instance running on that port (1) implements the HTTP spec correctly, or (2) does not implement the HTTP correctly. In the latter case, you must figure out what the error condition is. We will 'label' these ports with the associated errors so that you can check that your solution works (for these 10 cases at least). When we grade your submission, we will test against separate ports that you will not have access to.

You may request the following documents from the server. We are providing these for you so that you can check to make sure that what the server returns is actually the correct content.

1. <doc_root>/index.html (returns Content-Type text/html)
2. <doc_root>/kitty1.jpg (returns Content-Type image/jpeg)
3. <doc_root>/kitty2.png (returns Content-Type image/png)
4. <doc_root>/subdir/file1.html (return Content-Type text/html)
5. <doc_root>/forbidden/file2.html (should return a 403 Forbidden error)
6. <doc_root>/not_here.html (should return a 404 not found)

These files are available here (http://cseweb.ucsd.edu/~gmporter/classes/wi16/cse124/projects/proj1/doc_root/).

The possible error conditions (and their associated numerical codes to be printed to stdout) are:

Code number	| Code					| Description
------------|-----------------------|-------------
0			| OK					| The server works correctly
1			| Bad_socket			| The server socket is not setup or not accepting connections.
2			| Premature_close		| The server closed the connection before processing the full request(s).
3			| Bad_server_status		| The server's status line is malformed, or returns a response code not indicated in the above project spec.
4			| Bad_response_headers	| The response headers are malformed.
5			| Bad_response_body		| There isn't a response body, or the blank line between the response headers and the body isn't there, or the response body does not have the correct data in it. For example, not all of the HTML page or image was returned, or some of the content was corrupted and doesn't match the reference file.
6			| Wrong_content_length	| The Content-length field indicates a length that is not correct.
7			| Wrong_content_type	| The Content-Type field doesn't match the content.

## Project 1 Submission guidelines

You will submit your first project to your CSE 124 GitHub account. You should include a Makefile that will build your project, producing a binary executable should be called 'http-client'. You don't have to commit your binary, just the Makefile and associated source files.

```
cse124_github/
|-- project
    |-- proj1
        |-- http-client
        |-- Makefile

2 directories, 2 files
```

## Grading

We will run your http testing client against 10 ports, giving you a point if you get the error condition correct, and no points if that condition is incorrect (for a total of 10 points). Don't forget that error condition 0 indicates that the HTTP server correctly implements the protocol.

## Hints/tips

You can manually examine the response from a web server by 'telnet'ing to the port and manually issuing a request to see the response (see above for an example).

## Revisions

Jan 11: removed mention of pipelining
Updated the spec to remove pipelining and persistent connections, and replaced the 'Wrong_status_code' error code with 'Wrong_content_length'.
January 7, 2015: Initial revision

## Credits

kitty1.jpg used via a Creative Commons Attribution-No Derivative Works 3.0 License.
kitty2.png used via a Creative Commons Attribution-Share Alike 3.0 Unported license.