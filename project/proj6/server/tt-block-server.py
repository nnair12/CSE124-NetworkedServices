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


    def uploadBlock(self, hash, block):

        # check length and valid hash
        if len(block) > BLOCK_SIZE or hashlib.sha256(block).hexdigest() != hash:
            return 'ERROR'

        # save block into the blocks dict
        blocks[hash] = block
        return 'OK'


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
if len(sys.argv) != 4:
    print('Usage: tt-block-server <server_port> <metadata_server> <metadata_port>')
    sys.exit(0)

# Set the port number
portNum = int(sys.argv[1])

# Set the metadata server information
metadataServer = sys.argv[2]
metadataServerPort = int(sys.argv[3])

# Apache thrift things
handler = transferHandler()
processor = transfer.Processor(handler)
transport = TSocket.TServerSocket(port=portNum)
tfactory = TTransport.TBufferedTransportFactory()
pfactory = TBinaryProtocol.TBinaryProtocolFactory()

server = TServer.TThreadedServer(processor, transport, tfactory, pfactory)

server.serve()

try:

    transport = TSocket.TSocket(metadataServer, portNum)

    # Buffering is critical. Raw sockets are very slow
    transport = TTransport.TBufferedTransport(transport)

    # Wrap in a protocol
    protocol = TBinaryProtocol.TBinaryProtocol(transport)

    # Create a client to use the protocol encoder
    client = transfer.Client(protocol)

    # Connect!
    transport.open()

    print 'OK'

except Thrift.TException, tx:
    print '%s' % (tx.message)