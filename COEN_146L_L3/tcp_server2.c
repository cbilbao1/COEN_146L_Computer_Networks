//COEN 146L, Lab3, Step 1
/* 
* Name: Carlo Bilbao 
* Date: 10/14/22
* Title: Lab3 - TCP/IP Socket Programming 
* Description: This program is for a TCP server that accepts a client
* connection for file transfer
*/
#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>	// Required, gets struct sockaddr_in
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>	// Required, for socket creation
#include <sys/types.h>

#include <stdio.h>   // fprintf(), fread(), fwrite(), fopen(), fclose()
#include <stdlib.h>  // malloc(), free()
#include <pthread.h> // pthread_create()
#include <unistd.h>  // read(), write()
#include <fcntl.h>   // open(), close()
#include <errno.h>   // errno
#include <time.h>    // clock()
#include <sys/mman.h> // mmap()
#include <sys/types.h>
#include <sys/stat.h>

#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>	// For extracting strlen
#define BUF_SIZE 2048
#define PORT 5000
#define SA struct sockaddr
   
//Connection handler for servicing client request for file transfer
void connectionHandler(void* connfd2, char* src_filename2, char* dst_filename2)
//void connectionHandler(int connfd)
{
	printf("BEFORE for : ");
    char buff[BUF_SIZE];	// This is the buffer used for exchanging information
    int n;	// The current item in the buffer
    // infinite loop for chat
    printf("BEFORE for : ");
    for (;;) {
        bzero(buff, BUF_SIZE);
        // Read the requested filename message from client and copy it in buffer
        read(*(int*)connfd2, buff, sizeof(buff));
        // print buffer which contains the client contents
        printf("From client: %s\t To client : ", buff);
        bzero(buff, BUF_SIZE);
        n = 0;
        // copy Client requested filename message in the buffer
        while ((src_filename2[n++] = buff[n]) != '\n')
        {
            
		}
        // and send that buffer to client
        //--write(*(int*)connfd, buff, sizeof(buff));
   
        if (strncmp("exit", buff, 4) == 0) {	// A way to get out, just type exit
            printf("Server Exit...\n");
            break;
        }
    }
    
    
    //--NEW CODE
	//SETUP
	printf("src_ %s\n", src_filename2);
	printf("dst_ %s\n", dst_filename2);
	int src_fd;
	int dst_fd;
	
	src_fd = open(src_filename2, O_RDONLY);	// opens a file for reading
	if (src_fd < 0) { // open() error checking
		fprintf(stderr, "unable to open %s for reading: %i\n", src_filename2, errno);
		close(src_fd);
		exit(0);
	}
	// O_WRONLY - write only
	// O_CREAT - create file if it doesn't exist
	// O_TRUNC - truncates file to length 0 if it already exists (deletes existing file)
	// opens a file for writing; erases old file/creates a new file
	dst_fd = open(dst_filename2, O_WRONLY | O_CREAT | O_TRUNC);	
	if (dst_fd < 0) { // open() error checking
		fprintf(stderr, "unable to open/create %s for writing: %i\n", dst_filename2, errno);
		close(dst_fd);
		exit(0);
	}


	// read/ write loop
	//char* buf = malloc((size_t)BUF_SIZE);   // allocate a buffer to store read data
	int bytes_read; // Variable for number of bytes read for each call
	while ((bytes_read = read(src_fd, buff, BUF_SIZE)) > 0) {	// reads up to BUF_SIZE bytes from src_filename
	// writes bytes_read to dst_filename
		if (write(dst_fd, buff, bytes_read) < 0) // Writes bytes_read into dst_filename
		{
			fprintf(stderr, "Write error: %i\n", errno);
			exit(0);
		}
	}
	// closes src_fd file descriptor
	close(src_fd);
	// closes dst_fd file descriptor
	close(dst_fd);
	// frees memory used for buf
	//free(buf);

}
   
/*
 *	Socket
 *  Bind
 * 	Listen
 * 	Accept
 * 	Read / Write
 * 	Close
 */
int main(int argc, char *argv[])
{
	//Get from the command line, server IP, src and dst files.
	if (argc != 3){
		printf ("Usage: %s <src_filename> <dst_filename> \n",argv[0]);
		exit(0);
	}
	
	char* src_filename = argv[1];	// First argument is src
	char* dst_filename = argv[2];	// Second argument is dst
 
	//Declare the socket file descriptor
	//Declare the connection file descriptor
	//Will be used when we accept the connection with the client, separate from sockfd (our server's)
    int sockfd, connfd, len;
    struct sockaddr_in servaddr, cli;
   
    //Create the TCP socket for the server
	//socket(domain, type, protocol)
	//If the value returned is negative, then it failed
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        printf("Server socket failed!\n");
        exit(0);
    }
    else printf("Socket successfully created..\n");
    
    bzero(&servaddr, sizeof(servaddr));
   
    //Setup the server address to bind using socket addressing structure
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(PORT);	// We will be listening on port 5000
   
    //Bind the IP address and port for this server endpoint socket
    if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) {
        printf("Server bind() failed!\n");
        exit(0);
    }
    else printf("Socket successfully binded..\n");
   
    // Call listen to wait for client connection, and can queue 5 client requests
	//If failed, listen() returns -1
	//The backlog is set to 5
    if ((listen(sockfd, 5)) != 0) {
        printf("Server listen() failed!\n");
        exit(0);
    }
    else printf("Server listening..\n");
    
    //Accept the connection, creating new socket with new file descriptor
    len = sizeof(cli);	// Length of client address
   
    //Set connection file descriptor, we will be accepting the client address here, since we are the server
    connfd = accept(sockfd, (SA*)&cli, (socklen_t *)&len);
    printf("BEFORE for %i: \n", connfd);
    if (connfd < 0) {
        printf("Server accept() failed!\n");
        exit(0);
    }
    else printf("Server accepted the client...\n");
    
    //Server accepts the connection and call the connection handler
	//--char *message /*----, client_message[2000]*/;
	//--message = "Connection handler called.\n";
	//write(sock , message, strlen(message));
	//write(*new_socket, message, strlen(message));	// Write over the newly established socket
	//--write(connfd, message, strlen(message));	// Write over the newly established socket
	//connectionHandler(new_socket);	// You can pass in any data type to this function, but it must be a pointer
	printf("Right before connectionHandler() for %i: \n", connfd);
	connectionHandler((void*) &connfd, src_filename, dst_filename);	// You can pass in any data type to this function, but it must be a pointer
   printf("After accepted : ");
    //close socket descriptor
	close (sockfd);	// Close the original descriptor for the server socket
}
