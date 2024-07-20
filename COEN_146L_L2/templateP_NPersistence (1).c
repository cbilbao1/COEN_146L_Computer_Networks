//COEN 146L : Lab2, step 4
/* 
* Name: Carlo Bilbao 
* Date: 10/6/22 
* Title: Lab2 – Threads, Circuit, and Packet Switching 
* Description: This program calculates the time elapsed when doing DNS lookups
* 1 vs 6 objects, for non-persistent and persistent with n parallel connections 
*/
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

int main(int argc, char *argv[]) {
        if (argc != 2) {  // check correct usage
		fprintf(stderr, "usage: %s <n connections> \n", argv[0]);
		exit(1);
	}
        //Declare and values to n (n connections), np (np parralel connections), RTT0, RTT1, RTT2, RTTHTTP, RTTDNS, .. 
        int RTT0 = 3;	// - Delay for DNS servers
        int RTT1 = 20;
        int RTT2 = 26;
        int RTTHTTP = 47;	// - Delay between local host and the Web Server
        int n = atoi(argv[1]);	// - Number of parallel TCP connections, taken from command line argument
        int timeOne = 0;
        int timeSix = 0;
        int timePers = 0;
        int timeNonP = 0;
        
        // - a. = To find the time between client clicks link and receives object
        // - RRT for establishing TCP connection
        // - Then another RRT to get/recieve object -> 2 * RTTHTTP
        // - 2RTTHTTP + (RTT0 + RTT1 + RTT2 +... RTTn)
        timeOne = 2 * RTTHTTP + RTT0 + RTT1 + RTT2;
        
        // - b. Find the time instead for 6 objects, non-persistent HTTP
        // - RRT for establishing TCP connection
        // - Then another RRT to get/recieve object -> 2 * RTTHTTP
        // - Then same process for 6 more objects (6*(2 * RTTHTTP))
        timeSix = 2 * RTTHTTP + (6 * 2 * RTTHTTP) + RTT0 + RTT1 + RTT2;
        
        printf("One object: %d msec\n", timeOne);
        printf("Non-Persistent 6 objects: %d msec\n", timeSix);

        //find how many np (parralel connections)
        // - c. Compute response time in both persistent/non-persistent
        // - 2 * RTT + transmitTime
        
        // - Persistent case:
        //timePers = 2 * RTTHTTP + n * RTTHTTP;
        //timePers = (RTT0 + RTT1 + RTT2) + 2*RTTHTTP + (n - 1)*0.5*RTTHTTP;
        //timePers = (RTT0 + RTT1 + RTT2) + 2*RTTHTTP + RTTHTTP;
        timePers = (RTT0 + RTT1 + RTT2) + 2*RTTHTTP + (int)(ceil(6.0 / n))*RTTHTTP;
        
        // - Non persistent case:
        //timeNonP = 2 * RTTHTTP + 2 * n * RTTHTTP;
        //timeNonP = (RTT0 + RTT1 + RTT2) + 2*RTTHTTP + (n - 1)*RTTHTTP;
        //timeNonP = (RTT0 + RTT1 + RTT2) + 6*2*RTTHTTP;
        timeNonP = (RTT0 + RTT1 + RTT2) + 2*RTTHTTP + 2*(int)(ceil(6.0 / n))*RTTHTTP;
        
        printf("N: %f\n", (ceil(6.0 / n)));
        
        printf("%d parallel connection - Persistent: %d msec\n", n, timePers);
        printf("%d parallel connection - Non-Persistent: %d msec\n", n, timeNonP);

return 0;
}
