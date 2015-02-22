// Students: Jillian Hennessy; Stefan Gvozdenovic; Myles Karam
// Date: February 21, 2015
// Operating Systems CS3013 C'15 WPI
// Professor: Craig Shue
// Project4 - virtual memory
// Description: see *.pdf

#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h> /* semaphores */
#include <unistd.h> /*usleep*/

struct page_table
{
	int data;		// data is only single interger
	int page_number;	// page frame number, 0 to 1000
	int valid;		// valid bit
	int modified;
	int referenced;
	int lock;		// lock for current user
	int history;		// count variable for LRU replacement algorithm
};

/* To do list:
 * implement LRU and WSClock paging replacement alg.
 * 
 *
 *
 *
 *
 *
 */

/* MAIN */

int main(int argc, char **argv) {

	
  return EXIT_SUCCESS;
}

