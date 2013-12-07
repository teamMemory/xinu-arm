/**
 * @file     xsh_buddy.c
 * @provides xsh_buddy.
 * @author Blake Gross
 */

#include <stddef.h>
#include <buddymemory.h>
//#include <slabmemory.h>
#include <stdio.h>
#include <string.h>
#include <memstat.h>
#include <clock.h>

shellcmd xsh_buddy(int nargs, char *args[])
{
	int i;
	
	ulong startTime = clktime;
	buddyMalloc(1024);
	for( i = 0; i < 10000000000; ++i )
	{
		if( buddyMalloc(5) == 0 )
		{
			printf( "Buddy Malloc failed\n" );
			break;
		}
	}
	
	ulong endTime = clktime;
	//for( i = 0; i < 100; ++i )
	//{
	//	if(!( slabMalloc(sizeof(int)) != NULL && slabMalloc(sizeof(char)) != NULL && slabMalloc(sizeof(short)) != NULL && slabMalloc(sizeof(long)) != NULL && slabMalloc(sizeof(bool)) != NULL ))
	//	{
	//		printf("Failed to allocate: %d", i);
	//		break;
	//	}
	//}
	printf("Malloced memory in time of %d, %d\n", startTime, endTime);
	
	printMemUsage();
	
	buddyDealloc();
	
	printf("Dealloced memory\n");
	
	printMemUsage();
	return 0;
}
