## Project 6: Peer-to-peer TritonTransfer with RPCs

### Project overview

The goal of project 6 is to extend TritonTransfer to support a peer-to-peer delivery mode.

**General learning objectives:**

* Designing a protocol for transferring blocks of a file from one peer to another
* Using an RPC framework and stub compiler

**Specific learning objectives:**

* Using git for source code management
* Apache Thrift
* Extending a simplified version of the Dropbox protocol

This project should be done individually. The due date is listed on the course syllabus.

### TritonTransfer-p2p: Project description

In this extension to project 5, there is now going to be two kinds of servers--one _metadata server_, and one or more _block servers_. The metadata server keeps a list of all the hashes that make up a file. Block servers store and serve out blocks of data. Clients issue `uploadFile` and `downloadFile` calls to the metadata server, yet issue `uploadBlock` and `downloadBlock` calls to the block servers. Unlike in project 5, this peer-to-peer version does not need to store any persistent data--all file metadata and data blocks are to be kept in memory.

#### Client interface

The client interface is the same as in project 5.

#### Metadata server interface

The metadata server is invoked as follows:

```
$ tt-md-server
```

1. **server_port**: The port on the server

#### Block server interface

The block server is invoked as follows:

```
$ tt-md-server <server_port>
```

1. **server_port**: The port to listen for incoming connections
2. **metadata_server**: The hostname/ip address of the metadata server.
3. **metadata_port**: The port of the metadata server.

### Locating blocks

In this project, you are going to start up **three** block servers (can be on separate servers, or all on the same server running on different TCP ports). Each block of data should be stored on **two** block servers. That way, if one block server were to fail, you would not lose any data.

You have several choices for how to locate blocks of data:

* You can have the client query all of the block servers for each block of data requested. A design question becomes how the client finds the block servers
* Alternatively, you can store the locations in the metadata server, which will be returned by the uploadFile and downloadFile calls. If you store the locations in the metadata server, then the uploadFile call will need to be modified in some way so that the metadata server can know where the blocks are. A design question becomes whether the client should tell the metadata server, or whether the metadata server makes that selection?
* Note that you can modify the protocols between the client, metadata server, and block servers as needed to implement your block location strategy

#### Implementation Notes

We will only invoke your client and server with valid arguments. All directories will exist, be readable/writable, the ports/arguments will be valid, etc.

Your server does not need to support subdirectories. For example, if you invoke:

```
$ tt-client localhost 9001 upload /var/lib/mydir/myfile.txt
```

Then you should be able to download that file via:

```
$ tt-client localhost 9001 download myfile.txt /tmp
```

And the file will be downloaded and stored in /tmp

### Grading

In this project, you will define your RPC service, your RPC APIs, and any RPC data types inside of a Thrift IDL file (called TritonTransfer.thrift). You can use any language you want for your client and server, assuming that Thrift supports the language.

We will carry out a _failure test_ in which we upload a file to TritonTransfer-p2p, then kill one of the three block servers. Your client should be able to download the file, even with one of the block servers out of commission

* Uploading files/blocks (3 points)
* Downloading files/blocks (3 points)
* Failure test (4 points)

### Project 6 Submission guidelines

You will submit this project to your CSE 124 GitHub account, including your client code, your server code, and your thrift IDL. You should run thrift on your IDL to generate the stubs for your language, and **commit those stubs to your repository**.

```
_cse124/
|-- project
    |-- proj6
        |-- thrift
            |-- TritonTransfer.thrift
        |-- client
            |-- Makefile
            |-- gen-py (or gen-java, gen-cpp, etc)
            |-- (various source code files)
        |-- server
            |-- Makefile
            |-- gen-py (or gen-java, gen-cpp, etc)
            |-- (various source code files)
```

We should be able to check out your code, go into the client directory and type 'make' and have your client built. We should then be able to go into the server directory, type 'make', and have your server built.

Note that you may certainly reuse code between projects 5 and 6, but please commit code separately.

### Revisions

1. Feb 25: Initial version

[1]: http://cseweb.ucsd.edu/block_hash_fig.png
[2]: http://cseweb.ucsd.edu/proto_example_1.png
[3]: http://cseweb.ucsd.edu/proto_example_down.png
[4]: http://cseweb.ucsd.edu/proto_upload_popular.png