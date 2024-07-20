//COEN 146L, Lab 4, Step 1
/* 
* Name: Carlo Bilbao 
* Date: 10/28/22
* Title: Lab4 - Part 1 
* Description: This is the program for the server that will attempt to do a
* UDP connect with and request a file to a server 
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
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

//Declare client address to fill in address of sender
struct sockaddr_in serv_addr, client_addr;

//Take in the packet, prints the packet information; header length and the packet data
void printPacket(Packet packet) {
    printf("Packet header: %d, Data: \"", packet.header.len);	// Print the packet length information
    fwrite(packet.data, (size_t)packet.header.len, 1, stdout);	// Print the packet data
    printf("\n");
}

//This function handles the ACKNOWLEDGEMENT that the server will send to the client
void serverSend(int sockfd, const struct sockaddr *address, socklen_t addrlen) {
    Packet packet;	// Packet structure	
    strcpy(packet.data, "ACKNOWLEDG");	// This is the packet ACKNOWLEDGEMENT statement from the server, sending to client
    packet.header.len = sizeof(packet.data);	// Update the length of this packet to the length of the string that we are sending over to the client
	
    //Now we send the packet through our socket
    sendto(sockfd, &packet, sizeof(packet), 0, address, addrlen);	// Send to this socket
    printf("\t Server sending \"%s\" \n", packet.data);	// Display what we are sending to the client
}

//This is the "main" (handler) function when we receive a packet; call print and send packet
Packet serverReceive(int sockfd, struct sockaddr *address, socklen_t *addrlen) {
    Packet packet;
    while (1) {
        //This function will receive the specified information about the packets from the client
	    recvfrom(sockfd, &packet, sizeof(packet), 0, address, addrlen);
        
        //Call the print function for our packets
        printPacket(packet);
        
        //Send an ACKNOWLEDGEMENT to the client
	    serverSend(sockfd, address, *addrlen);
        break;
    }
    return packet;
}

/*
 * Socket
 * Bind
 * Sendto / Recvfrom
 * Close
 */
int main(int argc, char *argv[]) {
    //Get from the command line, server IP and dst file
    if (argc != 3) {
        printf("Usage: %s <port> <dstfile>\n", argv[0]);
        exit(0);
    }
    //The socket file descriptor
    int sockfd; 

    //Create a UDP socket, returns an int descriptor, use SOCK_DGRAM for UDP
    if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("Socket creation failed!\n");
		exit(1);
	} else
	{
		printf("Socket creation successful!\n");
	}

    //Setup the server address to bind using socket addressing structure
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(atoi(argv[1]));	// Port number is given by first argument
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    //Call bind
    if((bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(struct sockaddr))) < 0) {
		perror("Server bind failed!\n");
		exit(1);
	}

    //Destination file is provided by second argument; if it doesn't exist, create it
    int fp = open(argv[2], O_CREAT | O_RDWR, 0666);
    if(fp < 0){	// Fail case
	    perror("Could not open the file!\n");
	    exit(1);
    }
    // We will be receiving the file contents of the source file as packets from the client
    Packet packet;	// A packet variable that will take each packet group and write to the destination file
    socklen_t addr_len = sizeof(client_addr);	// The address length of our client
    printf("Waiting for packets from the client..\n");
    do {
        packet = serverReceive(sockfd, (struct sockaddr *) &client_addr, &addr_len);	// Set the current packet group to this variable
	    write(fp, packet.data, packet.header.len);	// Write the current packets receieved to the destination file
    } while (packet.header.len != 0);	// Keep going until the packet length is 0, signifies no more packet data left
    
    //Close the file descriptor and socket descriptor
    close(fp);
    close(sockfd);
    
    return 0;
}
