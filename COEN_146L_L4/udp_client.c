//COEN 146L, Lab 4, Step 2
/* 
* Name: Carlo Bilbao 
* Date: 10/28/22
* Title: Lab4 - Part 2 
* Description: This is the program for the client that will attempt to do a
* UDP connect with and request a file to a server 
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <arpa/inet.h>

//This is a Header structure for the length of a packet
typedef struct {
    int len;
} Header;

//This is a structure for the packet data and the Header
typedef struct {
    Header header;	// Contains the length of the packet
    char data[10];	// Contains the actual data of the packet, 10 characters at a time
} Packet;

//Declare server address to which to bind for receiving messages and client address
struct sockaddr_in serv_addr, client_addr;

//Take in the packet, prints the packet information; header length and the packet data
void printPacket(Packet packet) {
    printf("Packet header: %d, Data: \"", packet.header.len);	// Print the packet length information
    fwrite(packet.data, (size_t)packet.header.len, 1, stdout);	// Print the packet data
    printf("\n");
}

//This function handles the ACKNOWLEDGEMENT that the server will send to the client
void clientSend(int sockfd, const struct sockaddr *address, socklen_t addrlen, Packet packet) {
    while (1) {	
        //Send the packet to the server
        printf("Client sending packet\n");
        sendto(sockfd, &packet, sizeof(packet), 0, address, addrlen);
        
        //This function should receieve an ACKNOWLEDGEMENT from the server
        Packet recvpacket;
        recvfrom(sockfd, &recvpacket, sizeof(recvpacket), 0, (struct sockaddr *)address, &addrlen);
        
        //Call the print function to print the receieved packet
        printPacket(recvpacket);
        break;
    }
}

/*
 * Socket
 * Sendto
 * Sendto / Recv
 * Close
 */
int main(int argc, char *argv[]) {
    //Get from the command line, server IP, Port and src file
    if (argc != 4) {
        printf("Usage: %s <ip> <port> <srcfile>\n", argv[0]);
        exit(0);
    }
    
    //Declare socket file descriptor.
    int sockfd; 

    //Open a UDP socket, if successful, returns a descriptor  
    if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("Socket creation failed!\n");
		exit(1);
	}  
    
    //Declare server address; We have to get the host to connect to
    struct sockaddr_in servAddr;
    struct hostent *host;
    host = (struct hostent *) gethostbyname(argv[1]);	// We will be passing in the IP through the first argument
    
    //We have to clear the memory segment holding servAddr
    memset(&servAddr, 0, sizeof(servAddr));

    //Set the server address to send using socket addressing structure 
    servAddr.sin_port = htons(atoi(argv[2]));	// The port is provided by the second argument
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr= *((struct in_addr *)host->h_addr);

    //The file is given by the third argument, which is the source file that we want to send to the server
    int fp = open(argv[3], O_RDWR);
    if(fp < 0){
    	perror("Could not open the file!\n");
	    exit(1);
    }

    //Send the contents of our source file to the server
    socklen_t addr_len = sizeof(servAddr);
    Packet packet;
    int bytes;
    while((bytes=read(fp, packet.data, sizeof(packet.data))) > 0){	// Keep reading the contents of the source file, and input it into the packet (the packet acts as the buffer)
    	packet.header.len = bytes;	// Update the length of this packet to the length of the string that we are sending over to the server
    	clientSend(sockfd, (struct sockaddr *)&servAddr, addr_len, packet);	// Call the send function, send the packet to the server via sockfd and servAddr
    }
    //sending zero-length packet (final) to server to end connection
    Packet final;
    final.header.len=0;
    clientSend(sockfd,(struct sockaddr *)&servAddr, addr_len, final);

	//Close the file descriptor and socket descriptor
    close(fp);
    close(sockfd);
    return 0;
}
