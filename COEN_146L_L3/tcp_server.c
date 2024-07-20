//COEN 146L, Lab3, Step 1
/* 
* Name: Carlo Bilbao 
* Date: 10/14/22
* Title: Lab3 - TCP/IP Socket Programming 
* Description: This program is for a TCP server that accepts a client
* connection for file transfer
*/
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>	// Required, for socket creation
#include <netinet/in.h>	// Required, gets struct sockaddr_in
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>	// For extracting strlen

#define PORT 5000
#define SA struct sockaddr

//Declare the socket file descriptor
 int sockfd;
 //Declare the connection file descriptor
 int connfd; // Will be used when we accept the connection with the client, separate from sockfd (our server's)

//Declare the components of the socket 
int domain = AF_INET;	// Declare the communication domain (AF_INET, IPv4)
//type = SOCK_STREAM;	// Declare type to be TCP
int protocol = 0;	// Declare the protocol, 0 is IP (Internet Protocol)


//Declare buffers of size 10 bytes
 char sendbuf[10];	// Buffer for sending
 char recbuf[10];	// Buffer for receiving

//Declare server address to which to bind for receiving messages and client address to fill in sending address
struct sockaddr_in server_addr, client_addr;	// server_addr is used for bind/listen; client_addr used when we accept

//Connection handler for servicing client request for file transfer????
void* connectionHandler(void* sock){	// The void pointer refers to a generic pointer not associated with any data types
   //declate buffer holding the name of the file from client
   char filenamebuf[10];

   //get the connection descriptor
   int sock_desc = *(int*)sock;	// Type cast the argument passed in into an integer, to accommodate a socket
   //----int read_size;
   
   //Connection established, server begins to read and write to the connecting client
   //printf("Connection Established with client IP: %s and Port: %d\n", inet_ntoa(clienAddr.sin_addr), ntohs(clienAddr.sin_port));
   printf("Connection Established with client IP: %s and Port: %d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
   
	//receive name of the file from the client
	
   //open file and send to client
   //FILE *fp;
   FILE *src;	// Source stream, for the source file (read)
   FILE *dst;	// Destination stream, for the destination file (write)
   src = fopen("src.dat","r");	// Hard-coded source file
   dst = fopen("dest.dat","w");	// Hard-coded destination file
   
   //read file and send to connection descriptor
   //Read segment
   //bzero(buffer, 256);
   bzero(filenamebuf, 256);
   //n = read(newsockfd, buffer, 255);
   int n = read(sock_desc, filenamebuf, 255);
   //if (n < 0) error("ERROR reading from socket");
   if (n < 0) perror("ERROR reading from socket");
   //printf("Here is the message: %s\n", buffer);
   printf("Here is the message: %s\n", filenamebuf);
   
   printf("File transfer complete\n");
   
   //close file
   //fclose(fp);
   fclose(dst);
   fclose(src);
    
   //Close connection descriptor
   close (sock_desc);
   
   return 0;
}

/*
 *	Socket
 *  Bind
 * 	Listen
 * 	Accept
 * 	Read / Write
 * 	Close
 */
int main(int argc, char *argv[]){
 //Get from the command line, server IP, src and dst files.
 if (argc != 2){
	printf ("Usage: %s <port #> \n",argv[0]);
	exit(0);
 } 
 
 //Create the TCP socket for the server
 //socket(domain, type, protocol)
 //If the value returned is negative, then it failed
 if ((sockfd = socket(domain, SOCK_STREAM, protocol)) < 0)
 {
	perror("cannot create socket"); 
    return 0;
 }
 
 bzero(&server_addr, sizeof(server_addr));
 bzero(&client_addr, sizeof(client_addr));
 
 //Open a TCP socket, if successful, returns a descriptor????
 /*int listen(int sockfd, int backlog);*/

 //Setup the server address to bind using socket addressing structure
 server_addr.sin_family = AF_INET;
 //serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
 server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
 //--my_sockaddress.sin_addr.s_addr = inet_addr("127.0.0.1");	// Hard-coded version
 server_addr.sin_port = htonl( PORT );
 //my_sockaddress.sin_addr.s_addr = htonl(INADDR_LOOPBACK);	// Used for binding the socket to the localhost
 //my_sockaddress.sin_addr.s_addr = htonl(INADDR_LOOPBACK);	// Used for binding the socket to the localhost
 //--int inet_pton(int af, const char *src, void *dst); //To convert an address in its standard text format into its numeric binary form use the inet_pton() function. The argument af specifies the family of the address. 
 
 //Declare components of bind
 //----const struct sockaddr *addr;	// Contains address to be bound to socket
 //----socklen_t addrlen = sizeof(client_addr);	// Length of the sockaddr structure via addr
 
 //--client.sin_family = AF_INET;       /* Internet domain */
 //--loopback.sin_addr.s_addr=htonl(INADDR_LOOPBACK);
 //--loopback.sin_port=htons(10000);

 //Bind the IP address and port for this server endpoint socket 
 //int bind (int sockfd, const struct sockaddr *addr, socklen_t addrlen);
 if (bind(sockfd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) perror("Server bind() failed");
 else printf("Socket successfully binded.\n");

 //Declare components of listen
 int backlog = 5;	// Maximum length to which the queue of pending connections for sockfd may grow
 
 // Call listen to wait for client connection, and can queue 5 client requests
 //int listen(int sockfd, int backlog);
 //If failed, listen() returns -1
 if (listen(sockfd, backlog) != 0) perror("Server listen() failed");
 printf("Server listening/waiting for client at port 5000\n");
 
 //Accept the connection, creating new socket with new file descriptor
 //int new_socket = accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
 //int *new_socket;
 //*new_socket = accept(sockfd, (struct sockaddr *) &client_addr, (socklen_t *) &addrlen);
 int len = sizeof(client_addr);	// Length of client address
 //Set connection file descriptor, we will be accepting the client address here, since we are the server
 connfd = accept(sockfd, (struct sockaddr *) &client_addr, (socklen_t *) &len);

 //Server accepts the connection and call the connection handler
 char *message /*----, client_message[2000]*/;
 message = "Connection handler called.\n";
 //write(sock , message, strlen(message));
 //write(*new_socket, message, strlen(message));	// Write over the newly established socket
 write(connfd, message, strlen(message));	// Write over the newly established socket
 //connectionHandler(new_socket);	// You can pass in any data type to this function, but it must be a pointer
 connectionHandler((void*) &connfd);	// You can pass in any data type to this function, but it must be a pointer
 
 
 //close socket descriptor
 //close (fd);
 close (sockfd);	// Close the original descriptor for the server socket

 return 0;
}
