/**
 * @file slabAllocater.c
 * @author Artur BRAGA
 * Manages the memory that is allocated
 *
 */

#include <slabmemory.h>
#include <stdlib.h>


void slabInit(uint numEl)
{
	dataCache=(struct dataType)malloc(6*sizeof(struct dataType));
	dataCache[0].size=sizeof(uchar);
	dataCache[0].memList=(struct dataNode)malloc(sizeof(struct dataNode));
	dataCache[0].memList->mem=malloc(numEl*(sizeof(uchar)+1);
	dataCache[1].size=sizeof(ushort);
	dataCache[1].memList=(struct dataNode)malloc(sizeof(struct dataNode));
	dataCache[1].memList->mem=malloc(numEl*(sizeof(ushort)+1);
	dataCache[2].size=sizeof(uint);
	dataCache[2].memList=(struct dataNode)malloc(sizeof(struct dataNode));
	dataCache[2].memList->mem=malloc(numEl*(sizeof(uint)+1);
	dataCache[3].size=sizeof(ulong);
	dataCache[3].memList=(struct dataNode)malloc(sizeof(struct dataNode));
	dataCache[3].memList->mem=malloc(numEl*(sizeof(ulong)+1);
	dataCache[4].size=sizeof(bool);
	dataCache[4].memList=(struct dataNode)malloc(sizeof(struct dataNode));
	dataCache[4].memList->mem=malloc(numEl*(sizeof(bool)+1);
}

void* slabAlloc(uint numBytes)
{
	if( rootNode == NULL )
	{
		slabInit();
	}
	
	// for now just returning rootnodes memory region
	return rootNode->memRegion;
}