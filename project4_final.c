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



typedef signed short vAddr;

struct page_table_entry
{
	vAddr page_number;	// page frame number (vAddr address), 0 to 1000
	int data;		// data is only single interger
	int valid;		// valid bit, make sure valid bits are zero at start 
	int referenced;
	int modified;
	int lock;		// lock for current user
	int memory;		// where is the page, ram ssd or hd

}page_table[HD_SIZE] ;

int ram[RAM_SIZE];
int ssd[SSD_SIZE];
int hard_disk[HD_SIZE];


void init()
{	
	int i ; 
	for(i = 0 ; i < 1000 ; i ++)
	{
		page_table[i].page_number = i ;
		page_table[i].data = i ;
		page_table[i].valid = -1 ;
		page_table[i].referenced = 0 ;
		page_table[i].modified = 0 ;
		page_table[i].lock = 0 ;
		page_table[i].memory = -1 ;
	}
	for(i = 0 ; i < HD_SIZE ; i ++)
	{
		hard_disk[i] = -1 ;
	}
	for(i = 0 ; i < SSD_SIZE ; i ++)
	{
		ssd[i] = -1 ;
	}
	for(i = 0 ; i < RAM_SIZE ; i ++)
	{
		ram[i] = -1 ;
	}
}

int RandomEntry(int size)
{
	srand(time(NULL)) ;
	if(size == RAM_SIZE)
	{
		return (rand() % 24) ;
	}
	else if(size == SSD_SIZE)
	{
		return (rand() % 99) ;
	}
	else if(size == HD_SIZE)
	{
		return (rand() % 999) ;
	}

}

int findFREE(int size) // Find a free slot in any array ie ram ssd or hd
{
	switch(size)
	{
		case RAM_SIZE :
		{
			int i ;
			for(i = 0 ; i < RAM_SIZE ; i++)
			{
				if(ram[i] == -1)
				{
					return i ;
				}
			}
			return -1 ;
		}

		case SSD_SIZE :
		{
			int i ;
			for(i = 0 ; i < SSD_SIZE ; i++)
			{
				if(ssd[i] == -1)
				{
					return i ;
				}
			}
			return -1 ;
		}

		case HD_SIZE :
		{
			int i ;
			for(i = 0 ; i < HD_SIZE ; i++)
			{
				if(hard_disk[i] == -1)
				{
					return i ;
				}
			}
			return -1 ;
		}
	}
	return -1 ;
}

void clock(int data, int size)
{
	int i ;
	int j ;
	int space ;
	if(size == RAM_SIZE)
	{
		for(i = 0 ; i < RAM_SIZE ; i++)
		{
			if(page_table[ram[i]].lock == 0 && page_table[ram[i]].referenced == 0)
			{
				int r = ram[i] ;
				space = findFREE(SSD_SIZE) ; //find free space in ssd. if -1, no free space
				if(space == -1)
					{
						clock(r, SSD_SIZE) ;
					}
					else
					{
						ssd[space] = r ;
					}
				page_table[r].memory = 1 ; //more page table stuff. IDK what needs to be done to it
				ram[i] = data ;
				break ;
			}
			else if(page_table[ram[i]].lock == 0 && page_table[ram[i]].referenced == 1)
			{
				page_table[ram[i]].referenced = 0 ;
			}
		}
	}
	else if(size == SSD_SIZE)
	{
		for(j = 0 ; j < SSD_SIZE ; j++)
		{
			if(page_table[ssd[j]].lock == 0 && page_table[ssd[j]].referenced == 0)
			{
				int s = ssd[j] ;
				space = findFREE(HD_SIZE) ;
				hard_disk[space] = s ;
				page_table[s].memory = 2 ;
				ssd[j] = data ;
				break ;
			}
			else if(page_table[ssd[j]].lock == 0 && page_table[ssd[j]].referenced == 1)
			{
				page_table[ssd[j]].referenced = 0 ;
			}
		}
	}
}


void random(int data, int size)
{
	int i ;

	if(size == RAM_SIZE)
	{
		int space ;
		int r = RandomEntry(RAM_SIZE) ;
		while(page_table[ram[r]].lock == 1)
		{
			r = RandomEntry(RAM_SIZE) ;
		}
		space = findFREE(SSD_SIZE) ;
		if(space == -1)
		{
			random(ram[r], SSD_SIZE) ;
		}
		else
		{
			ssd[space] = ram[r] ;
		}
		page_table[ram[r]].memory = 1 ;
		ram[r] = data ;
	}
	else if(size == SSD_SIZE)
	{
		int space ;
		int r = RandomEntry(SSD_SIZE) ;
		while(page_table[ram[r]].lock == 1)
		{
			r = RandomEntry(SSD_SIZE) ;
		}
		space = findFREE(HD_SIZE) ;
		hard_disk[space] = ssd[r] ;
		page_table[ssd[r]].memory = 2 ;
		ssd[r] = data ;
	}
}

void pageFaultHandler()
{
	return ;
}

vAddr allocateNewInt()
{

}

int * accessIntPtr(vAddr address)
{
	return 0 ;
}

void unlockMemory(vAddr address)
{
	return ;
}

void freeMemory(vAddr address)
{
	return ;
}

int main()
{
	init() ;

	printf("BLAH: %d %d %d %d %d", ram[16], ram[24], ssd[24], hard_disk[300], hard_disk[999]) ;
	return 0 ;
}
