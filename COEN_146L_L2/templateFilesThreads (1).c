// COEN 146L : Lab 2 - template to use for creating multiple thread to make file transfers (step 4)
/* 
* Name: Carlo Bilbao 
* Date: 10/6/22 
* Title: Lab2 – Threads, Circuit, and Packet Switching 
* Description: This program copies multiple files simultaneously 
* by implementing threads 
* NOT WORKING: IT IS COPYING THE FILE, AND DOING MULTIPLE THREADS, BUT THE RESULT IS A FILE THAT ENDS IN A BINARY SEQUENCE \00\00..
*/

#include <stdio.h>   // fprintf(), fread(), fwrite(), fopen(), fclose()
#include <stdlib.h>  // malloc(), free()
#include <pthread.h> // pthread_create()
#include <unistd.h>  // read(), write()
#include <fcntl.h>   // open(), close()
#include <errno.h>   // errno
#include <time.h>    // clock()

#define BUF_SIZE 2048 //buffer size

// data structure to hold copy
struct copy_struct {
	int thread_id; 
	char* src_filename;
	char* dst_filename; 
};

// copies a files from src_filename to dst_filename using functions fopen(), fread(), fwrite(), fclose()
int func_copy(char* src_filename, char* dst_filename) {
	//SETUP; These are the variables for the streams that we will be using to read and write
	FILE* src;	// - The source stream, which we will extract from
	FILE* dst;	// - The destination stream, which we will modify
	
	src = fopen(src_filename, "r");	// opens a file for reading
	if (src == NULL) { // fopen() error checking
		fprintf(stderr, "unable to open %s for reading: %i\n", src_filename, errno);
		exit(0);
	}
	dst = fopen(dst_filename, "w");	// opens a file for writing; erases old file/creates a new file
	if (dst == NULL) { // fopen() error checking
		fprintf(stderr, "unable to open/create %s for writing: %i\n", dst_filename, errno);
		exit(0);
	}

	char* buf = malloc((size_t)BUF_SIZE);  // allocate a buffer to store read data, make it an array of characters
	int bytes_read; // - Variable for number of bytes read for each function call, which will be used in the while
	// - loop to track if there is anything else to copy
	// reads content of file is loop iterations until end of file
	// - Reads up to the amount of BUF_SIZE, check if the current item we are reading is at least 1 character
	// - uses the buf (buffer) to interact temporarily with data, size 1 byte, number of elements in buf, into data stream src
	while ((bytes_read = fread(buf, 1, BUF_SIZE, src)) > 0) { 
	// writes bytes_read to dst_filename 
		// - Takes the buf (buffer), uses it to write data into data stream dst
		// - Copies each time a size of 1 byte, writing the number of elements in buf (BUF_SIZE), and into data stream dst
		if (fwrite(buf, 1, bytes_read, dst) < 0)
		{
			fprintf(stderr, "Write error: %i\n", errno);
			exit(0);
		}
		fprintf(stderr, "COPYING \n");
	}
		
	// closes src file pointer
	// - Uses function close variant to close the stream
	fclose(src);
	// closes dst file pointer
	fclose(dst);
	// frees memory used for buf
	free(buf);
	return 0;
}

// thread function to copy one file
void* copy_thread(void* arg) {
	struct copy_struct params = *(struct copy_struct*)arg;  // cast/dereference void* to copy_struct
	printf("thread[%i] - copying %s to %s\n", params.thread_id, params.src_filename, params.dst_filename);
	//call file copy function
	// - We are taking the attributes from the struct to extract the source and destination file names,
	// - provided by the parameters to the function
	func_copy(params.src_filename, params.dst_filename);
	pthread_exit(NULL);
}

int main(int argc, char* argv[]) {
	 // check correct usage of arguments in command line
	 // - For our example, we will have 10 arguments, but we will make it scalable
	 // - To do this, take total size of array and divide by size of array element
	 // - sizeof(a[0]) is equivalent sizeof(*a), which is the size of first element
	 // - REDACTED: sizeof(argv) / sizeof(argv[0])
	 // - argc gives number of arguments in argv, set to 3: first is source file
	 // - second is destination file, third is number of arguments
	if (argc != 8) {  
		fprintf(stderr, "usage: %s <n connections> ........\n", argv[0]);
		exit(1);
	}
	// - We are passing arguments in (src, dest) pairs
	char* src_filename[] = {argv[1],argv[3],argv[5]};	// - Let second argument passed by source file name
	char* dst_filename[] = {argv[2],argv[4],argv[6]};	// - Let third argument passed be destination file name
	// - Scale the problem to work for 3 files/threads, so set the arrays to be size 3
	char* src_filenames[atoi(argv[7])]; // array of source files
	char* dst_filenames[atoi(argv[7])]; // array of desintation files
	int num_threads = atoi(argv[7]); // number of threads to create, which is fourth argument passed
	printf("Num threads = %i\n", atoi(argv[7]));
	
	pthread_t threads[num_threads]; //initialize threads
	struct copy_struct thread_params[num_threads]; // structure for each thread
	int i;
	// - Skip the two previous arguments to get to the next one
	for (i = 0; i < num_threads; i++) {
		// initialize thread parameters
		// - Set the source and destination filenames
		// - For each corresponding thread
		printf("Current i = %i\n", i);
		src_filenames[i] = src_filename[i];
		dst_filenames[i] = dst_filename[i];
		thread_params[i].src_filename = src_filenames[i];
		thread_params[i].dst_filename = dst_filenames[i];
		
		/*pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
		pthread_cond_t full = PTHREAD_COND_INITIALIZER;
		pthread_cond_t empty = PTHREAD_COND_INITIALIZER;
		pthread_mutex_init($lock, NULL);
		pthread_cond_init($full, NULL);
		pthread_cond_init($empty, NULL);*/
		threads[i] = i;
		thread_params[i].thread_id = threads[i];
		
		
		// create each copy thread
		// use pthread_create(.....);
		// - Create a thread, using thread ID threads[i], no thread attributes (NULL),
		// - passing in the function that the thread will run in parallel, pass in the
		// - function parameters (thread_params), and no function arguments (NULL)
		// - Since you are using an array for threads[], you will not lose reference
		// - to any; using a single pthread_t thread will lose reference to all the first
		// - previous (9) threads (10 in our example)
		pthread_create(&threads[i], NULL, copy_thread, &thread_params[i]);
	}

	// wait for all threads to finish
	for (i = 0; i < num_threads; i++) {
		pthread_join(threads[i], NULL);
	}
	pthread_exit(NULL);
}
