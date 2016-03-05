## Project 5: Client-server TritonTransfer with RPCs

### Project overview

The goal of project 5 is to build a simple "DropBox"-like file sharing system that relies on RPC.

**General learning objectives:**

* Designing a protocol for transferring files from client to server, or server to client.
* Using an RPC framework and stub compiler

**Specific learning objectives:**

* Using git for source code management
* Apache Thrift
* Learning a simplified version of the Dropbox protocol

This project should be done individually. The due date is listed on the course syllabus.

### TritonTransfer: Project description

In this project, you will be creating a simplified version of DropBox, called _TritonTransfer_.

TritonTransfer consists of a command-line client and a command-line server. The client can upload or download files from the server, and the server stores the file(s).

#### Client interface

To upload a file, the client is invoked as follows:

```
$ tt-client <server_name> <server_port> upload <filename>
```

1. **server_name**: The hostname/IP of the server
2. **server_port**: The port on the server
3. **upload**: A static token representing an upload operation
4. **filename**: The file to upload (either an absolute or relative path)

**Return**: The client will print either "OK" (if the file was successfully uploaded), or "ERROR" (if there was some kind of error uploading the file).

To download a file, the client is invoked similarly:

```
$ tt-client <server_name> <server_port> download <filename> <download_dir>
```

1. **server_name**: The hostname/IP of the server
2. **server_port**: The port on the server
3. **download**: A static token representing a download operation
4. **filename**: The file to download
5. **download_dir**: The directory to store the downloaded file within
**Return**: The client will print either "OK" (if the file was successfully downloaded), or "ERROR" (if there was some kind of error downloading the file).

#### Server interface

Your server is invoked as follows:

```
$ tt-server <server_port> <file_dir>
```

1. **server_port**: The port on the server
2. **file_dir**: The location on the server used to store uploaded files, and from which to serve out downloaded files to clients

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

### Blocks and hashes

Any uploaded or downloaded file is broken into fixed-size blocks of size 16 KB (except for the last block, which can be smaller than 16KB). A hash value is computed over Each block using SHA-256. We say that a file _f_ consists of blocks <b1,b2,...,bn> with hashes <h1,h2,...,hn>. As we will now see, the file is transferred in these fixed-sized blocks

If you use Python, you can generate hashes of data using the `hashlib.sha256()` command:

Blocks should only be transferred if they aren't already on the client/server. Blocks are identified by hash functions. So, for example, if two files:

    file1: h1,h2,h3,h4

and

    file2: h5,h6,h1,h8

are to be transferred, only 7 blocks should be moved, not 8. For downloading, you only need to look at other files in the download directory for common hashes (to optimize the download).

### TritonTransfer upload protocol

To upload a file _f_, the client invokes an **uploadFile** RPC call, which takes two arguments:

1. **filename**: the name of the file being uploaded
2. **hashlist**: a list of block hashes making up the file

In the above example, the client would invoke the following RPC call on the server:

```
ret = uploadFile("catpicture.jpg", [h1,h2,h3,h4]);
```

**Return value**: uploadFile returns a list containing the hashes of blocks it still needs, if any. If the returned list is empty, that means that the file has been successfully uploaded. If the list is not empty, then additional blocks must be uploaded, using the next RPC call

To upload a block _b_, the client invokes an **uploadBlock** RPC call, which takes two arguments:

1. **hash**: the hash value identifying the block
2. **block**: the byte array making up the block

**Return value**: uploadBlock returns 'OK' if the block was stored successfully, or 'ERROR' if there was an error. Errors could occur if the block is longer than 16KB, or if the hash value doesn't match the hash of the actual block itself.

#### Upload protocol example

#### TritonTransfer download protocol

To download a file _f_, the client invokes a **downloadFile** RPC call, which takes ones argument:

1. **filename**: the name of the file being downloaded

In the above example, the client would invoke the following RPC call on the server:

```
ret = downloadFile("catpicture.jpg");
```

**Return value**: downloadFile returns a list containing the hashes of blocks making up the file.

To download a block _b_, the client invokes a **downloadBlock** RPC call, which takes one argument:

1. **hash**: the hash value identifying the block

**Return value**: downloadBlock returns the contents of the block if it stored on the server, or 'ERROR' if the block does not exist on the server.

#### Download protocol example

### Why these hashes?

You may be asking why we associate hashes with blocks. The reason is so that if the server already has a popular file, the client need not transfer any data. If many users upload the same large file, for example a popular movie, then only the first user needs to transfer the blocks. For example:

### Grading

In this project, you will define your RPC service, your RPC APIs, and any RPC data types inside of a Thrift IDL file (called TritonTransfer.thrift). You can use any language you want for your client and server, assuming that Thrift supports the language.

* Uploading files/blocks (4 points)
* Downloading files/blocks (4 points)
* Optimizing uploads/downloads so that duplicate blocks are not transferred (2 points)

### Project 5 Submission guidelines

You will submit this project to your CSE 124 GitHub account, including your client code, your server code, and your thrift IDL. You should run thrift on your IDL to generate the stubs for your language, and **commit those stubs to your repository**.

```
_cse124/
|-- project
    |-- proj5
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

### Revisions

1. Feb 29: When looking for common hashes to download, you only need to look in the download directory
2. Feb 28: Clarified that blocks are only to be transferred if they aren't already at the receiver.
3. Feb 24: Clarified point about subdirectories
4. Feb 24: Initial version