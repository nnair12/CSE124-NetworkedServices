import sys
import time
import threading
import os
import hashlib

sys.path.append('gen-py')

from tritonTransfer import transfer
from tritonTransfer.ttypes import *

from thrift import Thrift
from thrift.transport import TSocket
from thrift.transport import TTransport
from thrift.protocol import TBinaryProtocol

BLOCK_SIZE = 16000


# Ensure that the right number of command line parameters were given
if len(sys.argv) != 5 and len(sys.argv) != 6:
    print('Usage: tt-client <server_name> <server_port> upload|download <filename> (<download_dir>)')
    sys.exit(0)

# Set the port number
portNum = int(sys.argv[2])
server = sys.argv[1]

try:

    transport = TSocket.TSocket(server, portNum)

    # Buffering is critical. Raw sockets are very slow
    transport = TTransport.TBufferedTransport(transport)

    # Wrap in a protocol
    protocol = TBinaryProtocol.TBinaryProtocol(transport)

    # Create a client to use the protocol encoder
    client = transfer.Client(protocol)

    # Connect!
    transport.open()

    # Download file
    if sys.argv[3] == 'download':

        localFolder = sys.argv[5]
        localBlocks = {}
        fileBlocks = []

        # Generate a dict of all existing blocks inside the download folder
        if not os.path.isdir(localFolder) or not os.access(localFolder, os.W_OK):
            print 'ERROR'
            sys.exit(0)

        # Get array of local files
        localFiles = [f for f in os.listdir(localFolder) if os.path.isfile(os.path.join(localFolder, f))]

        # Get all hashes
        for file in localFiles:

            # get the path
            filepath = os.path.join(localFolder, file)

            # open the file for reading
            f = open(filepath, 'rb')

            # count the number of blocks
            blockCount = os.path.getsize(filepath) / BLOCK_SIZE
            if os.path.getsize(filepath) % BLOCK_SIZE > 0:
                blockCount += 1

            # Generate the hashes and the blocks
            hashes = []
            blocks = {}
            for i in range(blockCount):
                block = f.read(BLOCK_SIZE)
                hash = hashlib.sha256(block).hexdigest()
                localBlocks[hash] = block

        # Get the blocks associated with this file
        filename = sys.argv[4]
        hashes = client.downloadFile(filename)

        # If server hashes is empty
        if len(hashes) == 0:
            print 'ERROR'
            sys.exit(0)

        # Get all the blocks
        for i in hashes:

            # if block exists locally
            if localBlocks.has_key(i):
                fileBlocks.append(localBlocks[i])
                continue

            # Query for the block on the server
            block = client.downloadBlock(i)
            if block == '':
                print 'ERROR'
                sys.exit(0)

            # add the block into the array
            fileBlocks.append(block)
            localBlocks[i] = block

        # Generate and save file
        fileOutData = ''.join(fileBlocks)
        fileOut = open(os.path.join(localFolder, filename), 'w')
        fileOut.write(fileOutData)
        fileOut.close()


    # Upload file
    if sys.argv[3] == 'upload':

        # Generate hashes
        hashes = []

        # Check if the file exists
        filename = sys.argv[4]
        if not os.path.exists(filename):
            print filename + ' does not exist'
            sys.exit(0)

        # Open the file
        file = open(filename, 'rb')
        blockCount = os.path.getsize(filename) / BLOCK_SIZE
        if os.path.getsize(filename) % BLOCK_SIZE > 0:
            blockCount += 1

        # Generate the hashes and the blocks
        hashes = []
        blocks = {}
        for i in range(blockCount):
            block = file.read(BLOCK_SIZE)
            hash = hashlib.sha256(block).hexdigest()
            hashes.append(hash)
            blocks[hash] = block

        # attempt to upload to the server
        missingBlocks = client.uploadFile(os.path.basename(filename), hashes)

        # upload any missing blocks
        for i in missingBlocks:
            client.uploadBlock(i, blocks[i])

    print 'OK'

except Thrift.TException, tx:
    print '%s' % (tx.message)