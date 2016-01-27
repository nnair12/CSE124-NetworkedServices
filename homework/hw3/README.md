# CSE 124 : Winter 2016 : Homework 3

## Objective

In homework 3, you are going to modify the time/date server you created in HW2 so that it supports multiple concurrent clients.

## Protocol

Your client must send the string 'time' or 'date' to the server, which will reply back with the time or date in the proper format, as indicated. We will be testing your server with our own client, and so make sure that you send 'time' or 'date' and not a different, custom protocol.

## Concurrency

Modify your server so that each client is handled by a separate thread. You are free to choose between (1) a model where a new thread is created and spawned when a request comes in, and (2) a model where a set of threads already exist in the system (a thread pool), and the incoming connection is simply passed off to one of those threads for handling.

## Testing advice

To test your server, one possible option is to open a terminal window and telnet to your server, then issue part of a request (but not the full request). In a different terminal window, telnet to the same server, and issue a different request (and get the response). Then go back to your original terminal and finish the first request (and get the response). That's a signal that your design is working.

```
Terminal 1:
$ telnet localhost 9876
ti
```

Switch over to Terminal 2

```
Terminal 2:
$ telnet localhost 9876
date
2015-01-05
$
```

Switch back to Terminal 1

```
Terminal 1:
me
07:30:28
```

## Logistics

You will turn in HW3 using your GitHub account as before. Within your repository, which should be <github_id>_cse124, create a 'homework' directory, and under that a 'hw3' directory. Note the lowercase capitalization.
Include a Makefile that can be used to build your code by simply typing 'make'. The result should be two applications: one called 'client' and one called 'server', invoked via the above command line arguments. We should be able to check out your repository, then execute your code with these commands:

```
$ cd <github_id>_cse124
<github_id>_cse124 $ cd homework/hw3
hw2 $ make
hw2 $ ./server 9876
hw2 $ ./client time 127.0.0.1 9876
hw2 $ ./client date 127.0.0.1 9876
```

At the very least, your repository should contain the following files. You will of course have other files there too (e.g., your .c and .h files).

```
gmporter_cse124/
|-- homework
    |-- hw2
        |-- Makefile

2 directories, 1 files
```

After calling make, you'll have the following:

```
gmporter_cse124/
|-- homework
    |-- hw2
        |-- client
        |-- Makefile
        |-- server

2 directories, 3 files
```

The due date/time are specified on the course syllabus.

Updates: Jan 26: Clarified the time/date protocol