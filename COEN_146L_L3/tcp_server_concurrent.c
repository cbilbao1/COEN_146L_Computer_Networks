//COEN 146L : Lab3, step 3: concurrent TCP server that accepts and responds to multiple client connection requests, each requesting a file transfer
/* 
* Name: Carlo Bilbao 
* Date: 1/28/22
* Title: Lab3 - Part 3 
* Description: This program is for a TCP client that connects to
* a server
*/
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

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

//Define the number of clients/threads that can be served
#define N 100
int threadCount = 0;
pthread_t clients[N]; //declaring N threads

// data structure to hold copy
struct copy_struct {
	int thread_id; 
	char* src_filename;
	char* dst_filename; 
};

 //Declare the socket file descriptor
 //Declare the connection file descriptor
 //Will be used when we accept the connection with the client, separate from sockfd (our server's)
 //Declare socket and connection file descriptors.
 int sockfd, connfd, len;

 //Declare receiving and sending buffers of size 10 bytes
 char sendbuf[10];
 char recbuf[10];

 //Declare server address to which to bind for receiving messages and client address to fill in sending address
 struct sockaddr_in servaddr, cli;	// server_addr is used for bind/listen; client_addr used when we accept

//Connection handler (thread function) for servicing client requests for file transfer
void* connectionHandler(void* sock){
	struct copy_struct params = *(struct copy_struct*)sock;  // cast/dereference void* to copy_struct; get the socket information from the function argument
	//func_copy(params.src_filename, params.dst_filename);
	
	char* src_filename2 = params.src_filename;
	char* dst_filename2 = params.dst_filename;
   //declare buffer holding the name of the file from client
   //char filenamebuf[10];
   
   //Connection established, server begins to read and write to the connecting client
   printf("Connection Established with client IP: %s and Port: %d\n", inet_ntoa(cli.sin_addr), ntohs(cli.sin_port));
   
	//receive name of the file from the client
	
   //open file and send to client
   FILE *src_fp;
   FILE *dst_fp;
   src_fp = fopen(src_filename2,"r");
   dst_fp = fopen(dst_filename2,"w");
   
   int newsockfd = *(int*)sock;	// Type cast the argument passed in into an integer, to accommodate a socket; take information from socket, put to new
   char* buffer = malloc((size_t)BUF_SIZE);
     
   //read file and send to connection descriptor
   //Read segment
   bzero(buffer, 256);
   int n = read(newsockfd, buffer, 255);
   if (n < 0) perror("ERROR reading from socket");
   printf("Here is the message: %s\n", buffer);
   
   printf("File transfer complete\n");
   
   //close file
   fclose(src_fp);
   fclose(dst_fp);
    
   //Close connection descriptor
   close (*(int*)sock);

   pthread_exit(0);
   pthread_exit(NULL);
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
	if (argc != 4){
		printf ("Usage: %s <src_filename> <dst_filename> <num_threads> \n",argv[0]);
		exit(0);
	}
	
	//char* src_filename = argv[1];	// First argument is src
	//char* dst_filename = argv[2];	// Second argument is dst
	int num_threads = atoi(argv[3]);	// Third argument is number of threads
   
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
	
	//Threads
	pthread_t clients[num_threads]; //initialize threads
	struct copy_struct thread_params[num_threads]; // structure for each thread
	
	for (int i = 0; i < num_threads; ++i) {
		clients[i] = i;	// Each client thread corresponds to a number
		thread_params[i].src_filename = argv[1];
		thread_params[i].dst_filename = argv[2];
	}
 
 //ThreadCount starts at 0 here, will be incremented in the while loop, acting as the index number for the clients[] thread
 
 while (1){
   //Server accepts the connection and call the connection handler
   char *message /*, client_message[2000]*/;
   message = "Connection handler called.\n";
   write(sockfd, message, strlen(message));
   
   if(pthread_create(&clients[threadCount], NULL, connectionHandler, (void*) &connfd) < 0){
      perror("Unable to create a thread");
      exit(0);
   }
   else 
      printf("Thread %d has been created to service client request\n",++threadCount);
 }
 for(int i = 0; i < threadCount; i++){
      pthread_join(clients[i], NULL);
 }
 return 0;
}
