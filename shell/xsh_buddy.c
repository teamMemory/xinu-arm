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

shellcmd xsh_buddy(int nargs, char *args[])
{
	int i;
	buddyMalloc(1024);
	for( i = 0; i < 100; ++i )
	{
		buddyMalloc(i * 3);
	}
	//for( i = 0; i < 100; ++i )
	//{
	//	if(!( slabMalloc(sizeof(int)) != NULL && slabMalloc(sizeof(char)) != NULL && slabMalloc(sizeof(short)) != NULL && slabMalloc(sizeof(long)) != NULL && slabMalloc(sizeof(bool)) != NULL ))
	//	{
	//		printf("Failed to allocate: %d", i);
	//		break;
	//	}
	//}
	printf("Malloced memory!\n");
	//printMemUsage();
	
	return 0;
}
