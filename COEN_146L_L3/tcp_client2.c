#include <arpa/inet.h> // inet_addr()
#include <netdb.h>	// Required, gets struct sockaddr_in
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h> // bzero()
#include <sys/socket.h>	// Required, for socket creation
#include <unistd.h> // read(), write(), close()

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

#include <sys/stat.h>	// Required for open
#include <fcntl.h>

#define BUF_SIZE 2048
#define PORT 5000
#define SA struct sockaddr
void connectionHandler(void* sockfd2, char* src_filename2)
{
	printf("BEFORE for : ");
    char buff[BUF_SIZE];	// This is the buffer used for exchanging information
    int n;	// The current item in the buffer
    printf("BEFORE for : ");
    for (;;) {
        bzero(buff, sizeof(buff));	// Create the buffer
        printf("Enter the string : ");
        n = 0;
        while ((buff[n++] = src_filename2[n]) != '\n')
        {
            
		}
        write(*(int*)sockfd2, buff, sizeof(buff));
        bzero(buff, sizeof(buff));
        read(*(int*)sockfd2, buff, sizeof(buff));
        printf("From Server : %s", buff);
        if ((strncmp(buff, "exit", 4)) == 0) {	// A way to get out, just type exit
            printf("Client Exit...\n");
            break;
        }
    }
    
    //--NEW CODE
	//SETUP
	printf("src_ %s\n", src_filename2);
	/*printf("dst_ %s\n", dst_filename);
	int src_fd;
	//int dst_fd;
	
	src_fd = open(src_filename, O_RDONLY);	// opens a file for reading
	if (src_fd < 0) { // open() error checking
		fprintf(stderr, "unable to open %s for reading: %i\n", src_filename, errno);
		close(src_fd);
		exit(0);
	}
	// O_WRONLY - write only
	// O_CREAT - create file if it doesn't exist
	// O_TRUNC - truncates file to length 0 if it already exists (deletes existing file)
	// opens a file for writing; erases old file/creates a new file
	dst_fd = open(dst_filename, O_WRONLY | O_CREAT | O_TRUNC);	
	if (dst_fd < 0) { // open() error checking
		fprintf(stderr, "unable to open/create %s for writing: %i\n", dst_filename, errno);
		close(dst_fd);
		exit(0);
	}


	// read/ write loop
	char* buf = malloc((size_t)BUF_SIZE);   // allocate a buffer to store read data
	int bytes_read; // Variable for number of bytes read for each call
	while ((bytes_read = read(src_fd, buf, BUF_SIZE)) > 0) {	// reads up to BUF_SIZE bytes from src_filename
	// writes bytes_read to dst_filename
		if (write(dst_fd, buf, bytes_read) < 0) // Writes bytes_read into dst_filename
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
	free(buf);*/

}
 
 /*
 *	Socket
 *  
 * 	
 * 	Connect	- Analogous to Server's Accept
 * 	Read / Write
 * 	Close
 */
int main(int argc, char *argv[])
{
	//Get from the command line, server IP, src and dst files.
	if (argc != 2){
		printf ("Usage: %s <src_filename> \n",argv[0]);
		exit(0);
	}
	
	char* src_filename = argv[1];	// First argument is src
	//char* dst_filename = argv[2];	// Second argument is dst
	
	//Declare socket components
	//The socket file descriptor for client
    // The socket file descriptor when connecting to server
    int sockfd;
    struct sockaddr_in servaddr;	//Declare server address to accept
 
    //Create the TCP socket for the client
	//If the value returned is negative, then it failed
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        printf("Client socket failed...\n");
        exit(0);
    }
    else printf("Socket successfully created..\n");
    
    bzero(&servaddr, sizeof(servaddr));
 
    //Declare host for the client
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    servaddr.sin_port = htons(PORT);	// Swap bytes from local/host byte order to network byte order
 
    // connect the client socket to server socket
    if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) {
        printf("Connection with the server failed!\n");
        exit(0);
    }
    else printf("Connected to the server..\n");
	printf("BEFORE connectionHandler");
    // Passes off a communication message to the server
    connectionHandler((int*)&sockfd, src_filename);	// We will only have control of requesting a file
 
    //Close socket descriptor
    close(sockfd);
}
