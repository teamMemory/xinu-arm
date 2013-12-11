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

// Testing multiple different types of objects being added and then freed and variable places to test external fragmentation
void test1()
{
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
			longArr[ i ] = buddyMalloc( sizeof(long) );
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
		
		startSlab = clkticks;
		slabInit();
		for( i = 0; i < 100; ++i )
		{
			void* object = slabAlloc( sizeof(struct testStruct) );
			longArr[ i ] = slabAlloc( sizeof(long) );
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
}

shellcmd xsh_buddy(int nargs, char *args[])
{
	test1();

	return 0;
}
