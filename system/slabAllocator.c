/**
 * @file slabAllocater.c
 * @author Artur Braga; Blake Gross
 * Manages the memory that is allocated
 *
 */

#include <slabmemory.h>
#include <stdlib.h>


struct SlabCacheList *cacheHead;
void* memBlock;

void slabInit()
{
	memBlock = malloc( TOTAL_MEMORY );
}

void* slabMalloc(uint elSize)
{
	int i,j;
	struct SlabCacheList* cacheToUse = cacheHead;
	while( cacheHead != NULL )
	{
		if( cacheToUse->objSize == elSize )
		{
			break;
		}
		cacheHead = cacheHead->pNext;
	}
	
	if( cacheToUse == NULL )
	{
		cacheToUse = createCache(elSize);
	}
	
	struct Slab* currentSlab = cacheHead->pSlabList;	// need to get way of getting this....
	
	
	void* memoryToReturn = NULL;
	while( currentSlab != NULL )
	{
		if( currentSlab-> NbFree > 0 )	// if the slab has free space, we can allocate in it
		{
			struct BufferList* headBuffer = currentSlab->pFree;
			memoryToReturn = headBuffer->pObject;		// first object on list is always going to be free, now we have to take if off the list (works like a queue)

			if( headBuffer->pNext != NULL )
			{
				headBuffer->pNext->pPrev = headBuffer->pPrev;	// reordering the list
				headBuffer->pNext = NULL;
				headBuffer->pPrev = NULL;
			}
			currentSlab->pFree = headBuffer->pNext;
			
			--NbFree;
			break;
		}
		
		currentSlab = currentSlab->pNext;
	}

	// for now just returning rootnodes memory region
	//return rootNode->memRegion;
	return memoryToReturn;
}

struct SlabCacheList *createCache(uint size)
{
	if( memBlock == NULL )
	{
		slabInit();
	}
}