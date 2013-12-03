/**
 * @file slabAllocater.c
 * @author Artur BRAGA
 * Manages the memory that is allocated
 *
 */

#include <slabmemory.h>
#include <stdlib.h>


struct dataType *dataCache;

void slabInit(uint numEl)
{
	dataCache = malloc(6 * sizeof(struct dataType));
	dataCache[0].size=sizeof(uchar);
	dataCache[0].memList=malloc(sizeof(struct dataNode));
	dataCache[0].memList->mem=malloc(numEl*(sizeof(uchar)+1));
	dataCache[1].size=sizeof(ushort);
	dataCache[1].memList=malloc(sizeof(struct dataNode));
	dataCache[1].memList->mem=malloc(numEl*(sizeof(ushort)+1));
	dataCache[2].size=sizeof(uint);
	dataCache[2].memList=malloc(sizeof(struct dataNode));
	dataCache[2].memList->mem=malloc(numEl*(sizeof(uint)+1));
	dataCache[3].size=sizeof(ulong);
	dataCache[3].memList=malloc(sizeof(struct dataNode));
	dataCache[3].memList->mem=malloc(numEl*(sizeof(ulong)+1));
	dataCache[4].size=sizeof(bool);
	dataCache[4].memList=malloc(sizeof(struct dataNode));
	dataCache[4].memList->mem=malloc(numEl*(sizeof(bool)+1));
}

void* slabMalloc(uint elSize)
{
	int i,j;
	if( dataCache == NULL )
	{
		slabInit(NUM_ELEMENTS);	
	}
	
	for( i = 0; i < NBDATASTR; ++i )		// for the amount of elements in the list...
	{
		if( dataCache[ i ].size == elSize)	// we look for the size that matches the requested allocation
		{
			void* tempMemory = NULL;
			struct dataNode *current = dataCache[i].memList;
			do								// We go through the linked list until we find a "page" that is available for use
			{
				if( !current->isTaken )
				{
					tempMemory = current->mem;
					break;
				}
				
				if( current->next != NULL )
				{
					current = current->next;
				}
				else
				{
					break;
				}
			}while( current->next != NULL );
			
			if( tempMemory != NULL )
			{
				for( j = 0; j < NUM_ELEMENTS; ++j )
				{
					bool* tempMemoryFull = (tempMemory + j * (elSize + 1) );
					if( *tempMemoryFull == FALSE )
					{
						*tempMemoryFull = TRUE;
						
						// RETURN ALLOCATED MEMORY
						return (void*)(&(*tempMemoryFull));
					}
				}
				
				current->isTaken = TRUE;	// went through whole "page" without finding anything
				--i;						// resetting i, so that it will go through the search again
				continue;
			}
			else							// No memory can be found so attempting to request more
			{
				current->next = malloc(sizeof(struct dataNode));
				if( current->next != NULL )
				{
					current->next->mem = malloc((NUM_ELEMENTS*(elSize+1)));
					if( current->next->mem != NULL )
					{
						--i;
						continue;				// we want to retry the allocation now that we have expanded our memory
					}
				}
				break;	// if the allocation failed, break out (drops to NULL return)
				
			}
		}
	}

	// for now just returning rootnodes memory region
	//return rootNode->memRegion;
	return NULL;
}