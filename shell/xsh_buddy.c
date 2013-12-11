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
	int j;
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

// Testing multiple different types of objects being added and then freed and variable places to test external fragmentation
void test1()
{
	printf( "START TEST 1\n");

	int TEST_AMOUNT = 100;
	
	long longArr[ 100 ];
	int intArr[ 100 ];
	int i,j = 0;
	
	ulong startBuddy, startSlab, startLinkedList;
	ulong endBuddy, endSlab, endLinkedList;
	ulong totalBuddy = 0, totalSlab = 0, totalLinkedList = 0;
	struct MemFragFloat buddyFrag, slabFrag, linkedListFrag;
	uint buddyFail = 0, slabFail = 0, linkedListFail = 0;
	
	for( j = 0; j < TEST_AMOUNT; ++j )
	{
		clkticks = 0;
		startBuddy = clkticks;
		buddyMalloc( 10240 );
		// Initializing
		for( i = 0; i < 100; ++i )
		{
			void* object = buddyMalloc( sizeof( struct testStruct ) );
			longArr[ i ] = buddyMalloc( sizeof(long long) );
			void* object2 = buddyMalloc( sizeof(struct testStruct2) );
			intArr[ i ] = buddyMalloc( sizeof(int) );
			
			if( object == 0 ) 
			{
				buddyFail++;
			}
			if( longArr[ i ] == 0 )
			{
				buddyFail++;
			}
			if( object2 == 0 )
			{
				buddyFail++;
			}
			if( intArr[ i ] == 0 )
			{
				buddyFail++;
			}
		}
		
		for( i = 0; i < 100; ++i )
		{
			buddyFree( (void*)(longArr[ i ] ) );
			buddyFree( (void*)(intArr[ i ] ) );
		}
		
		endBuddy = clkticks;
		
		buddyFrag = buddyFragmentationAmount();
		
		startSlab = clkticks;
		slabInit();
		for( i = 0; i < 100; ++i )
		{
			void* object = slabAlloc( sizeof(struct testStruct) );
			longArr[ i ] = slabAlloc( sizeof(long long) );
			void* object2 = slabAlloc( sizeof(struct testStruct2) );
			intArr[ i ] = slabAlloc( sizeof(int) );
			if( object == 0 ) 
			{
				slabFail++;
			}
			if( longArr[ i ] == 0 )
			{
				slabFail++;
			}
			if( object2 == 0 )
			{
				slabFail++;
			}
			if( intArr[ i ] == 0 )
			{
				slabFail++;
			}
		}
		
		for( i = 0; i < 100; ++i )	
		{
			slabFree( (void*)(longArr[ i ] ) );
			slabFree( (void*)(intArr[ i ] ) );
		}
		endSlab = clkticks;
		
		slabFrag = calculateFragmentation();
			
		startLinkedList = clkticks;
		for(i = 0; i < 100; ++i)
		{
			void* object = linkedListMalloc( sizeof(struct testStruct) );
			longArr[ i ] = linkedListMalloc( sizeof(long) );
			void* object2 = linkedListMalloc( sizeof(struct testStruct2) );
			intArr[ i ] = linkedListMalloc( sizeof(int) );
			if( object == 0 ) 
			{
				linkedListFail++;
			}
			if( longArr[ i ] == 0 )
			{
				linkedListFail++;
			}
			if( object2 == 0 )
			{
				linkedListFail++;
			}
			if( intArr[ i ] == 0 )
			{
				linkedListFail++;
			}
		}		
		
		for( i = 0; i < 100; ++i )	
		{
			removeNode( (void*)(longArr[ i ] ) );
			removeNode( (void*)(intArr[ i ] ) );
		}

		endLinkedList = clkticks;
		
		// get fragmentation here
		buddyDealloc();
		slabCleanup();
		freeMemory();
		
		totalBuddy += (endBuddy - startBuddy);
		totalSlab += (endSlab - startSlab);
		//printf( "%d, %d, %d", endBuddy - startBuddy, endSlab - startSlab, endLinkedList - startLinkedList );
		totalLinkedList += (endLinkedList - startLinkedList);

	}
	
	totalBuddy = totalBuddy / TEST_AMOUNT;
	totalSlab = totalSlab / TEST_AMOUNT;
	totalLinkedList = totalLinkedList / TEST_AMOUNT;
	buddyFail = buddyFail / ( TEST_AMOUNT );
	slabFail = slabFail / ( TEST_AMOUNT );
	linkedListFail = linkedListFail / ( TEST_AMOUNT );
	printf( "Time Information\n" );
	printf( "Buddy: %d\tSlab:%d\tLinked List: %d\t\n", totalBuddy, totalSlab, totalLinkedList );
	printf( "Buddy Fail: %d\tSlab Fail:%d\tLinked List Fail: %d\t\n", buddyFail, slabFail, linkedListFail );
	printf( "Fragmentation\n" );
	printf( "Buddy Internal: %d%% Buddy External: %d%%\nSlab Internal: %d%% Slab External: %d%%\n", (int)(buddyFrag.intFragPercentage*100), (int)(buddyFrag.extFragPercentage*100),
		  (int)(slabFrag.intFragPercentage * 100), (int)(slabFrag.extFragPercentage * 100) );
	printf( "Size of Struct 1: %d\tSize of long: %d\tSize of Struct 2: %d\tSize of int: %d\n", sizeof(struct testStruct), sizeof(long long), sizeof(struct testStruct2), sizeof(int));
}

// Test 2 is meant to test a decreasing allocation size for each memory manager
void test2()
{
	printf( "START TEST 2\n");
	int TEST_AMOUNT = 100;
	
	long longArr[ 100 ];
	int intArr[ 100 ];
	int i,j = 0;
	
	ulong startBuddy, startSlab, startLinkedList;
	ulong endBuddy, endSlab, endLinkedList;
	ulong totalBuddy = 0, totalSlab = 0, totalLinkedList = 0;
	struct MemFragFloat buddyFrag, slabFrag, linkedListFrag;
	uint buddyFail = 0, slabFail = 0, linkedListFail = 0;
	
	for( j = 0; j < TEST_AMOUNT; ++j )
	{
		clkticks = 0;
		startBuddy = clkticks;
		buddyMalloc( 10240 );
		// Initializing
		for( i = 512; i > 0; --i )
		{
			void* object = buddyMalloc( i * 10 );
			
			if( object == 0 ) 
			{
				buddyFail++;
			}
		}
		endBuddy = clkticks;
		
		buddyFrag = buddyFragmentationAmount();
		
		startSlab = clkticks;
		slabInit();
		for( i = 512; i > 0; --i )
		{
			void* object = slabAlloc( i * 10 );
			if( object == 0 ) 
			{
				slabFail++;
			}
		}
		endSlab = clkticks;
		
		slabFrag = calculateFragmentation();
			
		startLinkedList = clkticks;
		for( i = 512; i > 0; --i )
		{
			void* object = linkedListMalloc( i  * 10 );
			
			if( object == 0 ) 
			{
				linkedListFail++;
			}
		}		
		
		endLinkedList = clkticks;
		
		// get fragmentation here
		buddyDealloc();
		slabCleanup();
		freeMemory();
		
		totalBuddy += (endBuddy - startBuddy);
		totalSlab += (endSlab - startSlab);
		//printf( "%d, %d, %d", endBuddy - startBuddy, endSlab - startSlab, endLinkedList - startLinkedList );
		totalLinkedList += (endLinkedList - startLinkedList);

	}
	
	totalBuddy = totalBuddy / TEST_AMOUNT;
	totalSlab = totalSlab / TEST_AMOUNT;
	totalLinkedList = totalLinkedList / TEST_AMOUNT;
	buddyFail = buddyFail / ( TEST_AMOUNT );
	slabFail = slabFail / ( TEST_AMOUNT );
	linkedListFail = linkedListFail / ( TEST_AMOUNT );
	printf( "Time Information\n" );
	printf( "Buddy: %d\tSlab:%d\tLinked List: %d\t\n", totalBuddy, totalSlab, totalLinkedList );
	printf( "Buddy Fail: %d\tSlab Fail:%d\tLinked List Fail: %d\t\n", buddyFail, slabFail, linkedListFail );
	printf( "Fragmentation\n" );
	printf( "Buddy Internal: %d%% Buddy External: %d%%\nSlab Internal: %d%% Slab External: %d%%\n", (int)(buddyFrag.intFragPercentage*100), (int)(buddyFrag.extFragPercentage*100),
		  (int)(slabFrag.intFragPercentage * 100), (int)(slabFrag.extFragPercentage * 100) );
}

// Test 3 is meant to 
void test3()
{
	printf( "START TEST 3\n");
	int TEST_AMOUNT = 100;
	
	long longArr[ 100 ];
	int intArr[ 100 ];
	int i,j = 0;
	
	ulong startBuddy, startSlab, startLinkedList;
	ulong endBuddy, endSlab, endLinkedList;
	ulong totalBuddy = 0, totalSlab = 0, totalLinkedList = 0;
	struct MemFragFloat buddyFrag, slabFrag, linkedListFrag;
	uint buddyFail = 0, slabFail = 0, linkedListFail = 0;
	
	for( j = 0; j < TEST_AMOUNT; ++j )
	{
		clkticks = 0;
		startBuddy = clkticks;
		buddyMalloc( 10240 );
		// Initializing
		for( i = 10; i > 0; --i )
		{
			void* object = buddyMalloc( i * 4 );
			
			if( object == 0 ) 
			{
				buddyFail++;
			}
		}
		endBuddy = clkticks;
		
		buddyFrag = buddyFragmentationAmount();
		
		startSlab = clkticks;
		slabInit();
		for( i = 10; i > 0; --i )
		{
			void* object = slabAlloc( i * 4 );
			if( object == 0 ) 
			{
				slabFail++;
			}
		}
		endSlab = clkticks;
		
		slabFrag = calculateFragmentation();
			
		startLinkedList = clkticks;
		for( i = 10; i > 0; --i )
		{
			void* object = linkedListMalloc( i  * 4 );
			
			if( object == 0 ) 
			{
				linkedListFail++;
			}
		}		
		
		endLinkedList = clkticks;
		
		// get fragmentation here
		buddyDealloc();
		slabCleanup();
		freeMemory();
		
		totalBuddy += (endBuddy - startBuddy);
		totalSlab += (endSlab - startSlab);
		//printf( "%d, %d, %d", endBuddy - startBuddy, endSlab - startSlab, endLinkedList - startLinkedList );
		totalLinkedList += (endLinkedList - startLinkedList);

	}
	
	totalBuddy = totalBuddy / TEST_AMOUNT;
	totalSlab = totalSlab / TEST_AMOUNT;
	totalLinkedList = totalLinkedList / TEST_AMOUNT;
	buddyFail = buddyFail / ( TEST_AMOUNT );
	slabFail = slabFail / ( TEST_AMOUNT );
	linkedListFail = linkedListFail / ( TEST_AMOUNT );
	printf( "Time Information\n" );
	printf( "Buddy: %d\tSlab:%d\tLinked List: %d\t\n", totalBuddy, totalSlab, totalLinkedList );
	printf( "Buddy Fail: %d\tSlab Fail:%d\tLinked List Fail: %d\t\n", buddyFail, slabFail, linkedListFail );
	printf( "Fragmentation\n" );
	printf( "Buddy Internal: %d%% Buddy External: %d%%\nSlab Internal: %d%% Slab External: %d%%\n", (int)(buddyFrag.intFragPercentage*100), (int)(buddyFrag.extFragPercentage*100),
		  (int)(slabFrag.intFragPercentage * 100), (int)(slabFrag.extFragPercentage * 100) );
}

shellcmd xsh_buddy(int nargs, char *args[])
{
	test1();
	test2();
	//test3();
	return 0;
}
