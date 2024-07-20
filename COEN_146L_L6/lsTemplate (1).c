// COEN 146L - Lab 6: Link State routing
/* 
* Name: Carlo Bilbao 
* Date: 11/15/22
* Title: Lab6 - Link State Routing 
* Description: This program attempts to implement a link state
* routing algorithm
*/
 
#include <unistd.h>
#include <fcntl.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <pthread.h>
#include <arpa/inet.h>


//defines
#define	N			4
#define	INFINITE	1000
#define MIN(a,b)	(((a) < (b)) ? (a) : (b))


// types
typedef struct routers
{
	char	name[50];
	char	ip[50];
	int		port;	

} ROUTERS;


// global variables
ROUTERS	routers[N];
int			costs[N][N];
int			distances[N];
int			myid, nodes;
int			sock;
struct sockaddr_in addr;
struct sockaddr_in otheraddr;
socklen_t addr_size;
pthread_mutex_t	lock;

// print costs 
void print_costs (void)
{
	// print costs
	for (int i = 0; i < N; i++)	// The row variable of the costs, corresponds to actual node
	{
		for (int j = 0; j < N; j++)	// The column variable of the costs, corresponds to the neighbor of the row node
		{
			pthread_mutex_lock(&lock);	// Lock costs array
			printf("%d ", costs[i][j]);	// Print the <i, j> entry
			pthread_mutex_unlock(&lock);	// Unlock costs array
		}
		printf("\n");
	}
}

// receive info, one of the functions that threads will be running
void * receive_info (void *arg)
{
	int buf[3];	// Variable for the buffer to receive from socket
	int nbytes;	// Size of the buffer
	int n0, n1, n2;	// Initialize three nodes
	
	//Code to receive info and update costs
	//Continuous loop
	while (1)
	{
		//We are going line by line
		if (recvfrom(sock, buf, sizeof(int) * 3, 0, NULL, NULL) != 12)	// Each node cost is 3 digits max (100 is max), 4 nodes in total each line, so 4 * 3 = 12
		{
			printf("Error receiving!\n");	// Error case, didn't read enough nodes
			exit(1);
		}
		
		n0 = ntohl(buf[0]);	// First node
		n1 = ntohl(buf[1]);	// Second node
		n2 = ntohl(buf[2]);	// Third node
		
		//Update costs
		pthread_mutex_lock(&lock);	// Lock
		costs[n0][n1] = n2;
		costs[n1][n0] = n2;
		pthread_mutex_unlock(&lock);	// Unlock
		
		printf ("Received!\n");
		print_costs();	// Print the costs
	}

}

// run_link_state
void * run_link_state (void *arg)
{
	int	taken[N];
	int	min, spot;
	int	i, j;
	int	r;

	while (1)
	{
		r = rand () % 10;
		sleep (r);
	
		for (i = 0; i < N; i++)
		{
			taken[i] = 0;
			pthread_mutex_lock (&lock);
			distances[i] = costs[myid][i];
			pthread_mutex_unlock (&lock);
		}
		taken[myid] = 1;
	
		for (i = 1; i < N; i++)
		{
			// find the closest node
			min = INFINITE;	// Variable for the minimum distance
			spot = -1;	// Index variable, corresponds to the 
			for (j = 0; j < N; j++)	// Run for the number of nodes, corresponds to parallel elements in taken and distances
			{
				if (taken[j] == 0)	// Check if the current element in the taken table is not taken yet, means value of 0
				{
					if (distances[j] < min)	// Check if current element is smaller than current minimum
					{
						min = distances[j];	// Update minimum
						spot = j;	// Take note of the spot of the minimum element
					}
				}
			}
			
			//Put the closest node in the set
			taken[spot] = 1;
	
		
			
			// recalculate distances
			for (j = 0; j < N; j++)
			{
				if (taken[j] == 0)
				{
					pthread_mutex_lock (&lock);
					distances[j] = MIN (distances[j], (distances[spot] + costs[spot][j]));
					pthread_mutex_unlock (&lock);
				}
			}
		}

		printf ("new distances:\n");
		for (i = 0; i < N; i++)
			printf ("%d ", distances[i]);
		printf ("\n");
	}
}

//main()
int main (int argc, char *argv[])
{
	FILE	*fp;
	int		i, j;
	pthread_t	thr1, thr2;
	int		id, cost;
	int		packet[3];

    //Get from the command line, id, routers, cost table
    if (argc != 5) {
        printf("Usage: %s <id> <nodes> <routers> <cost table>\n", argv[0]);
        exit(0);
    }

	myid = atoi (argv[1]);
	nodes = atoi (argv[2]);

	if (myid >= N)
	{
		printf ("wrong id\n");
		return 1;
	}	

	if (nodes != N)
	{
		printf ("wrong number of nodes\n");
		return 1;
	}
	
	//get info on routers 
	if ((fp = fopen (argv[3], "r")) == NULL)
	{
		printf ("can't open %s\n", argv[3]);
		return 1;
	}

	for (i = 0; i < nodes; i++)
		fscanf (fp, "%s%s%d", routers[i].name, routers[i].ip, &routers[i].port);

	fclose (fp);

	// get costs
	if ((fp = fopen (argv[4], "r")) == NULL)
	{
		printf ("can't open %s\n", argv[4]);
		return 1;
	}

	for (i = 0; i < nodes; i++)
	{
		for (j = 0; j < nodes; j++)
		{
			fscanf (fp, "%d", &costs[i][j]);
		}
	}

	fclose (fp);
		

    // init address
    addr.sin_family = AF_INET;
    addr.sin_port = htons ((short)routers[myid].port);
    addr.sin_addr.s_addr = htonl (INADDR_ANY);
    memset ((char *)addr.sin_zero, '\0', sizeof (addr.sin_zero));
    addr_size = sizeof (addr);

    // create socket
    if ((sock = socket (AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        printf ("socket error\n");
        return 1;
    }

    // bind
    if (bind (sock, (struct sockaddr *)&addr, sizeof (addr)) != 0)
    {
        printf ("bind error\n");
        return 1;
    }


	// create threads
	pthread_mutex_init (&lock, NULL);
	pthread_create (&thr1, NULL, receive_info, NULL);
	pthread_create (&thr2, NULL, run_link_state, NULL);

	// read changes from the keyboard
	for (i = 0; i < 3; i++)
	{
		printf ("any changes? ");
		scanf ("%d%d", &id, &cost);
		if (id >= N  ||  id == myid)
		{
			printf ("wrong id\n");
			break;
		}

		pthread_mutex_lock (&lock);
		costs[myid][id] = cost;
		costs[id][myid] = cost;
		pthread_mutex_unlock (&lock);
		print_costs ();

		packet[0] = htonl (myid);
		packet[1] = htonl (id);
		packet[2] = htonl (cost);
		otheraddr.sin_family = AF_INET;
		addr_size = sizeof (otheraddr);

		for (j = 0; j < N; j++)
		{
			if (j != myid)
			{
				otheraddr.sin_port = htons ((short)routers[j].port);
				inet_pton (AF_INET, routers[j].ip, &otheraddr.sin_addr.s_addr);
				sendto (sock, packet, sizeof (packet), 0, (struct sockaddr *)&otheraddr, addr_size);
			}
		}
		printf ("sent\n");
	}

	sleep (20);
 return 0;
}
