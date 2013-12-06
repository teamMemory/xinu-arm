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

shellcmd xsh_buddy(int nargs, char *args[])
{
	int i;
	linkedListMalloc(10);
	for(i = 0; i < 100; ++i)
	{
		
		if(!( linkedListMalloc(sizeof(int)) != NULL && linkedListMalloc(sizeof(char)) != NULL && linkedListMalloc(sizeof(short)) != NULL && linkedListMalloc(sizeof(long)) != NULL && linkedListMalloc(sizeof(bool)) != NULL ))
		{
			printf("Failed to allocate: %d", i);
			break;
		}
	}
	printf("Malloced Linked List Memory");
	return 0;
}

shellcmd xsh_buddy(int nargs, char *args[])
{
	int i;
	buddyMalloc(10);
	for( i = 0; i < 100; ++i )
	{
		if(!( slabMalloc(sizeof(int)) != NULL && slabMalloc(sizeof(char)) != NULL && slabMalloc(sizeof(short)) != NULL && slabMalloc(sizeof(long)) != NULL && slabMalloc(sizeof(bool)) != NULL ))
		{
			printf("Failed to allocate: %d", i);
			break;
		}
	}
	printf("Malloced memory!\n");
	return 0;
}
