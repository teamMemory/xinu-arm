/**
 * @file     xsh_buddy.c
 * @provides xsh_buddy.
 * @author Blake Gross
 */

#include <stddef.h>
#include <buddymemory.h>
#include <slabmemory.h>
#include <stdio.h>
#include <string.h>
#include <memstat.h>
#include <clock.h>

shellcmd xsh_buddy(int nargs, char *args[])
{
	int i;
	
	ulong startTime = clkticks;
	buddyMalloc(1024);
	for( i = 0; i < 100; ++i )
	{
		if( buddyMalloc(5) == 0 )
		{
			printf( "Buddy Malloc failed\n" );
			break;
		}
	}
	
	ulong endTime = clkticks;
	endTime -= startTime;
	
	printMemUsage();
	printf("Malloced memory in time of %d, %d\n", endTime);
	
	slabInit();
	startTime = clkticks;
	for( i = 0; i < 100; ++i )
	{
		printf("At malloc pos %d: ", i );
		if( slabMalloc( sizeof( int ) ) == 0  )
		{
			printf( "Slab Malloc failed\n" );
		}
	}
	printMemUsage();
	endTime = clkticks;
	
	endTime-=startTime;
	printf("Malloced memory in time of %d, %d\n", endTime);
		

		printMemUsage();
	buddyDealloc();
	
	printf("Dealloced memory\n");
	
	printMemUsage();
	return 0;
}
