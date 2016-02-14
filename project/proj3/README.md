# CSE 124 : Winter 2016 : Project 3 : Quantifying performance

## Project overview

The goal of this project is to quantify the performance of a networked system, and communicate those measurements in an easy-to-understand manner.

### General learning objectives:

- Methodology for testing the performance of a network protocol implementation
- Techniques for communicating scientific results

### Specific learning objectives:

- Using git for source code management
- Measuring the performance of a protocol implementation in terms of latency, throughput (in requests/sec), and throughput (in terms of MB/s of data transferred).
- Generating graphs of system performance

This project should be done individually. The due date is listed on the course syllabus.

## Project description

In this project, you will be measuring the performance of the webserver you created in project 2.

You will carry out two basic experiments using the two "seed" servers that were assigned to your for this course. For each experiment, you will collect data as indicated, and will produce a set of graphs to visually depict the results. You will also answer a small set of questions about those graphs. All of these results will go into a report that you will submit through GitHub.

You do not need to create the input files--we have created them for you (see the details section of the spec).

### Experiment 1: Latency

In this experiment you are going to measure the response times of web requests.

#### Experiment 1a: Varying file sizes

Vary the size of the document that you request. The sizes you should use are:

- 1 KB
- 10 KB
- 100 KB
- 1 MB
- 10 MB

Issue 100 requests for each file size, and compute the mean and standard deviation of the response time. Produce a line graph where the x-axis is the file size, and the y-axis is the mean response time. Annotate the points on your graph with error bars corresponding to the standard deviation you've calculated.

#### Experiment 1b: Varying concurrency

Vary the number of concurrent clients to your webserver, keeping the file size constant. The levels of concurrency you should use are:

- 1 client
- 5 clients
- 10 clients

Issue 100 requests (in total) for a document of size 1 KB, and compute the mean and standard deviation of the response time. Produce a line graph where the x-axis is the amount of concurrency, and the y-axis is the mean response time. Annotate the points on your graph with error bars corresponding to the standard deviation you've calculated.

#### Experiment 1 questions

Please briefly answer the following questions related to experiment 1:

- How many requests/second can your webserver serve out under the best of circumstances?
- Which determines the throughput in terms of requests/second the most, concurrency level or file size?

### Experiment 2: Throughput

In this experiment you are going to measure the throughput of your server. The 'total throughput' includes both the HTTP response headers as well as the document, whereas the 'goodput' includes just the document itself. You can measure either quantity for this experiment.

#### Experiment 2a: Varying file sizes

Vary the size of the document that you request. The sizes you should use are:

- 1 KB
- 10 KB
- 100 KB
- 1 MB
- 10 MB

Issue 100 requests for each file size, and compute the mean and standard deviation of the throughput in MB/sec. Produce a line graph where the x-axis is the file size, and the y-axis is the mean throughput in MB/sec. Annotate the points on your graph with error bars corresponding to the standard deviation you've calculated.

#### Experiment 2a: Varying concurrency

Vary the number of concurrent clients to your webserver, keeping the file size constant. The levels of concurrency you should use are:

- 1 client
- 5 clients
- 10 clients

Issue 100 requests (in total) for a document of size 10 MB, and compute the mean and standard deviation of the throughput in MB/sec. Produce a line graph where the x-axis is the amount of concurrency, and the y-axis is the mean throughput. Annotate the points on your graph with error bars corresponding to the standard deviation you've calculated.

#### Experiment 2 questions

Please briefly answer the following questions related to experiment 2:

- In experiments 2a and 2b, did you measure the total throughput or the goodput?
- How many MB/second can your webserver serve out under the best of circumstances?
- When it comes to throughput, what would you say is the limiting bottleneck of your system?
- Which determines the throughput in terms of MB/second the most, concurrency level or file size?

## Details

Make sure to issue the requests from a client on a separate machine from your web server. Each student has been assigned two machines for this reason. Do not access localhost or 127.0.0.1, and don't access the server from your laptop.

You will likely find the ab tool to be useful in carrying out this project. More information can be obtained on the tool by typing man ab. In particular, to compute the standard deviation of a set of requests, look into the -g option.

We have provided a set of input files that you can use, and they are located at `/usr/local/cse124/doc_root`. Please do not copy these to your home directory--simply point your web server to this location so that we don't have 150+ copies of these files everywhere.

## If you had trouble with project 2...

If you had difficulties with building your own webserver as part of project 2, and aren't able to use it to carry out this performance measurement, then you can use our solution, which is located at `/usr/local/cse124/bin/httpd`. That server needs to be run as follows:

```
$ httpd <port> <doc_root> multiprocess
```

Note the word 'multiprocess'.

## Grading

- Experiment 1 (5 points)
    - Experiment 1a (2 points)
    - Experiment 1b (2 points)
- Response questions (1 point)
    - Experiment 2 (5 points)
    - Experiment 2a (2 points)
    - Experiment 2b (2 points)
    - Response questions (1 point)

## Project 3 Submission guidelines

You will submit your third project to your CSE 124 GitHub account. You only need to submit a single PDF file.

Include in your PDF file (1) your name, (2) your student ID number, (3) your GitHub ID, and (4) whether you used your own web server to test, or used the one we provided.

```
<github_id>_cse124/
|-- project
    |-- proj3
        |-- report.pdf

2 directories, 1 files
```

## Hints/tips

To generate the graphs, you can use gnuplot, or Matlab, or Excel, or OpenOffice, or Google Docs, or Boomslang (which was developed by a UCSD student!).

## Revisions

- Feb 5: Initial version
- Feb 6: Added requirement for username, ID, etc to submitted PDF; added info on how to call the provided httpd server