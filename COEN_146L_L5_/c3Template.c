//COEN 146L, Lab5
/*
 * Name: Carlo Bilbao
 * 10/24/22
 * Title: Lab 5 - Stop and Wait for an Unreliable Channel with error or loss
 * Description: Client Side - Implements protocol rdt3.0
 * UDP client that will work with packets that have a checksum,
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
    Header header;
    char data[10];
} Packet;

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

//client sending packet with checksum and sequence number and waits for acknowledgement, sets up a time
void clientSend(int sockfd, const struct sockaddr *address, socklen_t addrlen, Packet packet, unsigned retries) {	// The last parameter
    while (1) {	
		//if retries is greater than 3, we give up and move on
        if(retries >= 3){
			break;
		}
		
        //verify rdt3.0 protocol by simulate a packet corruption
        int corrupt;
        //you may use user input for deliberate errors:
        //printf("Would you like to simulate packet corruption? Enter 1 for yes, 0 for no\n");
        //scanf("%d",&corrupt);
        //or use a random function instead to generate 0 and 1 randomly:
        corrupt = rand() % 5;
        if (corrupt == 0) //simulate a drop of packet (probability = 20%)
        {
			printf("Dropping packet\n");
		}
        /*else if (corrupt != 0) {
        //send checksum = 0 (to simulate packet corruption)
			packet.header.cksum = 0;
		}*/
		else {
			//or calculate and send the right checksum of packet
			//calculate checksum of packet
				packet.header.cksum = getChecksum(packet);
			//send the packet
			printf("Client sending packet\n");
			sendto(sockfd, &packet, sizeof(packet), 0, address, addrlen);
		}
		
		// wait until an ACK is received or timeout using select statement      
        // local variables needed
        struct timeval tv; // timer
        tv.tv_sec = 1;
        tv.tv_usec = 0;
        int rv;     // select returned value
        
        // set up reads file descriptor at the beginning of the function to be checked for being ready to read
        fd_set  readfds;
        fcntl (sockfd, F_SETFL, O_NONBLOCK);
        // start before calling select
        FD_ZERO (&readfds); //initializes readfds to have zero bits
        FD_SET (sockfd, &readfds); //sets readfds bit
        
			//call select
			// call select () which returns the number of ready descriptors that are contained in the bit masks. 
			// if the time limit expires, select returns zero and sets errno
			rv = select(sockfd + 1, &readfds, NULL, NULL, &tv);
        
        if(rv == 0){
            printf("Timeout\n");
            //increment retries if packet is dropped
            retries++;
        }
        //so, there is an ACK to receive
        else{
            //receive an ACK from the server
            Packet recvpacket;
            recvfrom(sockfd, &recvpacket, sizeof(recvpacket), 0, (struct sockaddr *)address, (socklen_t*)&addrlen);
            //print received packet (ACK) and checksum
            printf("Client received ACK %d, checksum %d - \n", recvpacket.header.seq_ack, recvpacket.header.cksum);
            //calculate checksum of received packet (ACK)
			int e_cksum = getChecksum(recvpacket);
            //check the checksum
            if (recvpacket.header.cksum != e_cksum) {
                //if bad checksum, resend packet
                printf("Client: Bad checksum, expected checksum was: %d\n", e_cksum);
        
            //check the sequence number
			} else if (recvpacket.header.seq_ack != packet.header.seq_ack) {
                //if incorrect sequence number, resend packet
                printf("Client: Bad seqnum, expected sequence number was: %d\n",packet.header.seq_ack);
    
            //if the ckechsum and sequence numbers are correct, break and return to the main to get the next packet to send
            } else {
                //good ACK, we're done
                printf("Client: Good ACK\n");
                break;
            }
        }
        
        /*----
        //receive an ACK from the server
        Packet recvpacket;
        recvfrom(sockfd, &recvpacket, sizeof(recvpacket), 0, (struct sockaddr *)address, (socklen_t*)&addrlen);
        
        //print received packet (ACK) and checksum
        printf("Client received ACK %d, checksum %d - \n", recvpacket.header.seq_ack, recvpacket.header.cksum);

        //calculate checksum of received packet (ACK)
        int e_cksum = getChecksum(recvpacket);
        
        //check the checksum
        if (recvpacket.header.cksum != e_cksum) {
        
        //if bad checksum, resend packet
            printf("Client: Bad checksum, expected checksum was: %d\n", e_cksum);
        
        //check the sequence number
		} else if (recvpacket.header.seq_ack != packet.header.seq_ack) {
        
        //if incorrect sequence number, resend packet
            printf("Client: Bad seqnum, expected sequence number was: %d\n",packet.header.seq_ack);
	    
        //if the ckechsum and sequence numbers are correct, break and return to the main to get the next packet to send
        } else {
            // good ACK, we're done
            printf("Client: Good ACK\n");
            break;
        }----*/
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
		perror("Failed to serup endpoint socket.\n");
		exit(1);
	}

    //Declare server address to connect to
    struct sockaddr_in servAddr;
    struct hostent *host;
    host = (struct hostent *) gethostbyname(argv[1]);	// We will be getting the IP by the first argument, pass into get host function

    //Set the server address to send using socket addressing structure
    memset(&servAddr, 0, sizeof(servAddr));
    
    //initialize servAddr structure
    servAddr.sin_port = htons(atoi(argv[2]));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr = *((struct in_addr *)host->h_addr);

    //open file using argv[3]
    int fp=open(argv[3],O_RDWR);
    if(fp<0){
    	perror("Failed to open the file!\n");
	    exit(1);
    }

    //send file contents to server packet by packet
    int seq = 0;
    socklen_t addr_len = sizeof(servAddr);
    Packet packet;
    int bytes;	// We will use this variable to keep track of how many bytes were read
    while((bytes = read(fp, packet.data, sizeof(packet.data))) > 0){
        //assign seq and checksum to packet and send
    	packet.header.seq_ack = seq;	// Set the sequence number of our current packet
    	//packet.header.len = strlen(packet.data);	// Update the length of this packet to the length of the string that we are sending over to the server (REDACTED)
    	packet.header.len = bytes;	// Update the length of this packet to the length of the string that we are sending over to the server
    	packet.header.cksum = getChecksum(packet);	// Call the calculate checksum function, pass in the packet
    	clientSend(sockfd, (struct sockaddr *) &servAddr, addr_len, packet, 0);	// Call the send function, send the packet to the server; retries = 0

    	seq = (seq + 1) % 2;	//Update the sequence number, alternate between 0 and 1
    }

    // send zero-length packet to server to end connection
    Packet final;
    final.header.seq_ack = seq;
    final.header.len = 0;
    final.header.cksum = getChecksum(final);
    clientSend(sockfd, (struct sockaddr *)&servAddr, addr_len, final, 0);	// Again, retries = 0
    
	//Close the file descriptor and socket descriptor
    close(fp);
    close(sockfd);
    return 0;
}
