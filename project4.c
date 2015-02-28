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
#include <pthread.h>

#define RAM_SIZE 	25
#define SSD_SIZE	100
#define HD_SIZE 	1000
#define NUM_THREADS	5
#define TRUE		1

typedef signed short vAddr ;
int page_count, ram_count, ssd_count, hd_count = 0 ;
int entriesCounter = 0 ;
int waitRAM = 0.0 * 1000000 ;
int waitSSD = 0.0 * 1000000 ;

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

void init();

sem_t lock ;

sem_t accessIntPtr_sem ;
sem_t allocateNewInt_sem ;
sem_t unlockMemory_sem ;
sem_t freeMemory_sem ;

pthread_t threads[NUM_THREADS];
int thread_num[NUM_THREADS];


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

void randomSSD()//random eviction algorithm for SSD
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


vAddr allocateNewInt_unprotected()
{
	//sem_wait(&allocateNewInt_sem);
	int freeSpace = findFREE(RAM_SIZE);
	srand(time(NULL)) ;

	if(page_count == 1000)
	{
		printf("NOT ENOUGH MEMORY\n") ;
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

				
 				break ;
			}
		}
		//sem_post(&allocateNewInt_sem);
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

 				break ;
			}
		}
		//sem_post(&allocateNewInt_sem);
		return i ;
	}
}

vAddr allocateNewInt()
{
sem_wait(&allocateNewInt_sem);
vAddr tmp = allocateNewInt_unprotected();
sem_post(&allocateNewInt_sem);
return tmp;
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

int * accessIntPtr_unprotected(vAddr address);

int * accessIntPtr(vAddr address);

void unlockMemory(vAddr address)
{
	sem_wait(&unlockMemory_sem);
	page_table[address].lock = 0 ;
	sem_post(&unlockMemory_sem);
}

void freeMemory(vAddr address)
{
	sem_wait(&freeMemory_sem);
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
	sem_post(&freeMemory_sem);
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
	int *value ;
	vAddr indexes[1001] ;
	for(i = 0 ; i < 10 ; i++)
	{
		value = accessIntPtr(999 - (i + 24)) ;
		//*value = i ;
		printf("value: %d\n", *value) ;
		unlockMemory(999 - (i +24)) ;
	}
}

void tooMany()//asks for too much pages
{
	int i ;
	int *value ;
	vAddr indexes[1001] ;
	for(i = 0 ; i < 1001 ; i++)
	{
		indexes[i] = allocateNewInt() ;
		if(value != NULL){
		*value = i ;
		printf("value: %d\n", *value) ;
		unlockMemory(indexes[i]) ;
		}else printf("Cant allocate more, no memory\n");
	}
}

void *job_thread(void *arg){

	int i ;
	int *value ;
	vAddr indexes[1000] ;	
	int * thread_num = (int *) arg;

	while(TRUE){


		for(i = 0 ; i < 1000 ; i++)
		{
			sem_wait(&lock);
			printf("Thread: %d, opening vAddr: %d\n",(*thread_num),i);
			value = accessIntPtr(indexes[i]) ;
			if(value!=NULL)
			{
				*value = i ;
				printf("value: %d\n", *value) ;
			}else
			{
				unlockMemory(indexes[i]) ;
				printf("Returns NULL\n");
			}
			printf("Thread: %d, closing vAddr: %d\n",(*thread_num),i);
			sem_post(&lock);
		}

	}
	return 0;

}

void dump()
{
int i=0,a=0;
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
	printf("\nA COUNTER: %d\n", a) ;
}

int main()
{	
	int i ;
	int x ;
	int a = 0 ;

	// --------------------- initialize  queue semaphores, initialy they are unblocked
	if(sem_init(&accessIntPtr_sem, 0, 1) || sem_init(&allocateNewInt_sem, 0, 1) || sem_init(&unlockMemory_sem, 0, 1) || sem_init(&freeMemory_sem, 0, 1)|| sem_init(&lock, 0, 1))
	{
		printf("Couldn't initialize semaphore\n");
		return ;
	}


	for(i = 0 ; i < 1000 ; i++)
	{
		page_table[i].virtual_address = i ;
		page_table[i].physical_address = -1 ;
		page_table[i].referenced = 0;
		page_table[i].lock = 0 ;
		page_table[i].memory_location = -1 ;
		page_table[i].valid = 0 ;
	}

	memoryMaxer() ;

	int *value ;
	for(i = 999 ; i >= 0  ; i--)
	{
		value = accessIntPtr(i) ;
		if(value!=NULL)
		{
			*value = i ;
			printf("value: %d\n", *value) ;
			unlockMemory(i) ;
		}else printf("Returns NULL\n");
	}
	
	tooMany();	
	dump();	
	accessPages();

	
	//init() ;

	/*int *value ;
	vAddr indexes[1000] ;
	value = accessIntPtr(indexes[i]) ;
	if(value!=NULL)
	{
		*value = i ;
		printf("value: %d\n", *value) ;

	}else
	{
		unlockMemory(indexes[i]) ;
		printf("Returns NULL\n");
	}*/


	return 0 ;
}

void init()//creates and releases 5 evil threads
{
	int i,error ;

	for(i=0;i<NUM_THREADS;++i)
		thread_num[i]=i;// gives id to 5 evil threads

	for (i = 0; i < NUM_THREADS; ++i)
	{	//give birth to 5 evil sonofabitching threads
		if ((error = pthread_create(&threads[i], NULL, job_thread, &thread_num[i]))) {
		printf("error: pthread_create: %d\n", error);
		return ;}
	}
}

int * accessIntPtr(vAddr address)//protected with semaphore versio
{
sem_wait(&accessIntPtr_sem);
int * tmp = accessIntPtr_unprotected(address);
sem_post(&accessIntPtr_sem);
return tmp;
}

int * accessIntPtr_unprotected(vAddr address)//protected with semaphore versio
{	
	//sem_wait(&accessIntPtr_sem);
	if(page_table[address].valid == 0)
	{
		printf("DOESNT EXIST\n") ;
		//sem_post(&accessIntPtr_sem);
		return NULL;
	}else{
	int j = page_table[address].physical_address ;
	int i = page_table[address].memory_location ;
	page_table[address].referenced = 1 ;
	page_table[address].lock = 1 ;

	if(i == 0)
	{
		//sem_post(&accessIntPtr_sem);
		return &ram[j] ;
	}
	else if(i == 1)//SSD
	{
		pageFaultHandler(address) ;
		accessIntPtr_unprotected(address) ;
	}
	else if(i == 2)//HD
	{
		pageFaultHandler(address) ;
		accessIntPtr_unprotected(address) ;
	}
	}	
	
}

