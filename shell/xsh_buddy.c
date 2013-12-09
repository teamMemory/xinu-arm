/**
 * @file     xsh_buddy.c
 * @provides xsh_buddy.
 * @author Blake Gross
 */

#include <stddef.h>
#include <buddymemory.h>
#include <slabmemory.h>
#include <linkedListMalloc.h>
#include <stdio.h>
#include <string.h>
#include <memstat.h>
#include <clock.h>

struct testStruct
{
	int a;
	double b;
	double c;
	double d;
	double e;
	double f;
	char g;
	char h;
	int i;
};

struct testStruct2
{
	int a;
	double b;
	double c;
	double d;
	double e;
	double f;
	char g;
	char h;
};

shellcmd xsh_buddy(int nargs, char *args[])
{
	int i;
	struct MemFrag frag;
	ulong startTime = clkticks;
	
	// initialization for buddy Malloc
	printf( "---------------------Running Buddy Malloc Test----------------\n");
	
	buddyMalloc(1024);
	for( i = 0; i < 100; ++i )
	{
		if( buddyMalloc(5) == 0 )
		{
			printf( "Buddy Malloc failed at: %d\n", i );
			break;
		}
	}
	
	ulong endTime = clkticks;
	endTime -= startTime;
	
	printMemUsage();
	printf("Malloced memory in time of %d, %d\n", endTime);
	
	
	printf( "---------------------Running SLAB Malloc Test----------------\n");
	slabInit();	// SLAB initialzation
	startTime = clkticks;
	long longArr[ 20 ];
	long intArr[ 20 ];
	for( i = 0; i < 20; ++i )
	{
		printf("At malloc pos %d: ", i );
		void* object = slabAlloc( sizeof(struct testStruct) );
		longArr[ i ] = slabAlloc( sizeof(long) );
		void* object2 = slabAlloc( sizeof(struct testStruct2) );
		intArr[ i ] = slabAlloc( sizeof(int) );
		if( object == 0  )
		{
			printf( "Slab Malloc failed\n" );
			break;
		}
	}
	for( i = 0; i < 20; ++i )	// should cause external fragmentation
	{
		slabFree( (void*)(longArr[ i ] ) );
		slabFree( (void*)(intArr[ i ] ) );
	}
	
	printMemUsage();
	struct MemFragFloat fragmentationAmount = calculateFragmentation();
	int externalFragmentation = fragmentationAmount.extFragPercentage * 100;
	printf("External Fragmentation: %d%%, Internal Fragmentation: %3.2f \n", externalFragmentation, fragmentationAmount.intFragPercentage );
	endTime = clkticks;
	
	endTime-=startTime;
	printf("Malloced memory in time of %d, %d\n", endTime);
		

	printMemUsage();
	buddyDealloc();
	
	slabCleanup();
	printf("Dealloced memory\n");
	
	printMemUsage();

	
	printf( "---------------------Running Linked List Malloc Test----------------\n");
	////////////////////////////////////////////
	///// Linked List Test
	////////////////////////////////////////////
    
	int k;
	for(k=1; k < 100; k++)
	{
		void * loc;
		//printf("Allocating memory\n");
		loc = linkedListMalloc(k);
		if(loc == NULL){
			printf("%d failed\n",k);
		}
		//printf("Memory allocated\n");
		//printf("Deallocating memory\n");
		removeNode(loc);
		//printf("Memory deallocated\n");
		//printf("Freeing memory\n");
		freeMemory();
		//printf("MemoryFreed\n");
	}	
	printFrag();

	return 0;
}
