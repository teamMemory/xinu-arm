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

void* slabMalloc(uint numEl,uint elSize)
{
	int i,j;
	if( dataCache == NULL )
	{
		slabInit(numEl);
	}
	
	for( i = 0; i < NBDATASTR; ++i )
	{
		if( dataCache[ i ].size == elSize)
		{
			void* tempMemory = NULL;
			do
			{
				struct dataNode *current = dataCache[i].memList;
				if( !current->isTaken )
				{
					tempMemory = current->mem;
					break;
				}
			}while( dataCache[i].memList->next != NULL );
			
			if( tempMemory != NULL )
			{
				for( j = 0; j < numEl; ++j )
				{
					bool* tempMemoryFull = (tempMemory + j * (elSize + 1) );
					if( *tempMemoryFull == FALSE )
					{
						*tempMemoryFull = TRUE;
						return (void*)(&(*tempMemoryFull));
					}
				}
			}
			else	// No memory can be found so breaking out
			{
				break;
			}
		}
	}
	// for now just returning rootnodes memory region
	///return rootNode->memRegion;
	return NULL;
}