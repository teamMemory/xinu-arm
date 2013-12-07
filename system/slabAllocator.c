/**
 * @file slabAllocater.c
 * @author Artur Braga; Blake Gross
 * Manages the memory that is allocated
 *
 */

#include <slabmemory.h>
#include <stdlib.h>
#include <stdio.h>

struct SlabCacheList *cacheHead;
void* memBlock;

struct MemRange *freeMem,*usedMem;

void slabInit()
{
	//alocating the memory pull
	memBlock = malloc( TOTAL_MEMORY );
	
	//usedMem node is the first to be in the memory
	usedMem=(struct MemRange *)memBlock ;
	usedMem->pPrev=NULL;
	usedMem->pNext=NULL;
	usedMem->base=memBlock;
	//a node of usedMem + node of freeMem+ node of  the first cache
	usedMem->nbBytes=2*sizeof(struct MemRange)+sizeof(struct SlabCacheList);
	//usedMem->nbPages=1; //////////////To define!!!!!!!!!!!!!!!!!!!!///////////
	usedMem->pSlab=NULL;
	
	//alocating freeMem just after the used mem node
	freeMem=(struct MemRange *)((char *)memBlock+sizeof(struct MemRange));                       ////////////////////////////////Workaround////////////////////////////////
	freeMem->pPrev=NULL;
	freeMem->pNext=NULL;
	freeMem->base=(void * )((char *)usedMem->base+usedMem->nbBytes);
	freeMem->nbBytes=TOTAL_MEMORY-usedMem->nbBytes;
	//freeMem->nbPages=0; //////////////To define!!!!!!!!!!!!!!!!!!!!///////////
	freeMem->pSlab=NULL;
	

	cacheHead=(struct SlabCacheList *)((char * ) memBlock+ 2*sizeof(struct MemRange));
	cacheHead->pNext=NULL;
	cacheHead->pPrev=NULL;
	cacheHead->allocSize=-1;
	cacheHead->freeObj=-1;
	cacheHead->slabObjCnt=-1;
	cacheHead->objSize=-1;
	cacheHead->pSlabList = NULL;
}


struct Slab * createNewSlab(struct SlabCacheList *cache)
{
	uint size = 0;
	struct Slab *slabEl = NULL,*lastSlab = NULL;
	struct MemRange *current = NULL,*usedrange = NULL;
	current=freeMem;
	size=sizeof(struct Slab)+BUFFER;
	//Going through the list of free memory chunks
	while( current != NULL )
	{
		if( current->nbBytes>= size)
		{
			break;
		}
		current = current->pNext;
	}
	if( current==NULL) 
		{
			printf("There is not enough memory to create the cache!!!\n");
	}else{
		//Get the last slab in the cache
		lastSlab=cache->pSlabList;
		while( lastSlab->pNext != NULL )lastSlab=lastSlab->pNext;

		//Allocate slab header in the found freeMem
		slabEl=(struct Slab *)current->base+sizeof(struct MemRange);
		slabEl->firstObj=(void * )((char *)slabEl+sizeof(struct Slab));
		slabEl->nbFree=cache->slabObjCnt;
		slabEl->nbTotal=cache->slabObjCnt;
		slabEl->pCache=cache;
		slabEl->pRange=current;

		//create the buffers of objects
		slabEl->pFree=createBuffer(slabEl->firstObj,cache->allocSize);

		//appending the new slab to the list of slabs
		lastSlab->pNext=slabEl;
		slabEl->pNext=NULL;
		slabEl->pPrev=lastSlab;
		cache->freeObj+=cache->slabObjCnt;

				usedrange=usedMem;
		while( usedrange->pNext != NULL )usedrange=usedrange->pNext;
		
		
		if(current->nbBytes-size>sizeof(struct MemRange))
		{
			struct MemRange *freeNode=NULL;
			freeNode=(struct MemRange *)((char *)current->base+size+sizeof(struct MemRange));
			freeNode->base=(void *)freeNode;
			freeNode->nbBytes=current->nbBytes-size-sizeof(struct MemRange);
			freeNode->pSlab=NULL;
			freeNode->pNext=NULL;
			freeNode->pPrev=current->pPrev;

			if (current->pPrev!=NULL)current->pPrev->pNext=freeNode;
			else freeMem=freeNode;

			current->pPrev=usedrange;
			usedrange->pNext=current;
			current->pNext=NULL;
			current->nbBytes=size;
			current->pSlab=slabEl;
		}else{
		//Unmap the used memory from the free list
		current->pPrev=usedrange;
		usedrange->pNext=current;
		current->pNext=NULL;
		current->nbBytes=size+sizeof(struct MemRange);
		current->pSlab=slabEl;
		}
	}
	return slabEl;
}
struct SlabCacheList *createCache(uint objSize)
{
	struct SlabCacheList *lastCache = NULL;
	struct SlabCacheList *cacheEl = NULL;
	struct Slab *slabEl = NULL;
	struct MemRange *current = NULL,*usedrange = NULL;
	uint size = 0;
	size=sizeof(struct SlabCacheList)+sizeof(struct Slab)+BUFFER;
	current=freeMem;
	
	//Going through the list of free memory chunks
	while( current != NULL )
	{
		if( current->nbBytes>= size)
		{
			break;
		}
		current = current->pNext;
	}
	if( current==NULL) 
		{
			printf("There is not enough memory to create the cache!!!\n");
	}else{
		lastCache=cacheHead;
		
		while(lastCache->pNext !=NULL)
		{
			lastCache=lastCache->pNext;
		}
		
		cacheEl=(struct SlabCacheList *)((char *)current->base+sizeof(struct MemRange));
		lastCache->pNext=cacheEl;
		cacheEl->pPrev=lastCache;
		cacheEl->pNext=NULL;
		cacheEl->objSize=objSize;
		cacheEl->allocSize=alignMemory(objSize);
		cacheEl->slabObjCnt=(uint)(BUFFER/(sizeof(struct BufferList)+objSize));
		cacheEl->freeObj=cacheEl->slabObjCnt;


		slabEl=(struct Slab *)((char *)cacheEl+sizeof(struct SlabCacheList));
		cacheEl->pSlabList=slabEl;
		slabEl->pPrev=NULL;
		slabEl->pNext=NULL;
		slabEl->pCache=cacheEl;
		slabEl->pRange=current;
		slabEl->nbTotal=cacheEl->slabObjCnt;

		slabEl->firstObj=(void *)((char *)slabEl+sizeof(struct Slab));
		slabEl->pFree=createBuffer(slabEl->firstObj,cacheEl->allocSize);
		slabEl->nbFree=cacheEl->slabObjCnt;

		usedrange=usedMem;
		while( usedrange->pNext != NULL )usedrange=usedrange->pNext;
		
		
		if(current->nbBytes-size>sizeof(struct MemRange))
		{
			struct MemRange *freeNode;
			freeNode=(struct MemRange *)((char *)current->base+size);
			freeNode->base=(void *)((char *)current->base+size+sizeof(struct MemRange));
			freeNode->nbBytes=current->nbBytes-size-sizeof(struct MemRange);
			freeNode->pSlab=NULL;
			freeNode->pNext=NULL;
			freeNode->pPrev=current->pPrev;

			if (current->pPrev!=NULL)current->pPrev->pNext=freeNode;
			else freeMem=freeNode;

			current->pPrev=usedrange;
			usedrange->pNext=current;
			current->pNext=NULL;
			current->nbBytes=size;
			current->pSlab=slabEl;
		}else{
		//Unmap the used memory from the free list
		current->pPrev=usedrange;
		usedrange->pNext=current;
		current->pNext=NULL;
		current->nbBytes=size+sizeof(struct MemRange);
		current->pSlab=slabEl;
		}

	}
	return cacheEl;
}

uint cacheDestroy(struct SlabCacheList *pCache)
{
	struct Slab* startSlab = pCache->pSlabList;
	struct Slab* currentSlab = pCache->pSlabList;
	struct Slab* nextSlab = NULL;
	if( currentSlab != NULL )
	{
		do
		{
			nextSlab = currentSlab->pNext;
			slabDestroy( currentSlab );

			if( currentSlab->pPrev )
			{
				currentSlab->pPrev->pNext = currentSlab->pNext;
			}
			if( currentSlab->pNext )
			{
				currentSlab->pNext->pPrev = currentSlab->pPrev;
			}
			
			currentSlab->pPrev = 0;
			currentSlab->pNext = 0;

			currentSlab = nextSlab;
		}while( currentSlab != NULL && currentSlab != startSlab);
	}

	return 1;
}

uint slabDestroy(struct Slab* slab)
{
	struct Slab* startSlab = slab;
	struct Slab* currentSlab = slab;
	
	if( slab != NULL )
	{
		do
		{
			struct MemRange* currentMem = slab->pRange;			// takes the memrange off of the used list
			if( currentMem->pPrev )
			{
				currentMem->pPrev->pNext = currentMem->pNext;
			}

			if( currentMem->pNext )
			{
				currentMem->pNext->pPrev = currentMem->pPrev;
			}
			
			struct MemRange* currentFreeMem = freeMem;
			while( currentFreeMem->pNext != NULL )	// need to get to end of list
			{
				currentFreeMem = currentFreeMem->pNext;
			}
			currentFreeMem->pNext = currentMem;
			currentMem->pPrev = currentFreeMem;
			
			currentSlab = currentSlab->pNext;
		}while( currentSlab != NULL && startSlab != currentSlab );
	}

	return 1;
}

struct BufferList* createBuffer(void *base,uint objSize)
{
	void * addrSpace = NULL;
	int i = 0;
	struct BufferList* prev = NULL; 
	uint nbrObjects = (int)(BUFFER/(sizeof(struct BufferList)+objSize));
	addrSpace=base;
	prev=(struct BufferList *)addrSpace;
	
	prev->pPrev=NULL;
	prev->pNext = NULL;
	prev->pObject=(void *)((char*)addrSpace+(sizeof(struct BufferList)));
	addrSpace=(void *)((char *)prev->pObject+objSize);

	for( i=1;i<nbrObjects;++i)
	{
		struct BufferList* temp =(struct BufferList *)addrSpace;
		prev->pNext=temp;
		temp->pPrev=prev;
		temp->pObject=(void *)((char*)addrSpace+(sizeof(struct BufferList)));
		addrSpace= (void *)((char *)temp->pObject+objSize);
		prev=temp;
	}
	prev->pNext=(struct BufferList *)base;
	prev->pNext->pPrev=prev;
	return prev->pNext;
}
/*void slabInit()
{
	memBlock = malloc( TOTAL_MEMORY );
}*/

void* slabAlloc(uint elSize)
{
	int i = 0,j = 0;
	struct Slab* currentSlab = NULL;
	void* memoryToReturn = NULL;
	struct SlabCacheList* cacheToUse = cacheHead;
	while( cacheToUse != NULL )
	{
		if( cacheToUse->objSize == elSize )
		{
			break;
		}
		cacheToUse = cacheToUse->pNext;
	}
	
	if( cacheToUse == NULL )
	{
		cacheToUse = createCache(elSize);
	}
	
	currentSlab = cacheToUse->pSlabList;	// need to get way of getting this....
	
	
	if (cacheToUse->freeObj>0)
	{
		while( currentSlab != NULL )
		{
			if( currentSlab->nbFree > 0 )	// if the slab has free space, we can allocate in it
			{
				struct BufferList* headBuffer = currentSlab->pFree;
				memoryToReturn = headBuffer->pObject;		// first object on list is always going to be free, now we have to take if off the list (works like a queue)

				
					headBuffer->pNext->pPrev = headBuffer->pPrev;	// reordering the list
					headBuffer->pPrev->pNext = headBuffer->pNext;
				
				currentSlab->pFree = headBuffer->pNext;
				headBuffer->pNext = NULL;
				headBuffer->pPrev = NULL;
				cacheToUse->freeObj--;
				currentSlab->nbFree--;
				break;
			}
		
			currentSlab = currentSlab->pNext;
		}
	}else{
		struct BufferList* headBuffer = createNewSlab(cacheToUse)->pFree;
		memoryToReturn = headBuffer->pObject;		// first object on list is always going to be free, now we have to take if off the list (works like a queue)

		if( headBuffer->pNext != NULL )
		{
			headBuffer->pNext->pPrev = headBuffer->pPrev;	// reordering the list
			headBuffer->pPrev->pNext = headBuffer->pNext;
		}
		currentSlab->pFree = headBuffer->pNext;
		headBuffer->pNext = NULL;
		headBuffer->pPrev = NULL;
		cacheToUse->freeObj--;
		currentSlab->nbFree--;
	}

	// for now just returning rootnodes memory region
	//return rootNode->memRegion;
	return memoryToReturn;
}

uint slabFree( void* objectToFree )
{
	struct SlabCacheList* cacheToUse = cacheHead;	// cache space is non-contiguous by each slab is, by checking slab ranges we can search for the node
	while( cacheToUse != NULL )
	{
		struct Slab* currentSlab = cacheToUse->pSlabList;
		while( currentSlab != NULL )
		{
			struct MemRange* currentRange = currentSlab->pRange;
			if( currentRange->base <= objectToFree && (char *)currentRange->base + currentRange->nbBytes >= objectToFree)	// if within range need to check...
			{
				struct BufferList* currentBuffer =(struct BufferList*) currentSlab->firstObj;
				
				while( currentBuffer != NULL )
				{
					if( currentBuffer->pObject == objectToFree )
					{
						if( currentSlab->nbFree > 0 )
						{
							currentSlab->pFree->pPrev->pNext = currentBuffer;
							currentBuffer->pPrev = currentSlab->pFree->pPrev;
							currentBuffer->pNext = currentSlab->pFree;
							currentSlab->pFree->pPrev = currentBuffer;
						}
						else
						{
							currentSlab->pFree = currentBuffer;
						}
						
						++currentSlab->nbFree;
						++currentSlab->pCache->freeObj;

						if( currentSlab->nbFree == currentSlab->nbTotal )	// slab is useless if freed
						{
							slabDestroy( currentSlab );
						}
						return 1;	// we did our job ^_^
					}
					
					currentBuffer = (struct BufferList *)((char *)(currentBuffer) + sizeof( struct BufferList*));
				}

			}
			currentSlab = currentSlab->pNext;
		}
		cacheToUse = cacheToUse->pNext;
	}
	
	return 0;	// we failed.... the memory will never roam free
}

/*struct SlabCacheList *createCache(uint size)
{
	if( memBlock == NULL )
	{
		slabInit();
	}
	
	return 	newCache = (struct SlabCacheList*)(&memBlock->prev + sizeof(struct SlabCacheList);	// creates new cache from memory
}*/

//http://stackoverflow.com/questions/227897/solve-the-memory-alignment-in-c-interview-question-that-stumped-me
uint alignMemory(uint objSize)
{
	return objSize + 3 & ~0x03;
}

void slabCleanup()	// really just free...
{
	struct SlabCacheList* currentSlab = cacheHead;
	struct SlabCacheList* nextSlab = NULL;
	while( currentSlab != NULL )
	{
		nextSlab = currentSlab->pNext;
		cacheDestroy( currentSlab );
		currentSlab = nextSlab;
	}
	
	free( memBlock );
}
