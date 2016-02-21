import socket
import sys
import numpy
import time
import threading

# Ensure that the right number of command line parameters were given
if len(sys.argv) != 2:
    print('Usage: DelayMe <port>')
    sys.exit(0)

portNum = int(sys.argv[1])

# Create a TCP/IP socket
sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)



def session(connection, client_address):

    try:

        # Receive the delay command
        data = connection.recv(16)

        # Process the delay command
        command = data.split(' ')

        # Sigma = command[0]
        # mu = command[1]
        timeout = numpy.random.normal(command[1], command[2], 1)
        time.sleep(timeout[0] / 1000)

        connection.sendall('bye')

    finally:
        # Clean up the connection
        connection.close()



# MAIN

# Bind the socket to the port
server_address = ('localhost', portNum)
print('starting up on %s port %s' % server_address)
sock.bind(server_address)

# Listen for incoming connections
sock.listen(1)

while True:
    # Wait for a connection
    connection, client_address = sock.accept()
    t = threading.Thread(target=session, args=(connection, client_address))
    t.start()
