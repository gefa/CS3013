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

typedef signed short vAddr ;
int page_count, ram_count, ssd_count, hd_count = 0 ;
int entriesCounter = 0 ;
int waitRAM = 0.025 * 1000000 ;
int waitSSD = 0.025 * 1000000 ;

struct page_table_entry
{
	vAddr virtual_address ;
	int physical_address ;
	int referenced ;
	int lock ;
	int memory_location ;
	int valid ;
}page_table[1000] ;

int ram[RAM_SIZE] ;
int ssd[SSD_SIZE] ;
int hd[HD_SIZE] ;

pthread_mutex_t accessIntPtr_lock ;
pthread_mutex_t allocateNewInt_lock ;
pthread_mutex_t unlockMemory_lock ;
pthread_mutex_t freeMemory_lock ;

pthread_t threads[NUM_THREADS];

void init()
{
	int i ;
	for(i = 0 ; i < 1000 ; i++)
	{
		page_table[i].virtual_address = i ;
		page_table[i].physical_address = -1 ;
		page_table[i].referenced = 0;
		page_table[i].lock = 0 ;
		page_table[i].memory_location = -1 ;
		page_table[i].valid = 0 ;
	}
}


// int findFREE(int size) // Find a free slot in any array ie ram ssd or hd
// {
// 	int i ;
// 	if(size == RAM_SIZE)
// 	{
// 		for(i = 0 ; i < RAM_SIZE ; i++)
// 		{
// 			if(ram[i] == -1)
// 			{
// 				return i ;
// 			}
// 		}
// 		return -1 ;
// 	}

// 	else if(size == SSD_SIZE)
// 	{
// 		for(i = 0 ; i < SSD_SIZE ; i++)
// 		{
// 			if(ssd[i] == -1)
// 			{
// 				return i ;
// 			}
// 		}
// 		return -1 ;
// 	}

// 	else if(size == HD_SIZE)
// 	{
// 		for(i = 0 ; i < 1000 ; i++)
// 		{
// 			if(hd[i] == -1)
// 			{
// 				return i ;
// 			}
// 		}
// 		return -1 ;
// 	}
// }

int findFREE(int size) // Find a free slot in any array ie ram ssd or hd
{

	int i ;
	if(size == RAM_SIZE)
	{
		int freeArray1[RAM_SIZE];
		for(i=0;i<RAM_SIZE;i++)
		{
			freeArray1[i]=1 ;//assume all free
		}

		for(i=0;i<1000;i++)
		{
			if(page_table[i].memory_location == 0)
			{
				freeArray1[page_table[i].physical_address]=0;//mark the used ram pages
			}
		}

		for(i=0;i<RAM_SIZE;i++)
		{
			if(freeArray1[i] == 1)
			{//find the first free spot in ram 
				return i ;
			}
		}

		return -1;
	}

	else if(size == SSD_SIZE)
	{
		int freeArray2[SSD_SIZE];
		for(i=0;i<SSD_SIZE;++i)
			freeArray2[i]=1;//assume all free

		for(i=0;i<HD_SIZE;++i)
		{
			if(page_table[i].memory_location == 1)
				freeArray2[page_table[i].physical_address-25]=0;//mark the used ram pages
		}

		for(i=0;i<SSD_SIZE;++i)
			if(freeArray2[i] == 1)//find the first free spot in SSD 
				return i ;
		return -1 ;
	}

	else if(size == HD_SIZE)
	{
		int freeArray3[HD_SIZE];
		for(i=0;i<HD_SIZE;++i)
			freeArray3[i]=1;//assume all free

		for(i=0;i<HD_SIZE;++i)
		{
			if(page_table[i].memory_location == 2)
				freeArray3[page_table[i].physical_address-125]=0;//mark the used ram pages
		}

		for(i=0;i<HD_SIZE;++i)
			if(freeArray3[i] == 1)//find the first free spot in HD 
				return i ;
		return -1 ;
	}
}

void randomSSD()
{
	int i ;
	int j = rand() % 100 ;
	int freeSpace = findFREE(HD_SIZE) ;

	if(freeSpace != -1)
	{
		for(i = 0 ; i < 1000 ; i++)
		{
			if((page_table[i].physical_address - 25) == j)
			{
				if(page_table[i].lock == 0)
				{
					int a = ssd[j] ;
					ssd[j] = -1 ;
					hd[freeSpace] = a ;
					page_table[i].physical_address = freeSpace + 125 ;
					page_table[i].memory_location = 2 ;
					page_table[i].lock = 0 ;
					usleep(waitSSD) ;
					return ;
				}
				else if(page_table[i].lock == 1)
				{
					i = 0 ;
					j = rand() % 100 ;
				}
			}
		}
	}
	else if(freeSpace == -1)
	{
		return ;
	}
}

void randomRAM()
{
	srand(time(NULL)) ;
	int i ;
	int j = rand() % 25 ;
	int freeSpace = findFREE(SSD_SIZE) ;

	if(freeSpace != -1)
	{
		for(i = 0 ; i < 1000 ; i++)
		{
			if(page_table[i].physical_address == j)
			{
				if(page_table[i].lock == 0)
				{
					int a = ram[j] ;
					ram[j] = -1 ;
					ssd[freeSpace] = a ;
					page_table[i].physical_address = freeSpace + 25 ;
					page_table[i].memory_location = 1 ;
					page_table[i].lock = 0 ;
					usleep(waitRAM) ;
					return ;
				}
				else if(page_table[i].lock == 1)
				{
					i = 0 ;
					j = rand() % 25 ;
				}
			}
		}
	}
	else if(freeSpace == -1)
	{
		randomSSD() ;
		freeSpace = findFREE(SSD_SIZE) ;
		for(i = 0 ; i < 1000 ; i++)
		{
			if(page_table[i].physical_address == j)
			{
				if(page_table[i].lock == 0)
				{
					int a = ram[j] ;
					ram[j] = -1 ;
					ssd[freeSpace] = a ;
					page_table[i].physical_address = freeSpace + 25 ;
					page_table[i].memory_location = 1 ;
					page_table[i].lock = 0 ;
					usleep(waitRAM) ;
					return ;
				}
				else if(page_table[i].lock == 1)
				{
					i = 0 ;
					j = rand() % 25 ;
				}
			}
		}
	}
}


void clockSSD()
{
	int i ;
	int j ;
	int freeSpace = findFREE(HD_SIZE) ;

	if(freeSpace != -1)
	{
		for(j = 0 ; j < 2 ; j++)
		{
			for(i = 0 ; i < 1000 ; i++)
			{
				if(page_table[i].memory_location == 1 && page_table[i].lock == 0 && page_table[i].referenced == 0)
				{
					int a = ssd[page_table[i].physical_address - 25] ;
					hd[freeSpace] = a ;
					ssd[page_table[i].physical_address - 25] = -1 ;
					page_table[i].physical_address = freeSpace + 125 ;
					page_table[i].memory_location = 2 ;
					page_table[i].lock = 0 ;
					usleep(waitSSD) ;
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
		return ;
	}
}



void clockRAM()
{
	int i ;
	int j ;
	int freeSpace = findFREE(SSD_SIZE) ;
	if(freeSpace != -1)
	{
		for(j = 0 ; j < 2 ; j++)
		{
			for(i = 0 ; i < 1000 ; i++)
			{
				if(page_table[i].memory_location == 0 && page_table[i].lock == 0 && page_table[i].referenced == 0)
				{
					int a = ram[page_table[i].physical_address] ;
					ssd[freeSpace] = a ;
					ram[page_table[i].physical_address] = -1 ;
					page_table[i].physical_address = freeSpace + 25 ;
					page_table[i].memory_location = 1 ;
					page_table[i].lock = 0 ;
					usleep(waitRAM) ;
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
		freeSpace = findFREE(SSD_SIZE) ;
		for(j = 0 ; j < 2 ; j++)
		{
			for(i = 0 ; i < 1000 ; i++)
			{
				if(page_table[i].memory_location == 0 && page_table[i].lock == 0 && page_table[i].referenced == 0)
				{
					int a = ram[page_table[i].physical_address] ;
					ssd[freeSpace] = a ;
					ram[page_table[i].physical_address] = -1 ;
					page_table[i].physical_address = freeSpace + 25 ;
					page_table[i].memory_location = 1 ;
					page_table[i].lock = 0 ;
					usleep(waitRAM) ;
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
	pthread_mutex_lock(&allocateNewInt_lock);
	int freeSpace = findFREE(RAM_SIZE);
	srand(time(NULL)) ;

	if(page_count == 1000)
	{
		printf("NOT ENOUGH MEMORY\n") ;
		return -666;
	}

	else if(freeSpace != -1)
	{
		int i ;
		for(i = 0 ; i < 1000 ; i++)
		{
			if(page_table[i].valid == 0 && page_table[i].physical_address == -1)
			{
				page_table[i].valid = 1 ;
				page_table[i].physical_address = freeSpace ;
				page_table[i].lock = 1 ;
				page_table[i].memory_location = 0 ;
				page_table[i].referenced = 0 ;
				page_count += 1 ;

				//ram[freeSpace] = entriesCounter ;
				//entriesCounter += 1 ;
 				break ;
			}
		}
		pthread_mutex_unlock(&allocateNewInt_lock);
		return i ;
	}

	else if(freeSpace == -1) 
	{
		int i ;
		clockRAM() ;
		freeSpace = findFREE(RAM_SIZE) ;
		for(i = 0 ; i < 1000 ; i++)
		{
			if(page_table[i].valid == 0 && page_table[i].physical_address == -1)
			{
				page_table[i].valid = 1 ;
				page_table[i].physical_address = freeSpace ;
				page_table[i].lock = 1 ;
				page_table[i].memory_location = 0 ;
				page_table[i].referenced = 0 ;
				page_count += 1 ;

				//ram[freeSpace] = entriesCounter ;
				//entriesCounter += 1 ;
 				break ;
			}
		}
		pthread_mutex_unlock(&allocateNewInt_lock);
		return i ;
	}
}

void pageFaultHandler(vAddr address)
{
	int i ;
	int j ;
	if(page_table[address].valid == 0) 
	{
		printf("BITCH YOU MESSED UP THAT PAGE DONT EXIST\n") ;
		return ;
	}
	else
	{
		if(page_table[address].memory_location == 1)
		{
			int freeSpace = findFREE(RAM_SIZE) ;
			if(freeSpace != -1)
			{
				ram[freeSpace] = ssd[page_table[address].physical_address - 25] ;
				ssd[page_table[address].physical_address - 25] = 0 ;
				page_table[address].memory_location = 0 ;
				page_table[address].physical_address = freeSpace ;
			}
			else if(freeSpace == -1)
			{
				clockRAM() ;
				freeSpace = findFREE(RAM_SIZE) ;
				
				ram[freeSpace] = ssd[page_table[address].physical_address - 25] ;
				ssd[page_table[address].physical_address - 25] = 0 ;
				page_table[address].memory_location = 0 ;
				page_table[address].physical_address = freeSpace ;

			}
		}
		else if(page_table[address].memory_location == 2)
		{
			int freeSpace = findFREE(SSD_SIZE) ;
			if(freeSpace != -1)
			{
				ssd[freeSpace] = hd[page_table[address].physical_address - 125] ;
				hd[page_table[address].physical_address - 125] = 0 ;
				page_table[address].memory_location = 1 ;
				page_table[address].physical_address = freeSpace + 25 ;
				pageFaultHandler(address) ;
			}
			else if(freeSpace == -1)
			{
				clockSSD() ;
				freeSpace = findFREE(SSD_SIZE) ;

				ssd[freeSpace] = hd[page_table[address].physical_address - 125] ;
				hd[page_table[address].physical_address - 125] = 0 ;
				page_table[address].memory_location = 1 ;
				page_table[address].physical_address = freeSpace + 25 ;
				pageFaultHandler(address) ;
			}
		}
	}
}

int * accessIntPtr(vAddr address)
{
	pthread_mutex_lock(&accessIntPtr_lock);
	if(page_table[address].valid == 0)
	{
		printf("DOESNT EXIST\n") ;
		pthread_mutex_unlock(&accessIntPtr_lock);
		return NULL ;
	}
	int j = page_table[address].physical_address ;
	int i = page_table[address].memory_location ;
	page_table[address].referenced = 1 ;
	page_table[address].lock = 1 ;
	
	if(i == 0)
	{
		pthread_mutex_unlock(&accessIntPtr_lock);
		return &ram[j] ;
	}
	else if(i == 1)
	{
		pageFaultHandler(address) ;
		accessIntPtr(address) ;
	}
	else if(i == 2)
	{
		pageFaultHandler(address) ;
		accessIntPtr(address) ;
	}	
}


void unlockMemory(vAddr address)
{
	pthread_mutex_lock(&unlockMemory_lock);
	page_table[address].lock = 0 ;
	pthread_mutex_unlock(&unlockMemory_lock);
}

void freeMemory(vAddr address)
{
	pthread_mutex_lock(&freeMemory_lock);
	int j = page_table[address].physical_address ;
	int i = page_table[address].memory_location ;
	if(i == 0)
	{
		ram[j] = 0 ;
	}
	else if(i == 1)
	{
		ssd[j - 25] = 0 ;
	}
	else if(i == 2)
	{
		hd[j - 125] = 0 ;
	}	

	page_table[address].physical_address = -1 ;
	page_table[address].valid = 0 ;
	page_table[address].lock = 0 ;
	page_table[address].memory_location = -1 ;
	page_table[address].referenced = 0 ;
	pthread_mutex_unlock(&freeMemory_lock);
}

void memoryMaxer()
{
	int i ;
	int *value ;
	vAddr indexes[1000] ;
	for(i = 0 ; i < 1000 ; i++)
	{
		indexes[i] = allocateNewInt() ;
		value = accessIntPtr(indexes[i]) ;
		*value = i ;
		printf("value: %d\n", *value) ;
		unlockMemory(indexes[i]) ;
	}
	// for(i = 0 ; i < 1000 ; i++)
	// {
	// 	freeMemory(indexes[i]) ;
	// }
}

void memoryMaxFree()
{
	int i ;
	int *value ;
	vAddr indexes[1000] ;
	for(i = 0 ; i < 1000 ; i++)
	{
		indexes[i] = allocateNewInt() ;
		value = accessIntPtr(indexes[i]) ;
		*value = i ;
		printf("value: %d\n", *value) ;
		unlockMemory(indexes[i]) ;
	}
	for(i = 0 ; i < 1000 ; i++)
	{
		freeMemory(indexes[i]) ;
	}
	
	accessIntPtr(indexes[20]) ;
		
}

void accessPages() //goes through and picks a few pages to access, will cause evictions if necessary
{
	int i ;
	for(i = 0 ; i < 10 ; i++)
	{
		value = accessIntPtr(indexes[999 - (i + 24)]) ;
		*value = i ;
		printf("value: %d\n", *value) ;
		unlockMemory(indexes[999 - (i +24)]) ;
	}
}

void tooMany()
{
	int i ;
	int *value ;
	vAddr indexes[1001] ;
	for(i = 0 ; i < 1001 ; i++)
	{
		indexes[i] = allocateNewInt() ;
		*value = i ;
		printf("value: %d\n", *value) ;
		unlockMemory(indexes[i]) ;
	}
}

int main()
{
	int i ;
	vAddr c = 874 ;
	int x ;
	int a = 0 ;
	init() ;
	memoryMaxer() ;

	for(i = 0 ; i < 25 ; i++)
	{
		printf("%d\n", ram[i]) ;
	}

	for(i = 0 ; i < 100 ; i++)
	{
		printf("%d\n", ssd[i]) ;
	}

	for(i = 0 ; i < 1000 ; i++)
	{
		printf("%d\n", hd[i]) ;
		if(hd[i] == 0)
		{
			a += 1 ;
		}
	}
	printf("A COUNTER: %d", a) ;
	return 0 ;	
}

