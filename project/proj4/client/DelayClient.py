import socket
import sys
import time
import threading
import os

# Ensure that the right number of command line parameters were given
if len(sys.argv) < 4:
    print('Usage: DelayClient <port> <mu> <sigma> (<parallel|serial> <requests> <threshold>)')
    sys.exit(0)

# Set the port number
portNum = int(sys.argv[1])

requests = 1
finish = 1
completed = 0

def sendRequest():

    # Create a TCP/IP socket
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    # Connect the socket to the port where the server is listening
    server_address = ('localhost', portNum)
    sock.connect(server_address)

    try:
        # Send data
        message = 'delay '
        message += ' '.join(sys.argv[2:4])

        sock.sendall(message)

        # Look for the response
        sock.recv(16)

        global completed
        global requests
        global progMillis

        completed += 1

        if (float(completed) / requests) >= finish:
            print('Program Took: %dms' % (int(round(time.time() * 1000)) - progMillis))
            os._exit(0)

    finally:
        sock.close()



# MAIN

if len(sys.argv) >= 6 and sys.argv[5]:
    requests = int(sys.argv[5])

if len(sys.argv) >= 7:
    finish = float(sys.argv[6])

progMillis = int(round(time.time() * 1000))

if sys.argv[4] and (sys.argv[4] == 'parallel'):

    threads = []

    for i in range(0, requests):
        t = threading.Thread(target=sendRequest)
        threads.append(t)
        t.start()

    for t in threads:
        t.join()

else:
    for i in range(0, requests):
        sendRequest()


print('Program Took: %dms' % (int(round(time.time() * 1000)) - progMillis))