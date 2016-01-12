# CSE 124 : Winter 2016 : Homework 2

## Objective

In homework 2, you are going to create a client/server application used to get the time and date. This application is based on the "echo" server from Donahoo and Calvert chapter 2, which we are going to go through during class.

## Time/date server

You will write both a client and a server. The client connects to the server, and requests either the time or date. The server waits for connections, and when one comes in, it replies back with either the current time, or the current date, as requested by the client.

Client: The client program is invoked in one of the following two ways:

```
./client time <server_ip_address> <server_port_number>
```

or

```
./client date <server_ip_address> <server_port_number>
```

server_ip_address specifies the destination server's IP address, and server_port_number specifies the destination TCP port. For example:

```
$ ./client time 127.0.0.1 9876
07:30:25
$ sleep 3
$ ./client time 127.0.0.1 9876
07:30:28
$ ./client date 127.0.0.1 9876
2015-01-05
```

requests the time/date from the local machine (using the localhost address of 127.0.0.1), on port 9876. The program should print out the answer.

Server: The server program is invoked in the following way:

```
./server port
```

port specifies the port that the server will use to accept connections. For example: `$ ./server 9876` indicates that the server should listen for incoming connections on port 9876.

## Time/date format

You will likely find the strftime function (defined in time.h) to be very useful in getting the date/time. We will use the ISO 8601 extended time format to represent the current time, and the ISO 8601 date format to represent the date.

The ISO 8601 extended time format is: [hh]:[mm]:[ss], where hh is the zero-padded hour between 00 and 23, mm is the zero-padded minute between 00 and 59, and ss is the zero-padded second between 00 and 59.

Time examples:

- 07:30:00: 7:30am
- 07:30:25: 25 seconds after 7:30am
- 19:30:00: 7:30pm
- 23:59:59: 1 second before midnight

The ISO 8601 date format is: [YYYY]-[MM]-[DD], where YYYY is the four-digit year, MM is the zero-padded two digit month between 01 and 12, and DD is zero-padded day between 01 and 31 (depending on the month).

Date examples:

- 2016-01-05: January 5th, 2016
- 2016-05-01: May 1st, 2016
- 1999-12-31: December 31st, 1999

## Logistics

You will turn in HW2 using your GitHub account. Within your repository, create a 'homework' directory, and under that a 'hw2' directory. Note the lowercase capitalization.

Include a Makefile that can be used to build your code by simply typing 'make'. The result should be two applications: one called 'client' and one called 'server', invoked via the above command line arguments. We should be able to check out your repository, then execute your code with these commands:

```
$ cd <repository_directory>
repository_directory $ cd homework/hw2
hw2 $ make
hw2 $ ./server 9876
hw2 $ ./client time 127.0.0.1 9876
hw2 $ ./client date 127.0.0.1 9876
```

At the very least, your repository should contain the following files. You will of course have other files there too (e.g., your .c and .h files).

```
cse124_github/
|-- homework
    |-- hw2
        |-- client
        |-- Makefile
        |-- server

2 directories, 3 files
```

The due date/time are specified on the course syllabus.

- Update (Jan 8): Limit 'stdout' to what is specified above. You can print whatever you want to 'stderr' for debugging purposes.
- Update (Jan 9): Corrected a typo under 'Logistics'