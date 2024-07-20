// COEN 146L : Lab 1 - template to use for file transfer (steps 3, 4, and 5)
// You need to adapt this template according to the requirements of each of the steps
// Name: Carlo Bilbao 
// Date: 10/6/22 
// Title: Lab1 – Basic Linux Commands, C Programming Skills 
// Description: This program copies copies files using function calls and system calls and calculates the time it takes to run

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

#define BUF_SIZE 2048 //buffer size
// For the clock segment
clock_t start, end;	// Unused
double cpu_time_used;

// copies a files from src_filename to dst_filename using functions fopen(), fread(), fwrite(), fclose()
int func_copy(char* src_filename, char* dst_filename) {
	//SETUP
	FILE* src;
	FILE* dst;
	
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

	char* buf = malloc((size_t)BUF_SIZE);  // allocate a buffer to store read data
	// reads content of file is loop iterations until end of file
	int bytes_read; // Variable for number of bytes read for each function call
	while ((bytes_read = fread(buf, 1, BUF_SIZE, src)) > 0) { // Reads up to the amount of BUF_SIZE; uses the buf (buffer) to interact temporarily with data, size 1 byte, number of elements in buf, into data stream src
		if (fwrite(buf, 1, bytes_read, dst) < 0)	// Takes the buf (buffer), uses it to write data into data stream dst
		{
			fprintf(stderr, "Write error: %i\n", errno);
			exit(0);
		}
	} 
	
	// closes src file pointer
	fclose(src);
	// closes dst file pointer
	fclose(dst);
	// frees memory used for buf
	free(buf);
	return 0;
}

// copies a files form src_filename to dst_filename using syscalls open(), read(), write(), close()
int syscall_copy(char* src_filename, char* dst_filename)  {
	//SETUP
	int src_fd;
	int dst_fd;
	
	src_fd = open(src_filename, O_RDONLY);	// opens a file for reading
	if (src_fd < 0) { // open() error checking
		fprintf(stderr, "unable to open %s for reading: %i\n", src_filename, errno);
		close(src_fd);
		exit(0);
	}
	// O_WRONLY - write only
	// O_CREAT - create file if it doesn't exist
	// O_TRUNC - truncates file to length 0 if it already exists (deletes existing file)
	// opens a file for writing; erases old file/creates a new file
	dst_fd = open(dst_filename, O_WRONLY | O_CREAT | O_TRUNC);	
	if (dst_fd < 0) { // open() error checking
		fprintf(stderr, "unable to open/create %s for writing: %i\n", dst_filename, errno);
		close(dst_fd);
		exit(0);
	}


	// read/ write loop
	char* buf = malloc((size_t)BUF_SIZE);   // allocate a buffer to store read data
	int bytes_read; // Variable for number of bytes read for each call
	while ((bytes_read = read(src_fd, buf, BUF_SIZE)) > 0) {	// reads up to BUF_SIZE bytes from src_filename
	// writes bytes_read to dst_filename
		if (write(dst_fd, buf, bytes_read) < 0) // Writes bytes_read into dst_filename
		{
			fprintf(stderr, "Write error: %i\n", errno);
			exit(0);
		}
	}
	// closes src_fd file descriptor
	close(src_fd);
	// closes dst_fd file descriptor
	close(dst_fd);
	// frees memory used for buf
	free(buf);

	return 0;
}

// checks the runtime to copy a file using functions vs syscalls
void check_copy_times(char* src_filename, char* dst_filename) {
	clock_t func_start, func_end, syscall_start, syscall_end;
	//SETUP
	double func_time, syscall_time;
	
	// capture runtime of func_copy() using start clock, call the copy, end the clock
	func_start = clock();
	func_copy(src_filename, dst_filename); /* Time-consuming process */
	func_end = clock();
	cpu_time_used = ( (double) (func_end - func_start) ) / CLOCKS_PER_SEC;
	func_time = cpu_time_used;

	// capture runtime of syscall_copy() using start clock, call the copy, end the clock
	syscall_start = clock();
	//syscall_copy("src.dat", "dest.dat"); /* Time-consuming process */
	syscall_end = clock();
	cpu_time_used = ( (double) (syscall_end - syscall_start) ) / CLOCKS_PER_SEC;
	syscall_time = cpu_time_used;

	printf("time to copy using functions: %.7f\n", func_time);
	printf("time to copy using syscalls: %.7f\n", syscall_time);
}

int main(int argc, char* argv[]) {
	if (argc != 3) {  // check correct usage
		fprintf(stderr, "usage: %s <src_filename> <dst_filename>\n", argv[0]);
		exit(1);
	}
	//get the source and destination files from the command line arguments
	char* src_filename;
	char* dst_filename;
	src_filename = argv[1];	// First argument when launching program
	dst_filename = argv[2];	// Second argument when launching program
	//call the check copy times function
	check_copy_times(src_filename, dst_filename);	// Pass in the parameters, the file names
	return 0;
}
