# CSE 124 : Winter 2016 : Project 2 : Build an HTTP server

## Project overview

The goal of this project is to build a simple web server that can receive requests, and send back responses, to web clients.

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

At a high level, a web server listens for connections on a socket (bound to a specific port on a host machine). Clients connect to this socket and use a simple text-based protocol to retrieve files from the server. For example, you might try the following command from a UNIX machine:

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

For this assignment, you will need to support a (pretty small) subset of the HTTP 1.1 protocol. Your server will need to be able to serve out HTML files as well as in-line images (jpg and png).

One of the key things to keep in mind in building your web server is that the server is translating relative filenames (such as index.html) to absolute filenames in a local filesystem. For example, you might decide to keep all the files for your server in ~student/cse124/server/files/, which we call the document root. When your server gets a request for index.html, it will prepend the document root to the specified file and determine if the file exists, and if the proper permissions are set on the file (typically the file has to be world readable). If the file does not exist, a file not found error is returned. If a file is present but the proper permissions are not set, a permission denied error is returned. Otherwise, an HTTP OK message is returned along with the contents of a file.

You should also note that web servers typically translate "GET /" to "GET /index.html". That is, index.html is assumed to be the filename if no explicit filename is present. That is why the two URL's "http://www.cs.ucsd.edu" and "http://www.cs.ucsd.edu/index.html" return equivalent results.

When you type a URL into a web browser, it will retrieve the contents of the file. If the file is of type text/html, it will parse the html for embedded links (such as images) and then make separate connections to the web server to retrieve the embedded files. If a web page contains 4 images, a total of five separate connections will be made to the web server to retrieve the html and the four image files. The client handles this--your web server needs to only return one response at a time.

At a high level, your web server will be structured something like the following:

```
Initialize:
	Take a port number and document root as a command-line argument.
	$ ./http-server 8080 /var/lib/www/htdocs

Setup server socket and threading:
	Create a TCP server socket, and arrange so that a thread is spawned
	(or thread in a thread pool is retrieved) when a new connection comes in.

Loop:
	Parse HTTP/1.1 request
	Ensure it is well-formed (return error otherwise)
	Determine which file needs to be accessed to deliver the requested
	  content, and check that file's permissions and other metadata (returning
	  an error if needed).
	Construct the response, including response headers
	Transmit the contents of the file to the client	
	Close the connection
```

If you do not attempt the extra credit, make sure to include a "Connection: close" header in your request/response

You may choose from C or C++ to build your web server but you must do it in a Unix-like environment with the sockets API we've been using in class (e.g., no HTTP libraries).

Your server should support:

- The GET request method (no other methods need be supported)
- The 'Host' and 'User-Agent' request headers
- The following HTTP response status codes:
    - 200 OK
    - 400 Bad request
    - 403 Forbidden
    - 404 Not found
- The 'Server', 'Content-Type', and 'Content-Length' response headers
- The 'Connection: close' response header (if extra credit not attempted)
- The content of the response (either HTML or binary data if the response is an image)

Your server program will only need to support this subset of HTTP/1.1.

Your server program takes two command line arguments: (1) the port to listen for incoming connections, and (2) a path to the document root containing the files that you will serve out (e.g., HTML and image files)

## Resources

There are a variety of resources online that can be of help:

- Peterson and Davie, Computer Networks, Chapter 9.1.2. I've put a PDF copy of this section of the book in the Piazza Resources section.
- The authoritative source for all the details of the HTTP implementation is RFC 7230 Hypertext Transfer Protocol (HTTP/1.1): Message Syntax and Routing. This document is very long, and you only need to restrict your attention to these relevant sections:
    - 1: Introduction
    - 2.1: Client/server messenging
    - 3.0: Message format
    - 3.1: Start line
    - 3.2.0 Header fields (only... don't worry about all the corner cases and details in subsections 3.2.1, 3.2.2, etc)
    - 3.3.0 and 3.3.2: Message body and Content-Length. You only need to support Content-Length as we are not going to use transfer encodings
    - 5.4: Host header field
- RFC 7231 Hypertext Transfer Protocol (HTTP/1.1): Semantics and Content. Only these subsections:
    - 4.3.1: GET
    - 5.5.3: User-Agent
    - 6.3.1: 200 OK
    - 6.5.1: 400 Bad request
    - 6.5.3: 403 Forbidden
    - 6.5.4: 404 Not found
- HTTP Made Really Easy
    - http://www.jmarshall.com/easy/http/
- Eddie Kohler's guide to using Git
    - http://cs61.seas.harvard.edu/wiki/2015/Git

## Grading

- Basic functionality (5 points)
    - Serves out HTML pages based on well-formed requests
    - Serves out images based on well-formed requests
    - Response headers are accurate (e.g., Content-length, Content-type)
- Concurrency (2 points)
    - Correctly handles multiple, concurrent clients
    - Error handling (3 points)
    - Returns 403 for files that are not 'world readable'
    - Returns 404 for files that aren't found
    - Correctly handles malformed HTTP requests
- Extra credit option (2 points)
    - Supports HTTP/1.1 request pipelining

Your server must serve out HTML files, Jpeg and PNG image files. We are providing the following sample files to get you going, however you should be able to support other files as well.

- <doc_root>/index.html (returns Content-Type text/html)
- <doc_root>/kitty1.jpg (returns Content-Type image/jpeg)
- <doc_root>/kitty2.png (returns Content-Type image/png)
- <doc_root>/subdir/file1.html (return Content-Type text/html)
- <doc_root>/forbidden/file2.html (should return a 403 Forbidden error)
- <doc_root>/not_here.html (should return a 404 not found)

These files are available [here](http://cseweb.ucsd.edu/~gmporter/classes/wi16/cse124/projects/proj2/doc_root/).

## Project 2 Submission guidelines

You will submit your second project to your CSE 124 GitHub account. You should include a Makefile that will build your project, producing a binary executable should be called 'http-server'. You don't have to commit your binary, just the Makefile and associated source files.

```
<github_id>_cse124/
|-- project
    |-- proj2
        |-- Makefile

2 directories, 1 files
After calling 'make', you should have at least these files:
<github_id>_cse124/
|-- project
    |-- proj2
        |-- http-server
        |-- Makefile

2 directories, 2 files
```

## Hints/tips

You can manually examine the response from a web server by 'telnet'ing to the port and manually issuing a request to see the response (see above for an example).

## Extra credit

Extend your web server by adding persistent, pipelined request/response support. This means that a client can keep the socket open after they've issued a request in order to issue additional requests. The server should respond with responses, one after the other, over the same socket. The responses should be in the same order as the requests. When the client sends a request with a "Connection: close" request header, the server should close the connection. Alternatively, the server should implement a timeout where the connection is closed after a certain number of seconds have gone by with no new requests.

## Revisions

- Jan 14: Initial version posted

## Credits

- kitty1.jpg used via a Creative Commons Attribution-No Derivative Works 3.0 License.
- kitty2.png used via a Creative Commons Attribution-Share Alike 3.0 Unported license.