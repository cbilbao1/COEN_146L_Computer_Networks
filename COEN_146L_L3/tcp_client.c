//COEN 146L, Lab3, Step 2
/* 
* Name: Carlo Bilbao 
* Date: 10/14/22
* Title: Lab3 - TCP/IP Socket Programming 
* Description: This program is for a TCP client that connects to
* a server
*/
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>	// Required, for socket creation
#include <netinet/in.h>	// Required, gets struct sockaddr_in
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <arpa/inet.h>

#include <sys/stat.h>	// Required for open
#include <fcntl.h>

#define PORT 5000
#define SA struct sockaddr

/*
 *	Socket
 *  
 * 	
 * 	Connect	- Analogous to Server's Accept
 * 	Read / Write
 * 	Close
 */
int main(int argc, char *argv[]){
    //Get from the command line, server IP, src and dst files.
    if (argc != 5){
		printf ("Usage: %s <ip of server> <port #> <src_file> <dest_file>\n",argv[0]);
		exit(0);
    } 
    
    //Declare socket components
    int sockfd;	//The socket file descriptor for client
    //----int server_fd;	//The socket file descriptor for server
    int connfd;	// The socket file descriptor when connecting to server
    char buf[10];	//Buffer for the client

    //Declare server address to accept
    struct sockaddr_in server_addr/*----, my_sockaddress*/;	// Need the information for server we're connecting to and our client's socket
    //----socklen_t addrlen;	// Length of the sockaddr structure via addr
    //Declare the components of the socket
    //----int domain = AF_INET;	// Declare the communication domain (AF_INET, IPv4)
	//type = SOCK_STREAM;	// Declare type to be TCP
	//----int protocol = 0;	// Declare the protocol, 0 is IP (Internet Protocol)
    
    //Create the TCP socket for the client
	//socket(domain, type, protocol)
	//If the value returned is negative, then it failed
    /*sockfd = socket(AF_INET, SOCK_STREAM, 0); UNUSED*/
    //if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
    //if ((server_fd = socket(domain, SOCK_STREAM, protocol)) < 0) 
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
	{
		perror("cannot create socket"); 
		return 0;
	} else
	{
		printf("Client socket successfully created.\n");
	}
	
	bzero(&server_addr, sizeof(server_addr));

   //Declare host for the client
    server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons( PORT );	// Swap bytes from local/host byte order to network byte order

    //get hostname????
    //int bind (int sockfd, const struct sockaddr *addr, socklen_t addrlen);

    //Open a socket, if successful, returns????
    //int listen(int sockfd, int backlog);

    //Set the server address to send using socket addressing structure
    //serv_addr.sin_addr.s_addr = htonl (INADDR_ANY);
    //--server_addr.sin_addr.s_addr = htonl (INADDR_ANY);
    //----my_sockaddress.sin_addr.s_addr=htonl(INADDR_LOOPBACK);

    //Connect to the server????
    //int new_socket = connect(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
    //----int new_socket = connect(server_fd, (struct sockaddr *) &server_addr, sizeof(server_addr));
    connfd = connect(sockfd, (struct sockaddr *) &server_addr, sizeof(server_addr));	// Will be connecting to the server address because we are the client
    if (connfd != 0) {
		printf("Connection with the server failed! , %d\n", connfd);
		exit(0);
	} else
	printf("Successful connection with the server!\n");
    
    //Client sends the name of the file to retrieve from the server
    int fd = open("src.dat", O_RDWR);	// Give read and write permissions

    //Client begins to write and read from the server
    //Read segment
    //bzero(buffer, 256);
    bzero(buf, 256);	// Create the buffer for doing operations
    //n = read(newsockfd, buffer, 255);
    //----int n = read(new_socket, buf, 255);	// Use the newly created socket, read total number of characters in socket, or 255
    int n = read(connfd, buf, 255);	// Use the newly created socket, read total number of characters in socket, or 255
    //if (n < 0) error("ERROR reading from socket");	// Checks if the number of characters read is < 0
    if (n < 0) perror("ERROR reading from socket");	// Checks if the number of characters read is < 0
    //printf("Here is the message: %s\n", buffer);
    printf("Here is the message: %s\n", buf);
    
    //Write segment
    //n = write(newsockfd, "I got your message", 18);
    //----n = write(new_socket, "I got your message", 18);	// Confirmation message to the server from the client
    n = write(connfd, "I got your message", 18);	// Confirmation message to the server from the client
    //if (n < 0) error("ERROR writing to socket");
    if (n < 0) perror("ERROR writing to socket");	// Checks if the number of characters written is < 0
    
    //Close socket descriptor
    close (fd);
    close (sockfd);
    
    return 0;
}

