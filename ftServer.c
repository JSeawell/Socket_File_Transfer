/*********************************************

Name: Jake Seawell
Date: 11/15/19
Assignment: Project2 - Socket-FileTransfer
Description: This program, called ftserver,
interacts with ftclient on a certain port,
and creates a file transfer system over sockets.
See README.txt for compile/run directions.

Resources:
https://beej.us/guide/bgnet/html/
https://realpython.com/python-sockets/


*********************************************/

// LIBRARIES //
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/wait.h>

#define BACKLOG 10
#define MAXDATASIZE 1050000

using std::cin;
using std::cout;
using std::endl;


//Function: fileToBuffer
//This function takes a filename and buffer, 
//reads the file contents, and stores them 
//in the buffer to send over a socket. This 
//function returns the number of chars
//in the file.
 
int fileToBuffer(char* filename, char* buffer){
	int count = 0;
	char c2Str[2];
	char ch;
	c2Str[1] = '\0';
	FILE* fptr;
	fptr = fopen(filename, "r");
	if (fptr == NULL){
		cout << "FILE NOT FOUND" << endl;
		return -1;
	}
	else{
		ch = fgetc(fptr);
		c2Str[0] = ch;
		while (ch != EOF) {
			strcat(buffer, c2Str);
			count ++;
			ch = fgetc(fptr);
			c2Str[0] = ch;
		}
		fclose(fptr);
	
		return count;
	}
}



// get sockaddr
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}


//Main function:

int main(int argc, char* argv[]){
	//If incorrect # of arguments
	if (argc != 2){
		cout << "ftServer requires one argument (PORTNUM)." << endl;
	}
	//If correct # of arguments
	else{
		cout << "Server open on " << argv[1] << endl;

		int sockfd, new_fd;  // listen on sock_fd, new connection on new_fd
    		
		//For initial connection
		int rv;
		struct addrinfo hints, *servinfo, *p;
		//For data connection
		int new_fd_data;
		struct addrinfo hints_data, *servinfo_data, *p_data;
		int rv_data;

		struct sockaddr_storage their_addr; // connector's address information
		socklen_t sin_size;
		struct sigaction sa;
		int yes=1;
		char s[INET6_ADDRSTRLEN];

		//Message buffer and bytes counter
		char* messageReceive;
		int numbytes = 0;

		//clear hints
		memset(&hints, 0, sizeof hints);
		hints.ai_family = AF_UNSPEC;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_flags = AI_PASSIVE;	

		memset(&hints_data, 0, sizeof hints_data);
		hints_data.ai_family = AF_UNSPEC;
		hints_data.ai_socktype = SOCK_STREAM;
		hints_data.ai_flags = AI_PASSIVE;
	
		if ((rv = getaddrinfo(NULL, argv[1], &hints, &servinfo)) != 0) {
        		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        		return 1;
    		}

		// loop through all the results and bind to the first we can
		for(p = servinfo; p != NULL; p = p->ai_next) {
			if ((sockfd = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1) {
			    perror("server: socket");
			    continue;
			}

			if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
				sizeof(int)) == -1) {
			    perror("setsockopt");
			    exit(1);
			}

			if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			    close(sockfd);
			    perror("server: bind");
			    continue;
			}

			break;
		}
		
		freeaddrinfo(servinfo); // all done with this structure

		if (p == NULL)  {
        		fprintf(stderr, "server: failed to bind\n");
        		exit(1);
    		}

    		if (listen(sockfd, BACKLOG) == -1) {
        		perror("listen");
        		exit(1);
    		}	
		//printf("server: waiting for connections...\n");

		while(1) {  // main accept() loop
			sin_size = sizeof their_addr;
			//Accept client and make new file descriptor: new_fd
			new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
			if (new_fd == -1) {
			    perror("accept");
			    continue;
			}

			inet_ntop(their_addr.ss_family,
			get_in_addr((struct sockaddr *)&their_addr), s, sizeof s);
			
			char host[1024];
			char service[20];
			//Get host name and service of connected client
			getnameinfo((struct sockaddr *)&their_addr, sizeof their_addr, host, sizeof host, service, sizeof service, 0);
			//printf("Host: %s\n", host);    // e.g. "flip1.engr.oregnstate.edu"
			printf("Connection from %s.\n", host);


			//allocate and clear receive buffer
			messageReceive = (char *)malloc(100 * sizeof(char));
			memset(messageReceive, '\0', sizeof(messageReceive));
		
			//receive dataPort from client
			char dataPort[5];
			numbytes = 0;
			numbytes = recv(new_fd, messageReceive, 5, 0);
			if (numbytes < 0){
				perror("receive failure");
				exit(1);
			}
			strcpy(dataPort, messageReceive);
			//cout << "Data port requested: " << dataPort << endl;


			//clear receive buffer
			memset(messageReceive, '\0', sizeof(messageReceive));
		
			//receive command from client: -l OR -g
			numbytes = 0;
			numbytes = recv(new_fd, messageReceive, 2, 0);
			if (numbytes < 0){
				perror("receive failure");
				exit(1);
			}
			//cout << "Command: " << messageReceive << endl; 

			int howManyBytes = 0;
			char buffer[MAXDATASIZE] = "";
			char error[2] = "";
			char filename[20] = "";
	
			//If client requests list: -l
			if (strcmp(messageReceive, "-l") == 0){
				cout << "List directory requested on port " << dataPort << "." << endl;
				//Store directory info in temp file
				system("ls * > newFile.txt");
				memset(filename, '\0', sizeof(filename));
				strcat(filename, "newFile.txt");
				//Store directory info from temp file in buffer
				howManyBytes = fileToBuffer(filename, buffer);
				system("rm newFile.txt");
				memset(error, '\0', sizeof(error));
				strcat(error, "N");
				cout << "Sending directory contents to " << host << ":" << dataPort << endl;
			}
			//If client requests file: -g
			else if (strcmp(messageReceive, "-g") == 0){	

				//clear receive buffer
				memset(messageReceive, '\0', sizeof(messageReceive));
		
				//receive filename
				numbytes = 0;
				//Receives 11 bytes right now
				numbytes = recv(new_fd, messageReceive, 11, 0);
				if (numbytes < 0){
					perror("receive failure");
					exit(1);
				}
				//store filename
				memset(filename, '\0', sizeof(filename));
				strcat(filename, messageReceive);
				
				cout << "File '" << filename << "' requested on port " << dataPort << "." << endl;

				//Store file contents in buffer to send
				howManyBytes = fileToBuffer(filename, buffer);
				memset(error, '\0', sizeof(error));
				strcat(error, "N");

				//If error: file not found
				if (howManyBytes == -1){
					cout << "Sending error message to " << host << ":" << dataPort << endl;
					memset(error, '\0', sizeof(error));
					strcat(error, "Y");
					memset(buffer, '\0', sizeof(buffer));
					strcat(buffer, "Uh. Oh, file not found");
				}
				//No error found
				else
					cout << "Sending '" << filename << "' to " << host << ":" << dataPort << endl;					
			}


			//Setup data connection/socket
			if ((rv_data = getaddrinfo(host, dataPort, &hints_data, &servinfo_data)) != 0) {
        			fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv_data));
        			return 1;
    			}

			// loop through all the results and connect to the first we can
			for(p_data = servinfo_data; p_data != NULL; p_data = p_data->ai_next) {
				//Create new data connection/socket: new_fd_data
				if ((new_fd_data = socket(p_data->ai_family, p_data->ai_socktype, p_data->ai_protocol)) == -1) {
					perror("client: socket error");
					continue;
				}

				//Attempt to connect to client
				if (connect(new_fd_data, p_data->ai_addr, p_data->ai_addrlen) == -1) {
					close(new_fd_data);
					//perror("client: connect error");
					continue;
				}

				break;
			}

			//Connection error
			if (p_data == NULL) {
				fprintf(stderr, "Server: failed to connect\n");
				exit(2);
			}	

			
			//cout << "File-Transfer connection created on " << dataPort << "." << endl;

			//Send error (on non-error) to client
			send(new_fd_data, error, 1, 0);
			//Send message to client
			send(new_fd_data, buffer, howManyBytes, 0);
			//Close data connection
			close(new_fd_data);
		}
	}
	return 0;
}

