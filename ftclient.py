#!/usr/bin/env python3

# Jake Seawell
# 12/01/19
# This program, called ftclient,
# establishes a file tranfer with
# ftServer.c. See README for 
# compile/run instructions.

# Resources:
# https://beej.us/guide/bgnet/html/
# https://realpython.com/python-sockets/


import socket
import sys
import os.path

HOST = sys.argv[1]  # The server's hostname or IP address
PORT = int(sys.argv[2])        # The port used by the server

#Argument error handling
if len(sys.argv) < 5:
    print('Error: Not enough arguments.')
    exit(1)
elif len(sys.argv) > 6:
    print('Error: Too many arguments.')
    exit(1)
elif sys.argv[3] == '-g' and len(sys.argv) != 6:
    print('Error: "-g" requires exactly 5 arguments, 4 provided.')
    exit(1)
elif sys.argv[3] == '-l' and len(sys.argv) != 5:
    print('Error: "-l" requires exactly 4 arguments, 5 provided.')
    exit(1)
elif len(sys.argv) == 6 and len(sys.argv[4]) > 20:
    print('Error: Filename argument cannot exceed 20 chars')
    exit(1)
else:
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s1:
        #Attempt to connect to server
        s1.connect((HOST, PORT))
        #If user entered 6 arguments
        if len(sys.argv) == 6:
            #Send DATA PORT (argv[5])
            s1.send(sys.argv[5].encode())
        else:
            #Send DATA PORT (argv[4])
            s1.send(sys.argv[4].encode())
        
        #send argument ('-l' or '-g'
        s1.send(sys.argv[3].encode())
       
        #if -g, send filename
        if sys.argv[3] == '-g':
            s1.send(sys.argv[4].encode())

        #Create new DATA socket on DATA PORT
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s2:
            #If user entered 6 arguments
            if len(sys.argv) == 6:
                #Bind to argv[5]
                s2.bind(('', int(sys.argv[5]))) #bind socket to port number
            else:
                #Bind to argv[4]
                s2.bind(('', int(sys.argv[4])))
            
            #Listen for connection(s) from server
            s2.listen()
            #print('Client: Waiting for server connection...')
           
            #Accept connection from server, create new DATA CONNECTION 
            conn, addr = s2.accept()
            #On DATA connection socket
            with conn:
                #print('Server connected')
                #receive error bool
                error = conn.recv(1).decode()
                #Receive message from client
                
                #If receiving directory, and no errors
                if sys.argv[3] == '-l' and error == 'N':
                    print('Receiving directory structure from', sys.argv[1], ':', sys.argv[4])
                    message = conn.recv(1024).decode()
                    print(message)
                
                #If receiving file, and no errors
                elif sys.argv[3] == '-g' and error == 'N':
                    print('Receiving', sys.argv[4], 'from', sys.argv[1], ':', sys.argv[5])
                    message = conn.recv(1050000).decode()
                    #File path is same as argv[4]
                    path = '%s' % (sys.argv[4])
                    uniquePre = 1
                    #If that file already exists in current dir
                    while os.path.exists(path):
                        #Add #_ prefix to filename until filnamee is unique
                        path = '%d_%s' % (uniquePre, sys.argv[4])
                        uniquePre += 1
                    #Open file for writing
                    f = open(path, "w")
                    #Write message to file
                    f.write(message)
                    #Close file
                    f.close()

                    print('File transfer complete.')
                
                #Server sent error
                elif error == 'Y':
                    message = 'FILE NOT FOUND'
                    print(sys.argv[1], ':', sys.argv[5], "says", message)
    s1.close()
# vim: tabstop=8 expandtab shiftwidth=4 softtabstop=4
