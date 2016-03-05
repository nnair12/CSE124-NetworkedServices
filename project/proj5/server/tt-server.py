import sys
import time
import threading
import os
import hashlib

from thrift import Thrift
from thrift.transport import TSocket
from thrift.transport import TTransport
from thrift.protocol import TBinaryProtocol
from thrift.server import TServer

sys.path.append('gen-py')

from tritonTransfer import transfer
from tritonTransfer.ttypes import *

BLOCK_SIZE = 16000

# Metadata store
metadata = {}

# Block store
blocks = {}

# the handler that contains the methods specified in the thrift file
class transferHandler:


    def __init__(self):
        pass


    def uploadFile(self, fileName, hashlist):

        # Get only the filename and not the path
        fileName = os.path.basename(fileName)

        # Add the file metadata into the metadata store
        metadata[fileName] = hashlist

        # Keep track of the blocks that have not yet been updated
        missingBlocks = []

        # Loop through all things in the hashlist to determine what exists
        for i in hashlist:
            if not blocks.has_key(i) and i not in missingBlocks:
                missingBlocks.append(i)

        # Sends the missing blocks back to the client
        return missingBlocks


    def uploadBlock(self, hash, block):

        # check length and valid hash
        if len(block) > BLOCK_SIZE or hashlib.sha256(block).hexdigest() != hash:
            return 'ERROR'

        # save block into the blocks dict
        blocks[hash] = block
        return 'OK'


    def downloadFile(self, fileName):

        # check the metadata store for the blocks
        if metadata.has_key(fileName):
            return metadata[fileName]
        else:
            return []


    def downloadBlock(self, hash):

        # check if the hash exists in the block store
        if blocks.has_key(hash):
            return blocks[hash]
        else:
            return ''


###############################################################################
# MAIN
###############################################################################

# Ensure that the right number of command line parameters were given
if len(sys.argv) != 3 and len(sys.argv) != 2:
    print('Usage: tt-server <server_port>')
    sys.exit(0)

# Set the port number
portNum = int(sys.argv[1])

handler = transferHandler()
processor = transfer.Processor(handler)
transport = TSocket.TServerSocket(port=portNum)
tfactory = TTransport.TBufferedTransportFactory()
pfactory = TBinaryProtocol.TBinaryProtocolFactory()

server = TServer.TThreadedServer(processor, transport, tfactory, pfactory)

server.serve()