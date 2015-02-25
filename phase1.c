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
#define SSD_SIZE         100
#define RAM 0
#define SSD 1
#define HD  2

typedef signed short vAddr;

int page_cnt,ram_cnt,ssd_cnt,hd_cnt;

struct page_table//page table must have 1000 entries
{
  // virtual_addr is basically an index to page_table)entry
  //vAddr virtual_addr;	// page frame number (vAddr), 1 to 1000
    int valid;		// valid bit, make sure valid bits are zero at start 
  struct memory
  {
    int physical_addr;    // index to ram, ssd, hd arrays
    int present;          // in which memory is the page
  }ram,ssd,hd;
  int modified;         // modified bit (int) is only used for RAM
  int referenced;       // reference bit (int) is only used for RAM
  int lock;		// lock for current user
  int history;		// count variable for LRU replacement algorithm

}page_table_entry[HD_SIZE];//index to page_table_entry is vAddr

// these arrays hold page values
int ram[RAM_SIZE];
int ssd[SSD_SIZE];
int hard_disk[HD_SIZE];

vAddr allocateNewInt();
int * accessIntPtr(vAddr address);
void unlockMemory(vAddr address);
void freeMemory(vAddr address);

void pageFaultHandler()
{}

vAddr allocateNewInt()
{
  int i=0;
	
  // if new page available (if count=999, cant do)
  if(page_cnt == 1000)
    {
      printf("No available pages\n");
      return -1;
    }
  if(ram_cnt<25)
    {
      for(i=0;i<HD_SIZE;++i)
	if(page_table_entry[i].valid==0)
	  break;//we found an available page
	    
      page_table_entry[i].valid=1;//reserve this page
      
      page_table_entry[i].ram.present=1;//mark as present in ram
	    
      // map this page to ram
      page_table_entry[i].ram.physical_addr = ram_cnt;
      ram_cnt += 1;
      return i;//return virtual address
    }
  else
    {
      //call page fault handler
	    
      // if successful return vAddr to user, else error
    }

}

int * accessIntPtr(vAddr address)
{
  
  if(address >=1000 || address <=-1)
    {
      printf("Address out of range\n");
      return NULL;
    }

  if(page_table_entry[address].valid == 0)
    {
      printf("Address not valid\n");
      return NULL;      
    }

  // add another if statement that checks if page is locked
  // if it is locked, then busy wait for it to unlock?

  // is address in RAM
  if(page_table_entry[address].ram.present == 1)
    { // in RAM it is, return the pointer to the page
      page_table_entry[address].lock = 1;//lock page
      return &ram[page_table_entry[address].ram.physical_addr];
    }
  else
    { // not in RAM, call page fault handler
      
      // if successful return int* to user, else error
    }

}

void unlockMemory(vAddr address)
{
  if(address >=1000 || address <=-1)
    {
      printf("Address out of range\n");
      return;
    }

  if(page_table_entry[address].valid == 0)
    {
      printf("Address not valid (free page)\n");
      return;      
    }
  
   if(page_table_entry[address].ram.present == 1)
    { // in RAM it is
      page_table_entry[address].lock = 0;//lock page
      return;
    }
  else
    { // not in RAM, just unlock anyway
      page_table_entry[address].lock = 0;//lock page
      // do we need page handler here?
      return;
    }
}

void freeMemory(vAddr address)
{
  if(address >=1000 || address <=-1)
    {
      printf("Address out of range\n");
      return;
    }

  if(page_table_entry[address].valid == 0)
    {
      printf("Address not valid (free page)\n");
      return;      
    }
  
   if(page_table_entry[address].ram.present == 1)
    { // in RAM it is
      page_table_entry[address].valid = 0;//lock page
      return;
    }
  else
    { // not in RAM, just unlock anyway
      page_table_entry[address].valid = 0;//lock page
      // do we need page handler here?
      return;
    }
}

/* To do list:
 * implement LRU and WSClock paging replacement alg.
 * test with several users (test locks and protection)
 * why does free have to delete pages rather than just deallocating
 * 
 * pages in ssd and hd or ssd, hd, and ram count as one
 * tage table should be reentrant
 * free does not need to delete
 * where do data go, page table, no?ask forum
 * free can work after allocate
 * semaphore for each page
 * do we need an array or could we use struct?
 * you cant assume magical hardware
 * what is virtal and what physical addr. in our case?
 * virtual: 0-999
 * 
 */ 

/* MAIN */

int main(int argc, char **argv) {
  
  page_cnt=ram_cnt=ssd_cnt=hd_cnt=0;
  
  int i=0;
  // initialize all vAddresses
  for(i=0;i<HD_SIZE;++i)
    {
      page_table_entry[i].valid = -1;//not valid
      //page_table_entry[i].page_number = i;
      //page_table_entry[i].memory = -1;//no where in memory
    }
	
  return EXIT_SUCCESS;
}
