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

#define RAM_SIZE 	25
#define HD_SIZE 	1000

typedef signed short vAddr;

struct page_table
{
	int data;		// data is only single interger
	vAddr page_number;	// page frame number (vAddr address), 0 to 1000
	int valid;		// valid bit, make sure valid bits are zero at start 
	int modified;
	int referenced;
	int lock;		// lock for current user
	int history;		// count variable for LRU replacement algorithm
	int memory;		// where is the page, ram ssd or hd

}page_table_entry[HD_SIZE];

struct ram_table
{
	//int valid[RAM_SIZE];	// is page allocated?
	//int index_page_table[RAM_SIZE];	// this holds index to page_table_entry

	struct page_table * page_ram[25];
	int count;		//how many valid pages
};

struct ram_table RAM;

// not sure if we need the arrays if we uses structs as above
int ram[RAM_SIZE];
int ssd[100];
int hard_disk[1000];

vAddr allocateNewInt();
int * accessIntPtr(vAddr address);
void unlockMemory(vAddr address);
void freeMemory(vAddr address);

void pageFaultHandler()
{}

vAddr allocateNewInt()
{
	int i=0;
	
	// try to put in RAM
	// find first available page slot
	for(i=0;RAM->page_ram.valid[i]==0 && i!=RAM_SIZE;++i);

	if(i!=RAM_SIZE)
	{
		RAM->page_ram.valid[i] = 1;	//allocate this one
		return 1;
	}	
	else	//else RAM full => page fault => eviction algorithm 
	{		
		pageFaultHandler();
		return -1;
	}
}

int * accessIntPtr(vAddr address)
{
	int i=0;
	// linear search for page in RAM
	for(i=0;RAM->page_ram.page_number[i]!=address && i!= RAM_SIZE;++i);
	
	if(i!=RAM_SIZE)
	{
		return &(RAM->page_ram.data[i]);	//allocate this one
	}	
	else	//else RAM full => page fault => eviction algorithm 
	{		
		// todo		
		pageFaultHandler();
		
	}

}

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

