//COEN 146L, Lab5
/*
 * Name: Carlo Bilbao
 * 10/24/22
 * Title: Lab 5 - Stop and Wait for an Unreliable Channel with error or loss
 * Description: Server Side - Implements protocol rdt3.0
 * UDP server that will work with packets that have a checksum,
 * sequence number, and acknowledgement number
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/select.h>

//Declare a Header structure that holds, sequence/ acknowledgement number, checksum, and length of a packet
typedef struct {
    int seq_ack;
    int len;
    int cksum;
} Header;

//Declare a packet structure that holds data and header
typedef struct {
    Header header;	// Contains the length of the packet
    char data[10];	// Contains the actual data of the packet, 10 characters at a time
} Packet;

//Declare client address to fill in address of sender
struct sockaddr_in servAddr, clienAddr;

//Calculating the Checksum
/*The simplest checksum algorithm is the so-called longitudinal parity check, 
which breaks the data into "bytes" with a fixed number 8 bits, and then 
computes the exclusive or (XOR) of all those bytes. 
The result is appended to the message as an extra byte*/
int getChecksum(Packet packet) {
    packet.header.cksum = 0;
    int checksum = 0;
    char *ptr = (char *)&packet;
    char *end = ptr + sizeof(Header) + packet.header.len;
    while (ptr < end) {
        checksum ^= *ptr++;
    }
    return checksum;
}

//Print function, prints the packet, the sequence and ACKNOWLEDGEMENT number, and the checksum
void printPacket(Packet packet) {
    printf("Packet{ header: { seq_ack: %d, len: %d, cksum: %d }, data: \"",
            packet.header.seq_ack,
            packet.header.len,
            packet.header.cksum);	// Print the various packet information
    fwrite(packet.data, (size_t)packet.header.len, 1, stdout);	// Print the packet data
    printf("\" }\n");
}

//Server sending ACK to the client
void serverSend(int sockfd, const struct sockaddr *address, socklen_t addrlen, int seqnum) {
	//simulating a chance that ACK gets lost
    if (rand() % 5 == 0){ // this give a probability of 20% for a loss of ACK 
     printf("Dropping ACK\n");
    } else {
		Packet packet;	
		packet.header.seq_ack = seqnum;
		packet.header.len = sizeof(packet.data);
		packet.header.cksum = getChecksum(packet);
		//send Ack packet
		sendto(sockfd, &packet, sizeof(packet), 0, address, addrlen);
    
		printf("\t Server sending ACK %d, checksum %d\n", packet.header.seq_ack, packet.header.cksum);	// Send the ACKNOWLEDGEMENT packet to the client
	}
}

//This function handles receiving the packet from the client; call print, receive, and calculate checksum
Packet serverReceive(int sockfd, struct sockaddr *address, socklen_t *addrlen, int seqnum) {
    Packet packet;
    while (1) {
        //Receive the packets from the client
	    recvfrom(sockfd, &packet, sizeof(packet), 0, address, addrlen);
        
        //Print the packet
        printPacket(packet);
	    
        //Call the checksum function to calculate the checksum, store in temporary variable
        int e_cksum = getChecksum(packet);
        
        //If the checksum does not match, means failed
        if (packet.header.cksum != e_cksum) {
           printf("\t Server: Bad checksum, expected checksum was: %d\n", e_cksum);
            //Fail case, so resend packet
            serverSend(sockfd, address, *addrlen, !seqnum);
        } else if (packet.header.seq_ack != seqnum) {	// If our sequence number ACKNOWLEDGEMENT does not match our current sequence number, failed
            printf("\t Server: Bad seqnum, expected sequence number was:%d\n",seqnum);
            //resend packet
            serverSend(sockfd, address, *addrlen, !seqnum);
        } else {	// If the checksum and sequence numbers are correct
            printf("\t Server: Good packet\n");
            //Send the ACKNOWLEDGEMENT with our current sequence number
	        serverSend(sockfd, address, *addrlen, seqnum);	// Send the right sequence number with seqnum
            break;
        }
    }
    return packet;	// Return our packet, which we will be writing to the destination file in the main function
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

    //Declare socket file descriptor
    int sockfd;

    //Create a UDP socket, returns an int descriptor, use SOCK_DGRAM for UDP
    if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("Socket creation failed!\n");
		exit(1);
	} else
	{
		perror("Socket creation successful!\n");
	}

    //Setup the server address to bind using socket addressing structure
    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(atoi(argv[1]));	// We will get the port from the first argument
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    //bind IP address and port for server endpoint socket
    if((bind (sockfd, (struct sockaddr *)&servAddr, sizeof(struct sockaddr))) < 0) {
		perror("Server bind failed!\n");
		exit(1);
	}

    //open file using argv[2]
    int fp = open(argv[2], O_CREAT | O_RDWR, 0666);
    if (fp < 0) {
		perror("Could not open the file!\n");
		exit(1);
	}
    
    //initialize sequence number to 0
    int seqnum = 0;
    
    //get file contents (as packets) from client
    Packet packet;
    socklen_t addr_len = sizeof(clienAddr);
    printf("Waiting for packets from the client..\n");
    do {
        //receive a packet and check through serverReceive()
        packet = serverReceive(sockfd, (struct sockaddr *)&clienAddr, &addr_len, seqnum);
        //write(fp, packet.data, strlen(packet.data));	// Write the current packets receieved to the destination file (REDACTED)
        write(fp, packet.data, packet.header.len);	// Write the current packets receieved to the destination file
        seqnum = (seqnum + 1) % 2; //Update the sequence number, alternate between 0 and 1
    } while (packet.header.len != 0);	// Keep going until we have nothing left to send (the current packet being processed will have no length)
    
    //Close the file descriptor and socket descriptor
    close(fp);
    close(sockfd);
    return 0;
}
