# Socket_FT  
> CS-372 (Intro to Networks) Program  
  
A server/client file transfer service over sockets  

## Description:  
> This project functions using two  
> programs: ftClient.py (written in python), and  
> ftServer.c (written in C). The process is shown  
> below:  
> 1. ftserver starts and listens on [PORTNUM] socket  
> 2. ftclient initiates a connection on [PORTNUM] socket  
> 3. The connection is made  
> 4. ftserver initiates a data connection on [DATAPORTNUM] socket  
> 5. ftclient requests either a directory listing or a file on the [DATAPORT] socket  
> 6. ftserver sends the listing, file, or appropriate error to ftclient  
> 7. ftserver closes the data connection on [DATAPORTNUM]  
> 8. ftclint closes the original connection on [PORTNUM]  
> 9. ftserver repeats step 1 until terminated by user  
------------------------------------------------------


## Files Needed:  
> ftServer.c  
> ftclient.py  

## How to compile ftServer.c into executable:  
> g++ ftServer.c -o ftserver  

## How to Run Program:  
> 1. Start server by typing: 'ftserver [PORTNUM]'  
> 2a. Request directory from server by typing: 'python3 ftclient.py flip[x] [PORTNUM] -l [DATAPORTNUM]'  
> OR  
> 2b. Request file from server by typing: 'python3 ftclient.py flip[x] [PORTNUM] -g [FILENAME] [DATAPORTNUM]'  

> **In both cases (2a & 2b), flip[x] should be whichever flip server that the server is run from.  
> For example: if the flip server is started on flip3, then the client should give flip3 as the 3rd command**  

   
## Tech and/or concepts learned/used:
> - C
> - Python
> - Socket Programming
> - Network Programming
> - Client/Server Communication
