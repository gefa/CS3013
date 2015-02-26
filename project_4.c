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
#include <time.h>

#define RAM_SIZE 	25
#define SSD_SIZE	100
#define HD_SIZE 	1000
#define RAM	 	0
#define SSD		1
#define HD	 	2
#define waitSSD		250000
#define waitHD		2500000
#define below(x)        x+1    //this macro returns the next memory below in memory hierarchy


typedef signed short vAddr ;
int page_count = 0;
int ram_count = 0;
int ssd_count = 0;
int hd_count = 0 ;
int entriesCounter = 0 ;
vAddr indexes[HD_SIZE] ;

struct page_table_entry
{
	//vAddr virtual_address ;
	int physical_address ;
	int referenced ;
	int lock ;
	int memory_location ;
	int valid ;
}page_table[1000] ;

int ram[RAM_SIZE] ;
int ssd[SSD_SIZE] ;
int hd[HD_SIZE] ;
int * mem[3];
int mem_offset[3];

void init()
{
	int i ;
	for(i = 0 ; i < 1000 ; i++)
	{
	  //page_table[i].virtual_address = i ;
		//page_table[i].physical_address = -1 ;
		page_table[i].referenced = 0;
		page_table[i].lock = 0 ;
		page_table[i].memory_location = -1 ;
		page_table[i].valid = 0 ;
	}
	mem[0]=&ram[0];
	mem[1]=&ssd[0];
	mem[2]=&hd[0];
	mem_offset[0]=0;
	mem_offset[1]=25;
	mem_offset[2]=125;

}

int findFREE(int size) // Find a free slot in any array ie ram ssd or hd
{
	int i ;
	if(size == RAM)
	{
		int free[RAM_SIZE];
		for(i=0;i<RAM_SIZE;++i)
			free[i]=1;//assume all free

		for(i=0;i<HD_SIZE;++i)
		{
			if(page_table[i].memory_location == RAM)
				free[page_table[i].physical_address]=0;//mark the used ram pages
		}

		for(i=0;i<RAM_SIZE;++i)
			if(free[i])//find the first free spot in ram 
				return i;

		return -1;
	}

	else if(size == SSD)
	{
		int free[SSD_SIZE];
		for(i=0;i<SSD_SIZE;++i)
			free[i]=1;//assume all free

		for(i=0;i<HD_SIZE;++i)
		{
			if(page_table[i].memory_location == SSD)
				free[page_table[i].physical_address-25]=0;//mark the used ram pages
		}

		for(i=0;i<SSD_SIZE;++i)
			if(free[i])//find the first free spot in ram 
				return i;
		return -1 ;
	}

	else if(size == HD)
	{
		int free[HD_SIZE];
		for(i=0;i<HD_SIZE;++i)
			free[i]=1;//assume all free

		for(i=0;i<HD_SIZE;++i)
		{
			if(page_table[i].memory_location == HD)
				free[page_table[i].physical_address-125]=0;//mark the used ram pages
		}

		for(i=0;i<HD_SIZE;++i)
			if(free[i])//find the first free spot in ram 
				return i;
		return -1 ;
	}
}

void clockSSD()
{
	int i ;
	int j ;
	int freeSpace = findFREE(HD) ;

	if(freeSpace != -1)
	{
		for(j = 0 ; j < 2 ; j++)//
		{
			for(i = 0 ; i < 1000 ; i++)
			{
				if(page_table[i].memory_location == 1 && page_table[i].lock == 0 && page_table[i].referenced == 0)
				{
					int a = ssd[page_table[i].physical_address - 25] ;
					hd[freeSpace] = a ;
					//ssd[page_table[i].physical_address - 25] = -1 ;
					page_table[i].physical_address = freeSpace + 125 ;
					page_table[i].memory_location = 2 ;
					page_table[i].lock = 0 ;
					//usleep(waitHD) ;
					return ;
				}
				else if(page_table[i].memory_location == 1 && page_table[i].lock == 0 && page_table[i].referenced == 1)
				{
					page_table[i].referenced = 0 ;
				}	
			}
		}		
	}
	else if(freeSpace == -1)
	{
		printf("HD full\n");
		return ;
	}
}



void clockRAM()
{
	int i ;
	int j ;
	int freeSpace = findFREE(SSD) ;
	if(freeSpace != -1)
	{
		for(j = 0 ; j < 2 ; j++)//if all pages happen to be ref=1, do another loop and evict ref=0
		{
			for(i = 0 ; i < 1000 ; i++)
			{
				if(page_table[i].memory_location == 0 && page_table[i].lock == 0 && page_table[i].referenced == 0)
				{
					int a = ram[page_table[i].physical_address] ;
					ssd[freeSpace] = a ;
					//ram[page_table[i].physical_address] = -1 ;
					page_table[i].physical_address = freeSpace + 25 ;
					page_table[i].memory_location = 1 ;
					page_table[i].lock = 0 ;
					//usleep(waitSSD) ;
					return ;
				}
				else if(page_table[i].memory_location == 0 && page_table[i].lock == 0 && page_table[i].referenced == 1)
				{
					page_table[i].referenced = 0 ;
				}
			}
		}	
	}
	else if(freeSpace == -1)
	{
		clockSSD() ;
		freeSpace = findFREE(SSD) ;
		for(j = 0 ; j < 2 ; j++)
		{
			for(i = 0 ; i < 1000 ; i++)
			{
				if(page_table[i].memory_location == 0 && page_table[i].lock == 0 && page_table[i].referenced == 0)
				{
					int a = ram[page_table[i].physical_address] ;
					ssd[freeSpace] = a ;
					//ram[page_table[i].physical_address] = -1 ;
					page_table[i].physical_address = freeSpace + 25 ;
					page_table[i].memory_location = 1 ;
					page_table[i].lock = 0 ;
					//usleep(waitSSD) ;
					return ;
				}
				else if(page_table[i].memory_location == 0 && page_table[i].lock == 0 && page_table[i].referenced == 1)
				{
					page_table[i].referenced = 0 ;
				}
			}
		}
	}
}


vAddr allocateNewInt()
{
	int freeSpace = findFREE(RAM);
	//srand(time(NULL)) ;//

	if(page_count == 1000)
	{
		printf("NOT ENOUGH MEMORY\n") ;
	}

	else if(freeSpace != -1)
	{
		int i ;
		for(i = 0 ; i < 1000 ; i++)
		{
			if(page_table[i].valid == 0)// && page_table[i].physical_address == -1)//
			{
				page_table[i].valid = 1 ;
				page_table[i].physical_address = freeSpace ;
				page_table[i].lock = 0 ;
				page_table[i].memory_location = 0 ;//ram
				page_table[i].referenced = 0 ;
				page_count += 1 ;

				//ram[freeSpace] = entriesCounter ;//what is this
				//entriesCounter += 1 ;
 				break ;
			}
		}
		return i ;
	}

	else if(freeSpace == -1) 
	{
		int i ;
		clockRAM() ;
		freeSpace = findFREE(RAM) ;
		for(i = 0 ; i < 1000 ; i++)
		{
			if(page_table[i].valid == 0) //&& page_table[i].physical_address == -1)
			{
				page_table[i].valid = 1 ;
				page_table[i].physical_address = freeSpace ;
				page_table[i].lock = 0 ;
				page_table[i].memory_location = 0 ;
				page_table[i].referenced = 0 ;
				page_count += 1 ;

				//ram[freeSpace] = entriesCounter ;
				//entriesCounter += 1 ;
 				break ;
			}
		}
		return i ;
	}
}

vAddr evict_clock(int memory_location)
{
	vAddr i;
	int j;
	for(j = 0 ; j < 2 ; j++)//if all pages happen to be ref=1, do another loop and evict ref=0
	{
		for(i = 0 ; i < 1000 ; i++)
		{
			if(page_table[i].memory_location == memory_location && page_table[i].lock == 0 && page_table[i].referenced == 0)
			{
				return i;
			}
			else if(page_table[i].memory_location == 0 && page_table[i].lock == 0 && page_table[i].referenced == 1)
			{
				page_table[i].referenced = 0 ;
			}
		}
	}
	printf("Error_evict\n");
}

void pageFaultHandler(vAddr address,int memory)
{// Aguments explanation:
 // we want to swap the page with virtal address "address" to the
 // memory hierarchy level "memory"
		int freeSpace = findFREE(memory) ;
		if(freeSpace != -1)
		{

		  //swap in page from ssd to ram
		  mem[memory][freeSpace] = mem[below(memory)][page_table[address].physical_address] ;
		  //ram[page_table[i].physical_address] = -1 ;
		  page_table[address].physical_address = freeSpace + mem_offset[memory] ;
		  page_table[address].memory_location = memory ;
		  //usleep(waitSSD) ;
		  return ;
					
		}
		else if(freeSpace == -1)
		{//evict something from ram
			vAddr evict_from = evict_clock(memory);
			vAddr evict_to = evict_clock(below(memory));
			
			//swap values			
			int physical_address = findFREE(below(memory));			
			page_table[evict_to].physical_address = physical_address + mem_offset[below(memory)];
			mem[below(memory)][physical_address] = mem[memory][page_table[evict_from].physical_address];
			page_table[evict_to].memory_location = below(memory);
			page_table[evict_to].valid = 1;
		
			mem[memory][page_table[evict_from].physical_address] = mem[below(memory)][page_table[address].physical_address];
			return ;
		}
		printf("ERROR Oh God I should not be here\n");
		return;
  /*
		int freeSpace = findFREE(RAM) ;
		if(freeSpace != -1)
		{

			//swap in page from ssd to ram
			ram[freeSpace] = ssd[page_table[address].physical_address] ;
			//ram[page_table[i].physical_address] = -1 ;
			page_table[address].valid = freeSpace + 25 ;
			page_table[address].memory_location = 0 ;
			//usleep(waitSSD) ;
			return ;
					
		}
		else if(freeSpace == -1)
		{//evict something from ram
			vAddr evict_from = evict_clock(RAM);
			vAddr evict_to = evict_clock(SSD);
			
			//swap values			
			int physical_address = findFREE(SSD);			
			page_table[evict_to].physical_address = physical_address;
			ssd[physical_address] = ram[page_table[evict_from].physical_address];
			page_table[evict_to].memory_location = SSD;
			page_table[evict_to].valid = 1;
		
			ram[page_table[evict_from].physical_address] = ssd[page_table[address].physical_address];
			return ;
		}
  */

}

int * accessIntPtr(vAddr address)
{
  //int j = page_table[address].physical_address ;//
	int i = page_table[address].memory_location ;
	page_table[address].referenced = 1 ;
	page_table[address].lock = 1 ;
	
	if(i == 0)
	{
	  return &ram[page_table[address].physical_address] ;
	}
	else if(i == 1)
	{	//swap from ssd to ram
	  printf("page in ssd, wait, should be: %d\n",ssd[page_table[address].physical_address - 25]);
	  pageFaultHandler(address,RAM) ;//fault in RAM
	  usleep(waitSSD) ;		
	  return &ssd[page_table[address].physical_address - 25] ;
	}
	else if(i == 2)
	{	//swap in from hd to ssd and from ssd to ram
	  printf("page in hd, wait, should be: %d\n",hd[page_table[address].physical_address - 125]);
	  //note that page fault handler functions have to be in this order
	  pageFaultHandler(address,SSD) ;//fault in SSD, bring page from HD to SSD
	  pageFaultHandler(address,RAM) ;//fault in RAM, bring page from SSD to RAM	  
	  usleep(waitSSD) ;
	  usleep(waitHD) ;
	  return &hd[page_table[address].physical_address - 125] ;
	}	
}


void unlockMemory(vAddr address)
{
	page_table[address].lock = 0 ;
}

void freeMemory(vAddr address)
{
	int j = page_table[address].physical_address ;
	int i = page_table[address].memory_location ;

	//page_table[address].physical_address = -1 ;
	page_table[address].valid = 0 ;
	page_table[address].lock = 0 ;
	page_table[address].memory_location = -1 ;
	page_table[address].referenced = 0 ;
	page_count--;
}

void memoryMaxer()
{
	int i ;
	int *value ;

	for(i = 0 ; i < 1000 ; i++)
	{
		indexes[i] = allocateNewInt() ;
		value = accessIntPtr(indexes[i]) ;
		(*value) = i;
		printf("value: %d\n", *value) ;
		unlockMemory(indexes[i]) ;
	}
	// for(i = 0 ; i < 1000 ; i++)
	// {
	// 	freeMemory(indexes[i]) ;
	// }
}

int main()
{
	
	int i ;
	int a = 0 ;
	init() ;
	memoryMaxer();
	for(i = 0 ; i < 25 ; i++)
	{
		printf("%d ", ram[i]) ;
	}
	printf("\n\n") ;
	for(i = 0 ; i < 100 ; i++)
	{
		printf("%d ", ssd[i]) ;
	}
	printf("\n\n") ;
	for(i = 0 ; i < 1000 ; i++)
	{
		printf("%d ", hd[i]) ;	
		if(hd[i] == 0)
		{
			a += 1 ;
		}
	}
	printf("\n\n") ;
	printf("A COUNTER: %d\n", a) ;

	// test the pageFaultHandler
	// this time, we want to access pages that are not only in RAM
	int *value ;
	for(i = 0 ; i < 1000 ; i++)
	{
		value = accessIntPtr(indexes[i]) ;
		(*value) = i;
		printf("value: %d\n", *value) ;
		unlockMemory(indexes[i]) ;
	}


	/*printf("\nANOTHER ROUND\n") ;
	a=0;
	for (i = 0; i < 1000; ++i) {
	freeMemory(indexes[i]);
	}
	memoryMaxer() ;
	for(i = 0 ; i < 25 ; i++)
	{
		printf("%d ", mem[RAM][i]) ;
	}
	printf("\n\n") ;
	for(i = 0 ; i < 100 ; i++)
	{
	  printf("%d ", mem[SSD][i]) ;
	}
	printf("\n\n") ;
	for(i = 0 ; i < 1000 ; i++)
	{
		printf("%d ", mem[HD][i]) ;
		if(hd[i] == 0)
		{
			a += 1 ;
		}
	}
	printf("\n\n") ;
	printf("A COUNTER: %d\n", a) ;*/
	return 0 ;	
}

