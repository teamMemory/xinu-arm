/**
 * @file buddyAllocater.c
 * @author Blake Gross
 * Manages the memory that is allocated
 *
 */

#include <stddef.h> 
#include <buddymemory.h>
#include <stdlib.h>

struct buddynode* rootNode;

void buddyInit()
{
	// To be implemented fully. Temporary allocation made
	// For now allocating new memory for each node. We should fix in the future
	rootNode = (struct buddynode*)malloc(sizeof(struct buddynode));
	rootNode->memRegion = malloc( BUDDY_PAGE_SIZE );
	rootNode->isFull = FALSE;
}

void* buddyAlloc(uint numBytes)
{
	if( rootNode == NULL )
	{
		buddyInit();
	}
	
	// for now just returning rootnodes memory region
	return rootNode->memRegion;
}