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

int ram[25];
int ssd[100];
int hard_disk[1000];

typedef signed short vAddr;

vAddr allocateNewInt();
int * accessIntPtr(vAddr address);
void unlockMemory(vAddr address);
void freeMemory(vAddr address);

/* To do list:
 * implement LRU and WSClock paging replacement alg.
 * test with several users (test locks and protection)
 * why does free have to delete pages rather than just deallocating
 *
 *
 *
 */

/* MAIN */

int main(int argc, char **argv) {

	
  return EXIT_SUCCESS;
}

