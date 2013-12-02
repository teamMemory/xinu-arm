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
	buddyMalloc(10);
	slabMalloc(100, sizeof(int));
	printf("Malloced memory!\n");
	return 0;
}