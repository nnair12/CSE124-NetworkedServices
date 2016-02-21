# CSE 124 : Winter 2016 : Project 4 Specifications

## Project overview

The goal of this project is to demonstrate the effect of "tail latency" on a simple networked system.

**General learning objectives:**

* Measuring and comparing performance of parallel operations in a distributed system

**Specific learning objectives:**

* Using git for source code management
* Instantiating observations from a random variable
* Emulating artificial delay

This project should be done individually. The due date is listed on the course syllabus.

## Project description

In this project, you will be measuring the performance of a distributed application under three scenarios:

1. Scenario 1: A single client issues _N_ independent, non-overlapping requests to a network service.
2. Scenario 2: A single client issues _P_ parallel requests to a network service, waiting until all responses have been returned before considering the operation complete.
3. Scenario 3: A single client issues _P_ parallel requests to a network service, waiting until the first 90% of the responses have been returned before considering the operation complete.

## DelayMe: An emulated delay service

Your first task is to implement the networked service under test, which will be called _DelayMe_. Clients connecting to the DelayMe service send a single 'delay' command (specified below). The DelayMe server then waits the appropriate amount of time, specified by the delay command, before returning the string "bye" back to the client.

**Delay distribution**: The DelayMe server waits a random amount of milliseconds, based on a Gaussian/Normal distribution N(mu,sigma), before returning the response to the client. If the observed value of the random distribution is negative, then simply "round" that value up to 0 (i.e., don't wait).

**DelayMe protocol (client->server)**: A request (from the client to the server) consists of:

* `delay  mu  sigma`

Where:

* `delay`: a static token
* mu: the mean/expectation of the normal distribution (a whole, non-negative number)
* sigma: the standard deviation of the normal distribution(a whole, non-negative number)

**DelayMe protocol (server->client)**: A response (from the server to the client) consists of:

* bye

Where:

* bye: a static token

## Client

You need to invoke your server using a client program that can time the request/response latency to ~1 millisecond timing accuracy. You can implement this client in the language or tool of your choosing, as long as the timing accuracy of the client is precise to 1 millisecond.

## Protocol examples

### Example 1

A client sends a delay value of N(90,7) to the server.

### Example 2

A client sends a delay value of N(80,1) to the server. The server waits a random amount of time drawn from a normally-distributed random value with mean 80 and standard deviation 1.

## Experiments

### Experiment 1: N independent requests

In this experiment you are going to measure the average response time of a single client issuing N independent, non-overlapping requests to the DelayMe service, with a mean of 90 milliseconds, and a standard deviation of 20 milliseconds. You should run the experiment for N=

1. 10
2. 20
3. 30

For each N value in this experiment, calculate the mean service time. You will calculate three values.

### Experiment 2: P parallel requests, all must complete

In this experiment you are going to measure the response time of a single client issuing P parallel requests to the DelayMe service, with a mean of 90 milliseconds, and a standard deviation of 20 milliseconds. Your client must wait until *all* responses are returned before considering the operation complete. You should run the experiment for P=

1. 10
2. 20
3. 30

For each P value in this experiment, calculate the time it takes to receive *all* responses. You will report three values.

### Experiment 3: P parallel requests, 90% must complete

In this experiment you are going to measure the response time of a single client issuing P parallel requests to the DelayMe service, with a mean of 90 milliseconds, and a standard deviation of 20 milliseconds. Your client only has to wait for the first 90% of the responses to be returned before considering the operation complete. You should run the experiment for P=

1. 10
2. 20
3. 30

For each P value in this experiment, calculate the time it takes to receive the first 90% of the responses. You will report three values.

### Response question

In a paragraph, describe the impact that tail latency has on parallelism within a distributed system. What was the effect of "shedding" the slowest 10% of the requests (the so-called stragglers)?

## Details

Make sure to issue the requests from a client on the same machines as your DelayMe server, to eliminate variance from the network itself.

You can instantiate values from a normally-distributed random variable using the `numpy.random.normal` command in Python.

## Grading

* Experiment 1 (2 points)
* Experiment 2 (2 points)
* Experiment 3 (2 points)
* Response question (4 points)

## Project 4 Submission guidelines

You will submit this project to your CSE 124 GitHub account. You will need to submit your results in **a single PDF file**.

You will also submit your server code in a directory called server, and your client code in a directory called client.

Include in your PDF file (1) your name, (2) your student ID number, and (3) your GitHub ID

```
_cse124/
|-- project
    |-- proj4
        |-- report.pdf
        |-- client
            |-- DelayClient.java
        |-- server
            |-- DelayMe.py

4 directories, 3 files
```

Your report will list the values from the three experiments, and also include your response question answer. You do not need to graph any data for this experiment. Note that the java and python files above are just an example--you can use any language you want for this project.

## Revisions

1. Feb 18: Removed mention of netcat.
2. Feb 15: Initial version

[1]: http://cseweb.ucsd.edu/example1.png
[2]: http://cseweb.ucsd.edu/example2.png